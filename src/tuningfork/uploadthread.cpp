/*
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

#include <sstream>
#include "uploadthread.h"
#include "clearcutserializer.h"

namespace tuningfork {

std::string base64_encode(const uint8_t *a, size_t n) {
    static const std::string base64_chars =
      "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
      "abcdefghijklmnopqrstuvwxyz"
      "0123456789+/";
    std::string ret;
    int i = 0;
    int j = 0;
    uint8_t c3[3];
    uint8_t c4[4];

    if (n > 0) {
        const uint8_t *p = a;
        while (n--) {
            c3[i++] = *(p++);
            if (i == 3) {
                c4[0] = (c3[0] & 0xfc) >> 2;
                c4[1] = ((c3[0] & 0x03) << 4) + ((c3[1] & 0xf0) >> 4);
                c4[2] = ((c3[1] & 0x0f) << 2) + ((c3[2] & 0xc0) >> 6);
                c4[3] = c3[2] & 0x3f;

                for (i = 0; (i < 4); i++)
                    ret += base64_chars[c4[i]];
                i = 0;
            }
        }

        if (i) {
            for (j = i; j < 3; j++)
                c3[j] = '\0';

            c4[0] = (c3[0] & 0xfc) >> 2;
            c4[1] = ((c3[0] & 0x03) << 4) + ((c3[1] & 0xf0) >> 4);
            c4[2] = ((c3[1] & 0x0f) << 2) + ((c3[2] & 0xc0) >> 6);

            for (j = 0; (j < i + 1); j++)
                ret += base64_chars[c4[j]];

            while ((i++ < 3))
                ret += '=';

        }
    }
    return ret;
}

DebugBackend::~DebugBackend() {}

bool DebugBackend::Process(const ProtobufSerialization &evt_ser) {
    std::string s = base64_encode(&evt_ser[0], evt_ser.size());
    // Split the serialization into <128-byte chunks to avoid logcat line
    //  truncation.
    constexpr size_t maxStrLen = 128;
    int n = (s.size() + maxStrLen - 1)/maxStrLen; // Round up
    for(int i=0, j=0; i<n ;++i) {
        std::stringstream str;
        str << "(TCL" << (i+1) << "/" << n << ")";
        int m = std::min(s.size() - j, maxStrLen);
        str << s.substr(j,m);
        j += m;
        __android_log_print(ANDROID_LOG_INFO, "TuningFork", "%s",
                            str.str().c_str());
    }
    return true;
}

bool DebugBackend::GetFidelityParams(ProtobufSerialization &fp_ser,
                                     size_t timeout_ms) {
    FidelityParams fpDefault;
    // TODO: put some dummy params here for testing
    fp_ser.clear();
    return true;
};

std::unique_ptr<DebugBackend> s_debug_backend = std::make_unique<DebugBackend>();

UploadThread::UploadThread(Backend *backend) : backend_(backend),
                                               current_fidelity_params_(0) {
    if (backend_ == nullptr)
        backend_ = s_debug_backend.get();
    Start();
}

UploadThread::~UploadThread() {
    Stop();
}

void UploadThread::Start() {
    if (thread_) {
        __android_log_print(ANDROID_LOG_WARN, "TuningFork",
                            "Can't start an already running thread");
        return;
    }
    do_quit_ = false;
    ready_ = nullptr;
    thread_ = std::make_unique<std::thread>([&] { return Run(); });
}

void UploadThread::Stop() {
    if (!thread_->joinable()) {
        __android_log_print(ANDROID_LOG_WARN, "TuningFork",
                            "Can't stop a thread that's not started");
        return;
    }
    do_quit_ = true;
    cv_.notify_one();
    thread_->join();
}

void UploadThread::Run() {
    while (!do_quit_) {
        std::unique_lock<std::mutex> lock(mutex_);
        if (ready_) {
            ProtobufSerialization evt_ser;
            ClearcutSerializer::SerializeEvent(*ready_, current_fidelity_params_, evt_ser);
            backend_->Process(evt_ser);
            ready_ = nullptr;
        }
        cv_.wait_for(lock, std::chrono::milliseconds(1000));
    }
}

// Returns true if we submitted, false if we are waiting for a previous submit to complete
bool UploadThread::Submit(const ProngCache *prongs) {
    if (ready_ == nullptr) {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            ready_ = prongs;
        }
        cv_.notify_one();
        return true;
    } else
        return false;
}

} // namespace tuningfork {
