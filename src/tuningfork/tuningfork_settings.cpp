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

#include "tuningfork_internal.h"
#include "tuningfork_utils.h"

#define LOG_TAG "TuningFork"
#include "Log.h"

#include "file_cache.h"

#include <android/asset_manager_jni.h>

#include "tuningfork/protobuf_nano_util.h"
#include "pb_decode.h"
#include "nano/tuningfork.pb.h"
using PBSettings = com_google_tuningfork_Settings;

namespace tuningfork {

static FileCache sFileCache;

constexpr char kPerformanceParametersBaseUri[] =
        "https://performanceparameters.googleapis.com/v1/";

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

void CheckSettings(Settings &settings, const std::string& save_dir) {
    CheckPersister(settings.c_settings.persistent_cache, save_dir);
    if (settings.base_uri.empty())
        settings.base_uri = kPerformanceParametersBaseUri;
    if (settings.base_uri.back()!='/')
        settings.base_uri += '/';
    if (settings.initial_request_timeout_ms==0)
        settings.initial_request_timeout_ms = 1000;
    if (settings.ultimate_request_timeout_ms==0)
        settings.ultimate_request_timeout_ms = 100000;
}

static bool decodeAnnotationEnumSizes(pb_istream_t* stream, const pb_field_t *field, void** arg) {
    Settings* settings = static_cast<Settings*>(*arg);
    uint64_t a;
    if (pb_decode_varint(stream, &a)) {
        settings->aggregation_strategy.annotation_enum_size.push_back((uint32_t)a);
        return true;
    } else {
        return false;
    }
}
static bool decodeHistograms(pb_istream_t* stream, const pb_field_t *field, void** arg) {
    Settings* settings = static_cast<Settings*>(*arg);
    com_google_tuningfork_Settings_Histogram hist;
    if (pb_decode(stream, com_google_tuningfork_Settings_Histogram_fields, &hist)) {
        settings->histograms.push_back(
            {hist.instrument_key, hist.bucket_min, hist.bucket_max, hist.n_buckets });
        return true;
    } else {
        return false;
    }
}
static bool decode_string(pb_istream_t* stream, const pb_field_t *field, void** arg) {
    std::string* out_str = static_cast<std::string*>(*arg);
    out_str->resize(stream->bytes_left);
    char* d = const_cast<char*>(out_str->data()); // C++17 would remove the need for cast
    while(stream->bytes_left) {
        uint64_t x;
        if (!pb_decode_varint(stream, &x))
            return false;
        *d++ = (char)x;
    }
    return true;
}

static TFErrorCode DeserializeSettings(const ProtobufSerialization& settings_ser,
                                           Settings* settings) {
    PBSettings pbsettings = com_google_tuningfork_Settings_init_zero;
    pbsettings.aggregation_strategy.annotation_enum_size.funcs.decode = decodeAnnotationEnumSizes;
    pbsettings.aggregation_strategy.annotation_enum_size.arg = settings;
    pbsettings.histograms.funcs.decode = decodeHistograms;
    pbsettings.histograms.arg = settings;
    pbsettings.base_uri.funcs.decode = decode_string;
    pbsettings.base_uri.arg = (void*)&settings->base_uri;
    pbsettings.api_key.funcs.decode = decode_string;
    pbsettings.api_key.arg = (void*)&settings->api_key;
    pbsettings.default_fp_filename.funcs.decode = decode_string;
    pbsettings.default_fp_filename.arg = (void*)&settings->default_fp_filename;
    ByteStream str {const_cast<uint8_t*>(settings_ser.data()), settings_ser.size(), 0};
    pb_istream_t stream = {ByteStream::Read, &str, settings_ser.size()};
    if (!pb_decode(&stream, com_google_tuningfork_Settings_fields, &pbsettings))
        return TFERROR_BAD_SETTINGS;
    if(pbsettings.aggregation_strategy.method
          ==com_google_tuningfork_Settings_AggregationStrategy_Submission_TICK_BASED)
        settings->aggregation_strategy.method = Settings::AggregationStrategy::Submission::TICK_BASED;
    else
        settings->aggregation_strategy.method = Settings::AggregationStrategy::Submission::TIME_BASED;
    settings->aggregation_strategy.intervalms_or_count
      = pbsettings.aggregation_strategy.intervalms_or_count;
    settings->aggregation_strategy.max_instrumentation_keys
      = pbsettings.aggregation_strategy.max_instrumentation_keys;
    settings->initial_request_timeout_ms = pbsettings.initial_request_timeout_ms;
    settings->ultimate_request_timeout_ms = pbsettings.ultimate_request_timeout_ms;
    return TFERROR_OK;
}

// Gets the serialized settings from the APK.
// Returns false if there was an error.
static bool GetSettingsSerialization(const JniCtx& jni, ProtobufSerialization& settings_ser) {
    auto asset = apk_utils::GetAsset(jni, "tuningfork/tuningfork_settings.bin");
    if (asset == nullptr )
        return false;
    ALOGI("Got settings from tuningfork/tuningfork_settings.bin");
    // Get serialized settings from assets
    uint64_t size = AAsset_getLength64(asset);
    settings_ser.resize(size);
    memcpy(const_cast<uint8_t*>(settings_ser.data()), AAsset_getBuffer(asset), size);
    AAsset_close(asset);
    return true;
}

TFErrorCode FindSettingsInApk(Settings* settings, const JniCtx& jni) {
    if (settings) {
        ProtobufSerialization settings_ser;
        if (GetSettingsSerialization(jni, settings_ser)) {
            return DeserializeSettings(settings_ser, settings);
        }
        else {
            return TFERROR_NO_SETTINGS;
        }
    } else {
        return TFERROR_BAD_PARAMETER;
    }
}

} // namespace tuningfork
