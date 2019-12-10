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

#include "System.hpp"

#include <algorithm>
#include <array>
#include <thread>

#include <cpu-features.h>
#include <device_info/device_info.h>

#include "util/FpsCalculator.hpp"
#include "util/GLHelpers.hpp"
#include "util/Error.hpp"
#include "util/Log.hpp"

using namespace ancer;


//==============================================================================

namespace {
    Log::Tag TAG{"ancer::system"};
    jobject _activity_weak_global_ref;
    JavaVM* _java_vm;

    std::filesystem::path _internal_data_path;
    std::filesystem::path _raw_data_path;
    std::filesystem::path _obb_path;
}

//==============================================================================

namespace {
    // Returns if we need to be detached.
    bool GetEnv(JNIEnv** env) {
        int env_stat = _java_vm->GetEnv((void**)env, JNI_VERSION_1_6);
        if ( env_stat == JNI_EDETACHED) {
            if ( _java_vm->AttachCurrentThread(env, nullptr) != 0 ) {
                FatalError(TAG, "GetEnv - Failed to attach current thread");
            }
            return true;
        }
        return false; // Already attached
    }

    class AttachedEnv final {
    public:
        AttachedEnv() { _need_detach = GetEnv(&_env); }

        ~AttachedEnv() { if ( _need_detach ) _java_vm->DetachCurrentThread(); }

        JNIEnv* env() const noexcept { return _env; }

    private:
        JNIEnv* _env;
        bool _need_detach;
    };

    template <typename Func>
    void JniCallInAttachedThread(Func&& func) {
        try {
            return func(AttachedEnv{}.env());
        } catch ( const std::exception& e ) {
            FatalError(TAG, e.what());
        }
    }
} // anonymous namespace

//==============================================================================

void ancer::internal::SetJavaVM(JavaVM* vm) {
    _java_vm = vm;
}

void ancer::internal::InitSystem(jobject activity, jstring internal_data_path,
                                 jstring raw_data_path, jstring obb_path) {
    JniCallInAttachedThread([&](JNIEnv* env) {
        _activity_weak_global_ref = env->NewWeakGlobalRef(activity);

        _internal_data_path = std::filesystem::path(
                env->GetStringUTFChars(internal_data_path, nullptr));
        _raw_data_path = std::filesystem::path(
                env->GetStringUTFChars(raw_data_path, nullptr));
        _obb_path = std::filesystem::path(
                env->GetStringUTFChars(obb_path, nullptr));
    });
}


void ancer::internal::DeinitSystem() {
    JniCallInAttachedThread([](JNIEnv* env) {
        env->DeleteWeakGlobalRef(_activity_weak_global_ref);
    });
}

//==================================================================================================

std::string ancer::InternalDataPath() {
    return _internal_data_path.string();
}


std::string ancer::RawResourcePath() {
    return _raw_data_path.string();
}

std::string ancer::ObbPath() {
    return _obb_path.string();
}

//==============================================================================

std::string ancer::LoadText(const char* file_name) {
    std::string text;
    JniCallInAttachedThread(
            [file_name, &text](JNIEnv* env) {
                jstring name = env->NewStringUTF(file_name);
                jobject activity = env->NewLocalRef(_activity_weak_global_ref);
                jclass activity_class = env->GetObjectClass(activity);

                jmethodID get_asset_helpers_mid = env->GetMethodID(
                        activity_class, "getSystemHelpers",
                        "()Lcom/google/gamesdk/gamecert/operationrunner/util/SystemHelpers;");
                jobject asset_helpers_instance =
                    env->CallObjectMethod(activity, get_asset_helpers_mid);
                jclass asset_helpers_class =
                    env->GetObjectClass(asset_helpers_instance);


                jmethodID load_text_mid = env->GetMethodID(
                        asset_helpers_class, "loadText",
                        "(Ljava/lang/String;)Ljava/lang/String;");
                auto result_j_string = (jstring)env->CallObjectMethod(
                        asset_helpers_instance, load_text_mid,
                        name);

                if ( result_j_string ) {
                    const char* result_utf_ptr =
                        env->GetStringUTFChars(result_j_string, nullptr);
                    text = std::string(result_utf_ptr);
                    env->ReleaseStringUTFChars(result_j_string, result_utf_ptr);
                }

                env->DeleteLocalRef(name);
                env->DeleteLocalRef(activity);
            });
    return text;
}

//==============================================================================

