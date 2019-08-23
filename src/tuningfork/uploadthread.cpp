/*
 * Copyright 2018 The Android Open Source Project
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

#include "uploadthread.h"
#include "tuningfork_utils.h"
#include "tuningfork/protobuf_util.h"
#include "ge_serializer.h"

#include <sys/system_properties.h>
#include <GLES3/gl32.h>
#include <cstring>
#include <fstream>
#include <sstream>
#include <cmath>
#include "ge_serializer.h"
#include "modp_b64.h"

#define LOG_TAG "TuningFork"
#include "Log.h"

namespace tuningfork {

const uint64_t HISTOGRAMS_PAUSED = 0;
const uint64_t HISTOGRAMS_UPLOADING = 1;

DebugBackend::~DebugBackend() {}

TFErrorCode DebugBackend::Process(const std::string &s) {
    if (s.size() == 0) return TFERROR_BAD_PARAMETER;
    // Split the serialization into <128-byte chunks to avoid logcat line
    //  truncation.
    constexpr size_t maxStrLen = 128;
    int n = (s.size() + maxStrLen - 1) / maxStrLen; // Round up
    for (int i = 0, j = 0; i < n; ++i) {
        std::stringstream str;
        str << "(TJS" << (i + 1) << "/" << n << ")";
        int m = std::min(s.size() - j, maxStrLen);
        str << s.substr(j, m);
        j += m;
        ALOGI("%s", str.str().c_str());
    }
    return TFERROR_OK;
}

std::unique_ptr<DebugBackend> s_debug_backend = std::make_unique<DebugBackend>();

void Runnable::Start() {
    if (thread_) {
        ALOGW("Can't start an already running thread");
        return;
    }
    do_quit_ = false;
    thread_ = std::make_unique<std::thread>([&] { Run(); });
}
void Runnable::Run() {
    while (!do_quit_) {
        std::unique_lock<std::mutex> lock(mutex_);
        DoWork();
        cv_.wait_for(lock, std::chrono::milliseconds(wait_time_ms_));
    }
}
void Runnable::Stop() {
    if (!thread_->joinable()) {
        ALOGW("Can't stop a thread that's not started");
        return;
    }
    do_quit_ = true;
    cv_.notify_one();
    thread_->join();
}

class UltimateUploader : public Runnable {
    const TFCache* persister_;
    std::unique_ptr<std::thread> thread_;
    std::mutex mutex_;
    std::condition_variable cv_;
    bool do_quit_;
public:
    UltimateUploader(const TFCache* persister) : Runnable(1000),
                                                 persister_(persister) {}
    void DoWork() override {
        CheckUploadPending();
    }
    void CheckUploadPending() {
        CProtobufSerialization uploading_hists_ser;
        if (persister_->get(HISTOGRAMS_UPLOADING, &uploading_hists_ser,
                persister_->user_data)==TFERROR_OK) {
            // TODO(willosborn): Upload to play endpoint
            ALOGI("Upload pending");
            CProtobufSerialization_Free(&uploading_hists_ser);
        }
        else {
            ALOGI("No upload pending");
        }
    }
};

UploadThread::UploadThread(Backend *backend, const ExtraUploadInfo& extraInfo) : Runnable(1000),
                                               backend_(backend),
                                               current_fidelity_params_(0),
                                               upload_callback_(nullptr),
                                               extra_info_(extraInfo),
                                               persister_(nullptr) {
    if (backend_ == nullptr)
        backend_ = s_debug_backend.get();
    Start();
}

UploadThread::~UploadThread() {
    Stop();
}

void UploadThread::Start() {
    ready_ = nullptr;
    Runnable::Start();
}

void UploadThread::Stop() {
    if (ultimate_uploader_)
        ultimate_uploader_->Stop();
    Runnable::Stop();
}

void ToCProtobufSerialization(const std::string& s,
                              CProtobufSerialization& cpbs) {
    cpbs.bytes = (uint8_t*)::malloc(s.size());
    memcpy(cpbs.bytes, s.data(), s.size());
    cpbs.size = s.size();
    cpbs.dealloc = CProtobufSerialization_Dealloc;
}
void FromCProtobufSerialization(const CProtobufSerialization& cpbs,
                                std::string& s) {
    s = std::string((const char*)cpbs.bytes, cpbs.size);
}

void UploadThread::DoWork() {
    if (ready_) {
        UpdateGLVersion(); // Needs to be done with an active gl context
        std::string evt_ser_json;
        GESerializer::SerializeEvent(*ready_, current_fidelity_params_,
                                     extra_info_,
                                     evt_ser_json);
        if(upload_callback_) {
            upload_callback_(evt_ser_json.c_str(), evt_ser_json.size());
        }
        if (upload_)
            backend_->Process(evt_ser_json);
        else {
            CProtobufSerialization cser;
            ToCProtobufSerialization(evt_ser_json, cser);
            if (persister_)
                persister_->set(HISTOGRAMS_PAUSED, &cser, persister_->user_data);
            CProtobufSerialization_Free(&cser);
        }
        ready_ = nullptr;
    }
}

// Returns true if we submitted, false if we are waiting for a previous submit to complete
bool UploadThread::Submit(const ProngCache *prongs, bool upload) {
    if (ready_ == nullptr) {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            upload_ = upload;
            ready_ = prongs;
        }
        cv_.notify_one();
        return true;
    } else
        return false;
}

namespace {

// TODO(willosborn): replace these with device_info library calls once they are available

std::string slurpFile(const char* fname) {
    std::ifstream f(fname);
    if (f.good()) {
        std::stringstream str;
        str << f.rdbuf();
        return str.str();
    }
    return "";
}

const char* skipSpace(const char* q) {
    while(*q && (*q==' ' || *q=='\t')) ++q;
    return q;
}
std::string getSystemPropViaGet(const char* key) {
    char buffer[PROP_VALUE_MAX + 1]="";  // +1 for terminator
    int bufferLen = __system_property_get(key, buffer);
    if(bufferLen>0)
        return buffer;
    else
        return "";
}

}

/* static */
ExtraUploadInfo UploadThread::GetExtraUploadInfo(JNIEnv* env, jobject context) {
    ExtraUploadInfo extra_info;
    // Total memory
    std::string s = slurpFile("/proc/meminfo");
    if (!s.empty()) {
        // Lines like 'MemTotal:        3749460 kB'
        std::string to_find("MemTotal:");
        auto it = s.find(to_find);
        if(it!=std::string::npos) {
            const char* p = s.data() + it + to_find.length();
            p = skipSpace(p);
            std::istringstream str(p);
            uint64_t x;
            str >> x;
            std::string units;
            str >> units;
            static std::string unitPrefix = "bBkKmMgGtTpP";
            auto j = unitPrefix.find(units[0]);
            uint64_t mult = 1;
            if (j!=std::string::npos) {
                mult = ::pow(1024L,j/2);
            }
            extra_info.total_memory_bytes = x*mult;
        }
    }
    extra_info.build_version_sdk = getSystemPropViaGet("ro.build.version.sdk");
    extra_info.build_fingerprint = getSystemPropViaGet("ro.build.fingerprint");

    extra_info.session_id = UniqueId(env);

    extra_info.cpu_max_freq_hz.clear();
    for(int index = 1;;++index) {
        std::stringstream str;
        str << "/sys/devices/system/cpu/cpu" << index << "/cpufreq/cpuinfo_max_freq";
        auto cpu_freq_file = slurpFile(str.str().c_str());
        if (cpu_freq_file.empty())
            break;
        uint64_t freq;
        std::istringstream cstr(cpu_freq_file);
        cstr >> freq;
        extra_info.cpu_max_freq_hz.push_back(freq*1000); // File is in kHz
    }

    extra_info.apk_version_code = apk_utils::GetVersionCode(env, context,
        &extra_info.apk_package_name);

    extra_info.tuningfork_version = TUNINGFORK_PACKED_VERSION;

    return extra_info;
}

