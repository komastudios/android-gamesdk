/*
 * Copyright 2020 The Android Open Source Project
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

#include "request_info.h"

#include <android/api-level.h>
#include <sys/system_properties.h>

#include <cmath>
#include <fstream>
#include <memory>
#include <sstream>

#include "jni/jni_wrap.h"
#include "tuningfork_utils.h"

namespace {

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
    while (*q && (*q == ' ' || *q == '\t')) ++q;
    return q;
}

#if __ANDROID_API__ >= 26
std::string getSystemPropViaCallback(const char* key) {
    const prop_info* prop = __system_property_find(key);
    if (!prop) {
        return "";
    }
    std::string return_value;
    auto thunk = [](void* cookie, const char* /*name*/, const char* value,
                    uint32_t /*serial*/) {
        if (value != nullptr) {
            std::string* r = static_cast<std::string*>(cookie);
            *r = value;
        }
    };
    __system_property_read_callback(prop, thunk, &return_value);
    return return_value;
}
#else
std::string getSystemPropViaGet(const char* key) {
    char buffer[PROP_VALUE_MAX + 1] = "";  // +1 for terminator
    int bufferLen = __system_property_get(key, buffer);
    if (bufferLen > 0)
        return buffer;
    else
        return "";
}
#endif

std::string getSystemProp(const char* key) {
#if __ANDROID_API__ >= 26
    return getSystemPropViaCallback(key);
#else
    return getSystemPropViaGet(key);
#endif
}

}  // anonymous namespace

namespace tuningfork {

/* static */
RequestInfo RequestInfo::ForThisGameAndDevice(const Settings& settings) {
    RequestInfo info;
    // Total memory
    std::string s = slurpFile("/proc/meminfo");
    if (!s.empty()) {
        // Lines like 'MemTotal:        3749460 kB'
        std::string to_find("MemTotal:");
        auto it = s.find(to_find);
        if (it != std::string::npos) {
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
            if (j != std::string::npos) {
                mult = ::pow(1024L, j / 2);
            }
            info.total_memory_bytes = x * mult;
        }
    }
    info.build_version_sdk = getSystemProp("ro.build.version.sdk");
    info.build_fingerprint = getSystemProp("ro.build.fingerprint");

    if (jni::IsValid()) info.session_id = UniqueId();

    info.cpu_max_freq_hz.clear();
    for (int index = 1;; ++index) {
        std::stringstream str;
        str << "/sys/devices/system/cpu/cpu" << index
            << "/cpufreq/cpuinfo_max_freq";
        auto cpu_freq_file = slurpFile(str.str().c_str());
        if (cpu_freq_file.empty()) break;
        uint64_t freq;
        std::istringstream cstr(cpu_freq_file);
        cstr >> freq;
        info.cpu_max_freq_hz.push_back(freq * 1000);  // File is in kHz
    }

    if (jni::IsValid()) {
        info.apk_version_code = apk_utils::GetVersionCode(
            &info.apk_package_name, &info.gl_es_version);
        info.model = jni::android::os::Build::MODEL().C();
        info.brand = jni::android::os::Build::BRAND().C();
        info.product = jni::android::os::Build::PRODUCT().C();
        info.device = jni::android::os::Build::DEVICE().C();
    }
    info.tuningfork_version = TUNINGFORK_PACKED_VERSION;
    info.swappy_version = settings.c_settings.swappy_version;
    return info;
}

static RequestInfo s_request_info;

/*static*/ RequestInfo& RequestInfo::CachedValue() { return s_request_info; }

}  // namespace tuningfork