GLuint ancer::CreateProgram(const char* vtx_file_name, const char* frg_file_name) {
    std::string vtx_src = LoadText(vtx_file_name);
    std::string frag_src = LoadText(frg_file_name);
    if ( !vtx_src.empty() && !frag_src.empty()) {
        return glh::CreateProgramSrc(vtx_src.c_str(), frag_src.c_str());
    }
    return 0;
}

//==============================================================================

GLuint ancer::LoadTexture(
        const char* file_name, int32_t* out_width,
        int32_t* out_height, bool* has_alpha) {
    GLuint tex = 0;
    JniCallInAttachedThread(
            [&tex, &out_width, &out_height, &has_alpha, file_name](JNIEnv* env) {
                jstring name = env->NewStringUTF(file_name);
                jobject activity = env->NewLocalRef(_activity_weak_global_ref);
                jclass activity_class = env->GetObjectClass(activity);

                glGenTextures(1, &tex);
                glBindTexture(GL_TEXTURE_2D, tex);
                glTexParameterf(GL_TEXTURE_2D,
                    GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
                glTexParameterf(GL_TEXTURE_2D,
                    GL_TEXTURE_MAG_FILTER, GL_LINEAR);

                jmethodID get_asset_helpers_mid = env->GetMethodID(
                        activity_class,
                        "getSystemHelpers",
                        "()Lcom/google/gamesdk/gamecert/operationrunner/util/SystemHelpers;");
                jobject asset_helpers_instance = env->CallObjectMethod(
                        activity,
                        get_asset_helpers_mid);
                jclass asset_helpers_class =
                    env->GetObjectClass(asset_helpers_instance);

                jmethodID load_texture_mid = env->GetMethodID(
                        asset_helpers_class, "loadTexture",
                        "(Ljava/lang/String;)Lcom/google/gamesdk/gamecert/operationrunner/util/SystemHelpers$TextureInformation;");

                jobject out = env->CallObjectMethod(asset_helpers_instance, load_texture_mid, name);

                // extract TextureInformation from out
                jclass java_cls = RetrieveClass(
                        env, activity,
                        "com/google/gamesdk/gamecert/operationrunner/util/SystemHelpers$TextureInformation");
                jfieldID fid_ret = env->GetFieldID(java_cls, "ret", "Z");
                jfieldID fid_has_alpha = env->GetFieldID(java_cls, "alphaChannel", "Z");
                jfieldID fid_width = env->GetFieldID(java_cls, "originalWidth", "I");
                jfieldID fid_height = env->GetFieldID(java_cls, "originalHeight", "I");
                bool ret = env->GetBooleanField(out, fid_ret);
                bool alpha = env->GetBooleanField(out, fid_has_alpha);
                int32_t width = env->GetIntField(out, fid_width);
                int32_t height = env->GetIntField(out, fid_height);

                if ( !ret ) {
                    glDeleteTextures(1, &tex);
                    *out_width = 0;
                    *out_height = 0;
                    if ( has_alpha ) *has_alpha = false;
                    FatalError(TAG, "loadTexture - unable to load \"%s\"",
                        file_name);
                } else {
                    *out_width = width;
                    *out_height = height;
                    if ( has_alpha ) *has_alpha = alpha;

                    // Generate mipmap
                    glGenerateMipmap(GL_TEXTURE_2D);
                }

                env->DeleteLocalRef(name);
                env->DeleteLocalRef(activity);
            });

    return tex;
}

//==============================================================================

MemoryInfo ancer::GetMemoryInfo() {
    auto info = MemoryInfo{};
    JniCallInAttachedThread(
            [&info](JNIEnv* env) {
                jobject activity = env->NewLocalRef(_activity_weak_global_ref);
                jclass activity_class = env->GetObjectClass(activity);

                // get the memory info object from our activity

                jmethodID get_system_helpers_mid = env->GetMethodID(
                        activity_class,
                        "getSystemHelpers",
                        "()Lcom/google/gamesdk/gamecert/operationrunner/util/SystemHelpers;");
                jobject system_helpers_instance = env->CallObjectMethod(
                        activity,
                        get_system_helpers_mid);
                jclass system_helpers_class =
                    env->GetObjectClass(system_helpers_instance);

                jmethodID get_memory_info_mid = env->GetMethodID(
                        system_helpers_class, "getMemoryInfo",
                        "()Lcom/google/gamesdk/gamecert/operationrunner/util/SystemHelpers$MemoryInformation;");

                jobject j_info_obj = env->CallObjectMethod(system_helpers_instance, get_memory_info_mid);

                // get the field ids for the object

                jclass java_cls = RetrieveClass(
                        env, activity,
                        "com/google/gamesdk/gamecert/operationrunner/util/SystemHelpers$MemoryInformation");
                jfieldID fid_oom_score = env->GetFieldID(java_cls, "oomScore", "I");
                jfieldID fid_native_heap_allocation_size = env->GetFieldID(
                        java_cls,
                        "nativeHeapAllocationSize", "J");
                jfieldID fid_commit_limit = env->GetFieldID(java_cls, "commitLimit", "J");
                jfieldID fid_total_memory = env->GetFieldID(java_cls, "totalMemory", "J");
                jfieldID fid_available_memory = env->GetFieldID(java_cls, "availableMemory", "J");
                jfieldID fid_threshold = env->GetFieldID(java_cls, "threshold", "J");
                jfieldID fid_low_memory = env->GetFieldID(java_cls, "lowMemory", "Z");

                // finally fill out the struct

                info._oom_score = env->GetIntField(j_info_obj, fid_oom_score);
                info.native_heap_allocation_size = static_cast<long>(env->GetLongField(
                        j_info_obj,
                        fid_native_heap_allocation_size));
                info.commit_limit = static_cast<long>(env->GetLongField(j_info_obj, fid_commit_limit));
                info.total_memory = static_cast<long>(env->GetLongField(j_info_obj, fid_total_memory));
                info.available_memory = static_cast<long>(env->GetLongField(
                        j_info_obj,
                        fid_available_memory));
                info.low_memory_threshold = static_cast<long>(env->GetLongField(
                        j_info_obj, fid_threshold));
                info.low_memory = env->GetBooleanField(j_info_obj, fid_low_memory);
            });
    return info;
}

//==============================================================================

ThermalStatus ancer::GetThermalStatus() {
    ThermalStatus status = ThermalStatus::Unknown;

    JniCallInAttachedThread(
            [&status](JNIEnv* env) {
                jobject activity = env->NewLocalRef(_activity_weak_global_ref);
                jclass activity_class = env->GetObjectClass(activity);

                // get the memory info object from our activity

                jmethodID get_system_helpers_mid = env->GetMethodID(
                        activity_class,
                        "getSystemHelpers",
                        "()Lcom/google/gamesdk/gamecert/operationrunner/util/SystemHelpers;");
                jobject system_helpers_instance = env->CallObjectMethod(
                        activity,
                        get_system_helpers_mid);
                jclass system_helpers_class =
                    env->GetObjectClass(system_helpers_instance);

                jmethodID get_thermal_status_mid = env->GetMethodID(
                        system_helpers_class, "getCurrentThermalStatus", "()I");

                // we assume that the java api will return one of the defined thermal status
                // values, but the api is new as of Q and AFAIK only supported on pixel
                jint status_int =
                    env->CallIntMethod(system_helpers_instance, get_thermal_status_mid);
                if ( status_int >= 0 && status_int <= 6 ) {
                    status = static_cast<ThermalStatus>(status_int);
                }
            });

    return status;
}

//==============================================================================

void ancer::RunSystemGc() {
    JniCallInAttachedThread(
            [](JNIEnv* env) {
                jobject activity = env->NewLocalRef(_activity_weak_global_ref);
                jclass activity_class = env->GetObjectClass(activity);

                // get the memory info object from our activity

                jmethodID get_system_helpers_mid = env->GetMethodID(
                        activity_class,
                        "getSystemHelpers",
                        "()Lcom/google/gamesdk/gamecert/operationrunner/util/SystemHelpers;");
                jobject system_helpers_instance = env->CallObjectMethod(
                        activity,
                        get_system_helpers_mid);
                jclass system_helpers_class = env->GetObjectClass(system_helpers_instance);

                jmethodID run_gc_mid =
                    env->GetMethodID(system_helpers_class, "runGc", "()V");

                env->CallVoidMethod(system_helpers_instance, run_gc_mid);
            });
}

//==============================================================================

namespace {
    // -1 for little core, 0 for medium core, 1 for big core
    using AffinityOffset = int;
    [[nodiscard]] constexpr AffinityOffset ToAffinityOffset(ThreadAffinity aff) {
        assert (aff != ThreadAffinity::kAnyCore);
        return aff == ThreadAffinity::kLittleCore ? -1 :
               aff == ThreadAffinity::kMiddleCore ? 0 :
               1;
    }

    [[nodiscard]] constexpr ThreadAffinity FromAffinityOffset(AffinityOffset aff) {
        assert(-1 <= aff && aff <= 1);
        return (ThreadAffinity)((int)ThreadAffinity::kLittleCore + aff);
    }

    const auto core_info = [] {
        struct {
            std::array<int, 4> num_cores;
            std::vector<ThreadAffinity> core_sizes;
        } info;

        androidgamesdk_deviceinfo_GameSdkDeviceInfoWithErrors proto;
        androidgamesdk_deviceinfo::ProtoDataHolder data_holder;
        createProto(proto, data_holder);

        const auto num_cpus = std::min((int)data_holder.cpuFreqs.size, android_getCpuCount());
        if (data_holder.cpuFreqs.size != android_getCpuCount()) {
            Log::E(TAG, "GameSDK reports %d CPUs, but cpu-features reports %d.",
                   data_holder.cpuFreqs.size, android_getCpuCount());
        }

        Log::I(TAG, "%d", data_holder.errors.size);
        for (int i = 0; i < data_holder.errors.size; ++i) {
            const char* msg = data_holder.errors.data.get()[i].data.get();
            Log::E(TAG, "%d > %s", i, msg);
        }

        info.num_cores[(int)ThreadAffinity::kAnyCore] = num_cpus;
        info.core_sizes.reserve(num_cpus);


        auto min_freq = data_holder.cpuFreqs.data[0];
        auto max_freq = data_holder.cpuFreqs.data[0];

        for (int i = 0 ; i < num_cpus ; ++i) {
            const auto freq = data_holder.cpuFreqs.data[i];

            // Found a frequency outside of existing big/little range
            if (freq < min_freq || max_freq < freq) {
                // Determine which end we're replacing.
                const auto adj = ToAffinityOffset(
                        freq < min_freq ? ThreadAffinity::kLittleCore
                                        : ThreadAffinity::kBigCore);
                constexpr auto kMiddleIndex = (int)ThreadAffinity::kMiddleCore;
                if (min_freq == max_freq) {
                    // All data so far as been recorded as middle. Take that
                    // data and place it on the opposite end now that we've
                    // found an opposite frequency for a big/little pair.
                    std::swap(info.num_cores[kMiddleIndex],
                              info.num_cores[kMiddleIndex - adj]);
                    for (auto &size : info.core_sizes) {
                        size = (ThreadAffinity)((int)size - adj);
                    }
                } else {
                    // Since we've found a new frequency for big/little, take
                    // what we thought was the big/little end before and merge
                    // it with whatever's already in the middle.
                    info.num_cores[kMiddleIndex] += info.num_cores[kMiddleIndex + adj];
                    info.num_cores[kMiddleIndex + adj] = 0;
                    for (auto &size : info.core_sizes) {
                        if (size == FromAffinityOffset(adj)) {
                            size = ThreadAffinity::kMiddleCore;
                        }
                    }
                }
            }

            min_freq = std::min(min_freq, freq);
            max_freq = std::max(max_freq, freq);

            const auto which =
                    ((min_freq == max_freq) ||
                    (min_freq < freq && freq < max_freq)) ? ThreadAffinity::kMiddleCore
                    : freq == min_freq ? ThreadAffinity::kLittleCore
                    : ThreadAffinity::kBigCore;

            Log::D(TAG, "Core %d @ %d", i, freq);

            ++info.num_cores[(int)which];
            info.core_sizes.push_back(which);
        }

        Log::I(TAG, "%d cores detected : %d little @ %ld, %d big @ %ld, & %d middle)",
               info.num_cores[(int)ThreadAffinity::kAnyCore],
               info.num_cores[(int)ThreadAffinity::kLittleCore], min_freq,
               info.num_cores[(int)ThreadAffinity::kBigCore], max_freq,
               info.num_cores[(int)ThreadAffinity::kMiddleCore]);

        return info;
    }();
}


int ancer::NumCores(ThreadAffinity affinity) {
    return core_info.num_cores[(int)affinity];
}


void ancer::SetThreadAffinity(int index, ThreadAffinity affinity) {
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);

    // i is real CPU index, j is the index restricted to only the given affinity
    for (int i = 0, j = 0; i < std::size(core_info.core_sizes); ++i) {
        if (affinity != ThreadAffinity::kAnyCore &&
            affinity != core_info.core_sizes[i]) {
            continue;
        }
        if (j++ == index || index == -1) {
            CPU_SET(i, &cpuset);
            if (index != -1) break;
        }
    }

    if ( sched_setaffinity(0, sizeof(cpu_set_t), &cpuset) == -1 ) {
        std::string err;
        switch (errno) {
        case EFAULT:err = "EFAULT";
            break;
        case EINVAL:err = "EINVAL";
            break;
        case EPERM:err = "EPERM";
            break;
        case ESRCH:err = "ESRCH";
            break;
        default:err = "(errno: " + std::to_string(errno) + ")";
            break;
        }
        FatalError(TAG, "error setting thread affinity, error: " + err);
    }
}


