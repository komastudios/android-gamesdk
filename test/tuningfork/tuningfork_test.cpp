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
#include <iostream>
using namespace std;

#include "tf_test_utils.h"

#include "tuningfork/protobuf_util.h"
#include "tuningfork/tuningfork_internal.h"

#include "full/tuningfork.pb.h"
#include "full/dev_tuningfork.pb.h"

#include "gtest/gtest.h"

#include <vector>
#include <mutex>

#define LOG_TAG "TFTest"
#include "Log.h"

using namespace tuningfork;
using namespace test;

namespace tuningfork_test {

using ::com::google::tuningfork::FidelityParams;
using ::com::google::tuningfork::Annotation;
typedef std::string TuningForkLogEvent;

class TestBackend : public Backend {
public:
    TestBackend(std::shared_ptr<std::condition_variable> cv_,
                std::shared_ptr<std::mutex> mutex_) : cv(cv_), mutex(mutex_) {}

    TFErrorCode Process(const TuningForkLogEvent &evt_ser) override {
        ALOGI("Process");
        {
            std::lock_guard<std::mutex> lock(*mutex);
            result = evt_ser;
        }
        cv->notify_all();
        return TFERROR_OK;
    }

    void Clear() { result = ""; }

    TuningForkLogEvent result;
    std::shared_ptr<std::condition_variable> cv;
    std::shared_ptr<std::mutex> mutex;
};

class TestParamsLoader : public ParamsLoader {
public:
    TFErrorCode GetFidelityParams(const JniCtx& jni,
                                  const ExtraUploadInfo& info,
                                  const std::string& url_base,
                                  const std::string& api_key,
                                  ProtobufSerialization &fidelity_params,
                                  std::string& experiment_id, uint32_t timeout_ms) override {
        return TFERROR_NO_FIDELITY_PARAMS;
    }
};

// Increment time with a known tick size
class TestTimeProvider : public ITimeProvider {
public:
    TestTimeProvider(Duration tickSize_ = std::chrono::milliseconds(20),
                     SystemDuration systemTickSize_ = std::chrono::milliseconds(20))
        : tickSize(tickSize_), systemTickSize(systemTickSize_) {}

    TimePoint t;
    SystemTimePoint st;
    Duration tickSize;
    SystemDuration systemTickSize;

    TimePoint Now() override {
        t += tickSize;
        return t;
    }
    SystemTimePoint SystemNow() override {
        st += systemTickSize;
        return st;
    }
    void Reset() {
        t = TimePoint();
        st = SystemTimePoint();
    }
};

std::shared_ptr<std::condition_variable> cv = std::make_shared<std::condition_variable>();
std::shared_ptr<std::mutex> rmutex = std::make_shared<std::mutex>();
TestBackend testBackend(cv, rmutex);
TestParamsLoader paramsLoader;
TestTimeProvider timeProvider;
ExtraUploadInfo extra_upload_info = {};
static const std::string kCacheDir = "/data/local/tmp/tuningfork_test";

Settings TestSettings(Settings::AggregationStrategy::Submission method, int n_ticks, int n_keys,
                      std::vector<uint32_t> annotation_size,
                      const std::vector<TFHistogram>& hists = {}) {
    Settings s {};
    s.aggregation_strategy.method = method;
    s.aggregation_strategy.intervalms_or_count = n_ticks;
    s.aggregation_strategy.max_instrumentation_keys = n_keys;
    s.aggregation_strategy.annotation_enum_size = annotation_size;
    s.histograms = hists;
    CheckSettings(s, kCacheDir);
    return s;
}
const Duration test_wait_time = std::chrono::seconds(1);
JniCtx jni {NULL, NULL};
const TuningForkLogEvent& TestEndToEnd() {
    testBackend.Clear();
    timeProvider.Reset();
    const int NTICKS = 101; // note the first tick doesn't add anything to the histogram
    auto settings = TestSettings(Settings::AggregationStrategy::Submission::TICK_BASED, NTICKS - 1, 1, {});
    tuningfork::Init(settings, jni, &extra_upload_info, &testBackend, &paramsLoader, &timeProvider);
    std::unique_lock<std::mutex> lock(*rmutex);
    for (int i = 0; i < NTICKS; ++i)
        tuningfork::FrameTick(TFTICK_SYSCPU);
    // Wait for the upload thread to complete writing the string
    EXPECT_TRUE(cv->wait_for(lock, test_wait_time)==std::cv_status::no_timeout) << "Timeout";

    return testBackend.result;
}

const TuningForkLogEvent& TestEndToEndWithAnnotation() {
    testBackend.Clear();
    timeProvider.Reset();
    const int NTICKS = 101; // note the first tick doesn't add anything to the histogram
    // {3} is the number of values in the Level enum in tuningfork_extensions.proto
    auto settings = TestSettings(Settings::AggregationStrategy::Submission::TICK_BASED, NTICKS - 1, 2, {3});
    cout << "Initializing" << endl;
    tuningfork::Init(settings, jni, &extra_upload_info, &testBackend, &paramsLoader, &timeProvider);
    cout << "Initialized" << endl;
    Annotation ann;
    ann.set_level(com::google::tuningfork::LEVEL_1);
    tuningfork::SetCurrentAnnotation(Serialize(ann));
    std::unique_lock<std::mutex> lock(*rmutex);
    for (int i = 0; i < NTICKS; ++i)
        tuningfork::FrameTick(TFTICK_SYSGPU);
    // Wait for the upload thread to complete writing the string
    EXPECT_TRUE(cv->wait_for(lock, test_wait_time)==std::cv_status::no_timeout) << "Timeout";
    return testBackend.result;
}

const TuningForkLogEvent& TestEndToEndTimeBased() {
    testBackend.Clear();
    timeProvider.Reset();
    const int NTICKS = 101; // note the first tick doesn't add anything to the histogram
    TestTimeProvider timeProvider(std::chrono::milliseconds(100)); // Tick in 100ms intervals
    auto settings = TestSettings(Settings::AggregationStrategy::Submission::TIME_BASED, 10100, 1, {},
                                 {{TFTICK_SYSCPU, 50,150,10}});
    tuningfork::Init(settings, jni, &extra_upload_info, &testBackend, &paramsLoader, &timeProvider);
    std::unique_lock<std::mutex> lock(*rmutex);
    for (int i = 0; i < NTICKS; ++i)
        tuningfork::FrameTick(TFTICK_SYSCPU);
    // Wait for the upload thread to complete writing the string
    EXPECT_TRUE(cv->wait_for(lock, test_wait_time)==std::cv_status::no_timeout) << "Timeout";
    return testBackend.result;
}

void CheckEvent(const std::string& name, const TuningForkLogEvent& result,
                const TuningForkLogEvent& expected) {
    bool comp = CompareIgnoringWhitespace(result, expected);
    EXPECT_TRUE( comp ) << "\nResult:\n" << result << "\n!=\nExpected:" << expected;
}

static std::string ReplaceReturns(std::string in) {
    std::replace( in.begin(), in.end(), '\n', ' ');
    return in;
}

static const std::string session_context = R"TF(
{
  "device": {
  "build_version": "",
    "cpu_core_freqs_hz": [],
    "fingerprint": "",
    "gles_version": {
      "major": 2,
      "minor": 0
    },
    "total_memory_bytes": 0},
  "game_sdk_info": {
    "session_id": "",
    "version": "0.4"
  },
  "time_period": {
    "end_time": "1970-01-01T00:00:02.020000Z",
    "start_time": "1970-01-01T00:00:00.020000Z"
  }
}
)TF";

