/*
 * Copyright 2019 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "tuningfork_utils.h"
#include "tuningfork/protobuf_util.h"

#include <cstdio>
#include <sys/stat.h>
#include <errno.h>
#include <fstream>
#include <sstream>
// Unfortunately we can't use std::filesystem until we move to C++17
#include <dirent.h>

#define LOG_TAG "TuningFork"
#include "Log.h"
#include "file_cache.h"

#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>

namespace tuningfork {

static FileCache sFileCache;

// Use the default persister if the one passed in is null
static void CheckPersister(const TFCache*& persister, std::string save_dir) {
    if (persister == nullptr) {
        if (save_dir.empty())
            save_dir = "/data/local/tmp/tuningfork";
        ALOGI("Using local file cache at %s", save_dir.c_str());
        sFileCache.SetDir(save_dir);
        persister = sFileCache.GetCCache();
    }
}

void CopySettings(const TFSettings &c_settings, const std::string& save_dir,
                  Settings &settings_out) {
    auto& a = settings_out.aggregation_strategy;
    auto& ca = c_settings.aggregation_strategy;
    a.intervalms_or_count = ca.intervalms_or_count;
    a.max_instrumentation_keys = ca.max_instrumentation_keys;
    a.method = ca.method==TFAggregationStrategy::TICK_BASED?
                 Settings::AggregationStrategy::TICK_BASED:
                 Settings::AggregationStrategy::TIME_BASED;
    a.annotation_enum_size = std::vector<uint32_t>(ca.annotation_enum_size,
                                        ca.annotation_enum_size + ca.n_annotation_enum_size);
    settings_out.histograms = std::vector<TFHistogram>(c_settings.histograms,
                                        c_settings.histograms + c_settings.n_histograms);
    settings_out.persistent_cache = c_settings.persistent_cache;
    CheckPersister(settings_out.persistent_cache, save_dir);
    settings_out.base_uri = std::string(c_settings.base_uri?c_settings.base_uri:"");
    settings_out.api_key = std::string(c_settings.api_key?c_settings.api_key:"");
}

namespace apk_utils {

    class AssetManagerHelper {
          AAssetManager* mgr_;
          jobject jmgr_;
          JNIEnv* env_;
     public:
          AssetManagerHelper(JNIEnv* env, jobject context) : mgr_(nullptr), jmgr_(nullptr),
                                                             env_(env) {
              jclass cls = env->FindClass("android/content/Context");
              jmethodID get_assets = env->GetMethodID(cls, "getAssets",
                                                      "()Landroid/content/res/AssetManager;");
              if(get_assets==nullptr) {
                ALOGE("No Context.getAssets() method");
                return;
              }
              jmgr_ = env->CallObjectMethod(context, get_assets);
              if (jmgr_ == nullptr) {
                ALOGE("No java asset manager");
                return;
              }
              mgr_ = AAssetManager_fromJava(env, jmgr_);
              if (mgr_ == nullptr) {
                ALOGE("No asset manager");
                return;
              }
          }
          AAsset* GetAsset(const char* name) {
              if (mgr_)
                  return AAssetManager_open(mgr_, name, AASSET_MODE_BUFFER);
              else
                  return nullptr;
          }
          ~AssetManagerHelper() {
              env_->DeleteLocalRef(jmgr_);
          }
    };

    // Get an asset from this APK's asset directory.
    // Returns NULL if the asset could not be found.
    // Asset_close must be called once the asset is no longer needed.
    AAsset* GetAsset(JNIEnv* env, jobject context, const char* name) {
        AssetManagerHelper mgr(env, context);
        AAsset* asset = mgr.GetAsset(name);
        if (asset == nullptr) {
            ALOGW("Can't find %s in APK", name);
            return nullptr;
        }
        return asset;
    }

    // Get the app's version code. Also fills packageNameStr with the package name
    //  if it is non-null.
    int GetVersionCode(JNIEnv *env, jobject context, std::string* packageNameStr) {
        jstring packageName;
        jobject packageManagerObj;
        jobject packageInfoObj;
        jclass contextClass =  env->GetObjectClass( context);
        jmethodID getPackageNameMid = env->GetMethodID( contextClass, "getPackageName",
            "()Ljava/lang/String;");
        jmethodID getPackageManager =  env->GetMethodID( contextClass, "getPackageManager",
            "()Landroid/content/pm/PackageManager;");

        jclass packageManagerClass = env->FindClass("android/content/pm/PackageManager");
        jmethodID getPackageInfo = env->GetMethodID( packageManagerClass, "getPackageInfo",
            "(Ljava/lang/String;I)Landroid/content/pm/PackageInfo;");

        jclass packageInfoClass = env->FindClass("android/content/pm/PackageInfo");
        jfieldID versionCodeFid = env->GetFieldID( packageInfoClass, "versionCode", "I");

        packageName =  (jstring)env->CallObjectMethod(context, getPackageNameMid);

        if (packageNameStr != nullptr) {
            // Fill packageNameStr with the package name
            const char* packageName_cstr = env->GetStringUTFChars(packageName, NULL);
            *packageNameStr = std::string(packageName_cstr);
            env->ReleaseStringUTFChars(packageName, packageName_cstr);
        }
        // Get version code from package info
        packageManagerObj = env->CallObjectMethod(context, getPackageManager);
        packageInfoObj = env->CallObjectMethod(packageManagerObj,getPackageInfo,
                                               packageName, 0x0);
        int versionCode = env->GetIntField( packageInfoObj, versionCodeFid);
        return versionCode;
    }

} // namespace apk_utils

namespace file_utils {

    // Creates the directory if it does not exist. Returns true if the directory
    //  already existed or could be created.
    bool CheckAndCreateDir(const std::string& path) {
        ALOGV("CheckAndCreateDir:%s",path.c_str());
        struct stat sb;
        int32_t res = stat(path.c_str(), &sb);
        if (0 == res && sb.st_mode & S_IFDIR) {
            ALOGV("Directory %s already exists", path.c_str());
            return true;
        } else if (ENOENT == errno) {
            ALOGI("Creating directory %s", path.c_str());
            res = mkdir(path.c_str(), 0770);
            if(res!=0)
                ALOGW("Error creating directory %s: %d", path.c_str(), res);
            return res==0;
        }
        return false;
    }
    bool FileExists(const std::string& fname) {
        ALOGV("FileExists:%s",fname.c_str());
        struct stat buffer;
        return (stat(fname.c_str(), &buffer)==0);
    }
    std::string GetAppCacheDir(JNIEnv* env, jobject context) {
        jclass contextClass = env->GetObjectClass(context);
        jmethodID getCacheDir = env->GetMethodID( contextClass, "getCacheDir",
            "()Ljava/io/File;" );
        jobject cache_dir = env->CallObjectMethod( context, getCacheDir );

        jclass fileClass = env->FindClass( "java/io/File" );
        jmethodID getPath = env->GetMethodID( fileClass, "getPath", "()Ljava/lang/String;" );
        jstring path_string = (jstring)env->CallObjectMethod( cache_dir, getPath );

        const char *path_chars = env->GetStringUTFChars( path_string, NULL );
        std::string temp_folder( path_chars );
        env->ReleaseStringUTFChars( path_string, path_chars );

        return temp_folder;
    }
    bool DeleteFile(const std::string& path) {
        if (FileExists(path))
            return remove(path.c_str())==0;
        return true;
    }
    bool DeleteDir(const std::string& path) {
        // TODO(willosborn): It would be much better to use std::filesystem::remove_all here
        //  if we were on C++17 or experimental/filesystem was supported on Android.
        ALOGI("DeleteDir %s", path.c_str());
        struct dirent* entry;
        auto dir = opendir(path.c_str());
        if (dir==nullptr)
            return DeleteFile(path);
        while ((entry = readdir(dir))) {
            if (entry->d_name[0]!='\0' && entry->d_name[0]!='.')
                DeleteDir(path + "/" + entry->d_name);
        }
        closedir(dir);
        return true;
    }

    bool LoadBytesFromFile(std::string file_name, CProtobufSerialization* params) {
        ALOGV("LoadBytesFromFile:%s",file_name.c_str());
        std::ifstream f(file_name, std::ios::binary);
        if (f.good()) {
            f.seekg(0, std::ios::end);
            params->size = f.tellg();
            params->bytes = (uint8_t*)::malloc(params->size);
            params->dealloc = CProtobufSerialization_Dealloc;
            f.seekg(0, std::ios::beg);
            f.read((char*)params->bytes, params->size);
            return true;
        }
        return false;
    }

    bool SaveBytesToFile(std::string file_name, const CProtobufSerialization* params) {
        ALOGV("SaveBytesToFile:%s",file_name.c_str());
        std::ofstream save_file(file_name, std::ios::binary);
        if (save_file.good()) {
            save_file.write((const char*)params->bytes, params->size);
            return true;
        }
        return false;
    }

} // namespace file_utils

namespace json_utils {

using namespace json11;

std::string GetResourceName(const ExtraUploadInfo& request_info) {
    std::stringstream str;
    str << "applications/"<< request_info.apk_package_name<<"/apks/";
    str << request_info.apk_version_code;
    return str.str();
}

Json::object DeviceSpecJson(const ExtraUploadInfo& request_info) {
    Json gles_version = Json::object {
        {"major", static_cast<int>(request_info.gl_es_version>>16)},
        {"minor", static_cast<int>(request_info.gl_es_version&0xffff)}};
    std::vector<double> freqs(request_info.cpu_max_freq_hz.begin(),
                              request_info.cpu_max_freq_hz.end());
    return Json::object {
        {"fingerprint", request_info.build_fingerprint},
        {"total_memory_bytes", static_cast<double>(request_info.total_memory_bytes)},
        {"build_version", request_info.build_version_sdk},
        {"gles_version", gles_version},
        {"cpu_core_freqs_hz", freqs}};
}

} // namespace json_utils

std::string UniqueId(JNIEnv* env) {
    jclass uuid_class = env->FindClass("java/util/UUID");
    jmethodID randomUUID = env->GetStaticMethodID( uuid_class, "randomUUID",
            "()Ljava/util/UUID;");
    jobject uuid = env->CallStaticObjectMethod(uuid_class, randomUUID);
    jmethodID toString = env->GetMethodID( uuid_class, "toString", "()Ljava/lang/String;");
    jstring uuid_string = (jstring)env->CallObjectMethod(uuid, toString);
    const char *uuid_chars = env->GetStringUTFChars( uuid_string, NULL );
    std::string temp_uuid( uuid_chars );
    env->ReleaseStringUTFChars( uuid_string, uuid_chars );
    return temp_uuid;
}

} // namespace tuningfork