void ancer::SetThreadAffinity(ThreadAffinity affinity) {
    SetThreadAffinity(-1, affinity);
}

//==============================================================================

jclass ancer::RetrieveClass(JNIEnv* env, jobject activity, const char* className) {
    jclass activity_class = env->GetObjectClass(activity);
    jmethodID get_class_loader = env->GetMethodID(
            activity_class, "getClassLoader", "()Ljava/lang/ClassLoader;");
    jobject cls = env->CallObjectMethod(activity, get_class_loader);
    jclass class_loader = env->FindClass("java/lang/ClassLoader");
    jmethodID find_class = env->GetMethodID(
            class_loader, "loadClass", "(Ljava/lang/String;)Ljava/lang/Class;");

    jstring jstr_class_name = env->NewStringUTF(className);
    auto class_retrieved =
        (jclass)env->CallObjectMethod(cls, find_class, jstr_class_name);
    env->DeleteLocalRef(jstr_class_name);
    env->DeleteLocalRef(activity_class);
    env->DeleteLocalRef(class_loader);
    return class_retrieved;
}

//==============================================================================

FpsCalculator& ancer::GetFpsCalculator() {
    static FpsCalculator fps_calc;
    return fps_calc;
}

void ancer::BridgeGLContextConfiguration(GLContextConfig src_config,
                                         jobject dst_config)
{
  JniCallInAttachedThread(
      [src_config, dst_config](JNIEnv* env) {
        jobject activity = env->NewLocalRef(_activity_weak_global_ref);

        jclass java_cls = RetrieveClass(
            env, activity,
            "com/google/gamesdk/gamecert/operationrunner/hosts/BaseGLHostActivity$GLContextConfiguration");

        jfieldID red_bits = env->GetFieldID(java_cls, "redBits", "I");
        jfieldID green_bits = env->GetFieldID(java_cls, "greenBits", "I");
        jfieldID blue_bits = env->GetFieldID(java_cls, "blueBits", "I");
        jfieldID alpha_bits = env->GetFieldID(java_cls, "alphaBits", "I");
        jfieldID depth_bits = env->GetFieldID(java_cls, "depthBits", "I");
        jfieldID stencil_bits = env->GetFieldID(java_cls, "stencilBits", "I");

        env->SetIntField(dst_config, red_bits, src_config.red_bits);
        env->SetIntField(dst_config, green_bits, src_config.green_bits);
        env->SetIntField(dst_config, blue_bits, src_config.blue_bits);
        env->SetIntField(dst_config, alpha_bits, src_config.alpha_bits);
        env->SetIntField(dst_config, depth_bits, src_config.depth_bits);
        env->SetIntField(dst_config, stencil_bits, src_config.stencil_bits);
      });
}