void UploadThread::UpdateGLVersion() {
    // gl_es_version
    GLint glVerMajor = 2;
    GLint glVerMinor = 0;
    glGetIntegerv(GL_MAJOR_VERSION, &glVerMajor);
    if (glGetError() != GL_NO_ERROR) {
        glVerMajor = 0;
        glVerMinor = 0;
    } else {
        glGetIntegerv(GL_MINOR_VERSION, &glVerMinor);
    }
    extra_info_.gl_es_version = (glVerMajor<<16) + glVerMinor;
}

void UploadThread::InitialChecks(ProngCache& prongs,
                                 IdProvider& id_provider,
                                 const TFCache* persister) {
    persister_ = persister;
    if (!persister_) {
        ALOGE("No persistence mechanism given");
        return;
    }
    // Check for PAUSED prong cache
    CProtobufSerialization paused_hists_ser;
    if (persister->get(HISTOGRAMS_PAUSED, &paused_hists_ser,
                       persister_->user_data)==TFERROR_OK) {
        std::string paused_hists_str;
        FromCProtobufSerialization(paused_hists_ser, paused_hists_str);
        ALOGI("Got PAUSED histrograms: %s", paused_hists_str.c_str());
        GESerializer::DeserializeAndMerge(paused_hists_str,
                                          id_provider,
                                          prongs);
        CProtobufSerialization_Free(&paused_hists_ser);
    }
    else {
        ALOGI("No PAUSED histograms");
    }
    std::unique_lock<std::mutex> lock(mutex_);
    if( ultimate_uploader_.get() == nullptr) {
        ultimate_uploader_ = std::make_unique<UltimateUploader>(persister);
        ultimate_uploader_->Start();
    }
}


} // namespace tuningfork