TEST(TuningForkTest, EndToEnd) {
    auto& result = TestEndToEnd();
    TuningForkLogEvent expected = R"TF(
{
  "name": "applications//apks/0",
  "session_context":)TF" + session_context + R"TF(,
  "telemetry": [{
    "context": {
      "annotations": "",
      "duration": "2s",
      "tuning_parameters": {
        "experiment_id": "",
        "serialized_fidelity_parameters": ""
      }
    },
    "report": {
      "rendering": {
        "render_time_histogram": [{
         "counts": [
           0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 100, 0, 0, 0, 0,
           0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
         "instrument_id": 64000
        }]
      }
    }
  }]
}
)TF";
    CheckEvent("Base", result, expected);
}

TEST(TuningForkTest, TestEndToEndWithAnnotation) {
    auto& result = TestEndToEndWithAnnotation();
    TuningForkLogEvent expected = R"TF(
{
  "name": "applications//apks/0",
  "session_context":)TF" + session_context + R"TF(,
  "telemetry": [{
    "context": {
      "annotations": "CAE=",
      "duration": "2s",
      "tuning_parameters": {
        "experiment_id": "",
        "serialized_fidelity_parameters": ""
      }
    },
    "report": {
      "rendering": {
        "render_time_histogram": [{
         "counts": [
           0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 100, 0, 0, 0, 0,
           0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
         "instrument_id": 64001
        }]
      }
    }
  }]
}
)TF";
    CheckEvent("Annotation", result, expected);
}

TEST(TuningForkTest, TestEndToEndTimeBased) {
    auto& result = TestEndToEndTimeBased();
    TuningForkLogEvent expected = R"TF(
{
  "name": "applications//apks/0",
  "session_context":)TF" + session_context + R"TF(,
  "telemetry": [{
    "context": {
      "annotations": "",
      "duration": "10s",
      "tuning_parameters": {
        "experiment_id": "",
        "serialized_fidelity_parameters": ""
      }
    },
    "report": {
      "rendering": {
        "render_time_histogram": [{
         "counts": [
           0, 0, 0, 0, 0, 0, 100, 0, 0, 0, 0, 0],
         "instrument_id": 64000
        }]
      }
    }
  }]
}
)TF";
    CheckEvent("TimeBased", result, expected);
}

} // namespace tuningfork_test