GLContextConfig ancer::BridgeGLContextConfiguration(jobject src_config)
{
  GLContextConfig dst_config;
  JniCallInAttachedThread(
      [src_config, &dst_config](JNIEnv* env) {
        jobject activity = env->NewLocalRef(_activity_weak_global_ref);

        jclass java_cls = RetrieveClass(
            env, activity,
            "com/google/gamesdk/gamecert/operationrunner/hosts/BaseGLHostActivity$GLContextConfiguration");

        jfieldID red_bits = env->GetFieldID(java_cls, "redBits", "I");
        jfieldID green_bits = env->GetFieldID(java_cls, "greenBits", "I");
        jfieldID blue_bits = env->GetFieldID(java_cls, "blueBits", "I");
        jfieldID alpha_bits = env->GetFieldID(java_cls, "alphaBits", "I");
        jfieldID depth_bits = env->GetFieldID(java_cls, "depthBits", "I");
        jfieldID stencil_bits = env->GetFieldID(java_cls, "stencilBits", "I");

        dst_config.red_bits = env->GetIntField(src_config, red_bits);
        dst_config.green_bits = env->GetIntField(src_config, green_bits);
        dst_config.blue_bits = env->GetIntField(src_config, blue_bits);
        dst_config.alpha_bits = env->GetIntField(src_config, alpha_bits);
        dst_config.depth_bits = env->GetIntField(src_config, depth_bits);
        dst_config.stencil_bits = env->GetIntField(src_config, stencil_bits);
      });

  return dst_config;
}

