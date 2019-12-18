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

#include "BaseOperation.hpp"

#include <dlfcn.h>
#include <sched.h>
#include <thread>
#include <sstream>

#include "util/Error.hpp"
#include "util/Json.hpp"

using namespace ancer;


//==============================================================================

namespace {
    constexpr Log::Tag TAG{"operations_load"};

    void* OpenSelfLibrary(void) {
        // TODO: Have CMake generate this.
        constexpr const char* this_lib_name = "libnative-lib.so";
        void* lib = dlopen(this_lib_name, RTLD_LAZY);
        if ( lib == nullptr ) {
            FatalError(TAG, "Failed to load self library");
        }
        return lib;
    }
}

//==============================================================================

std::shared_ptr<BaseOperation> BaseOperation::Load(
        const std::string& operation_id,
        const std::string& suite_id,
        const std::string& suite_description,
        Mode mode) {
    void* lib = OpenSelfLibrary();

    std::string fn_name = operation_id;
    switch ( mode ) {
    case Mode::DataGatherer:fn_name += "_CreateDataGatherer";
        break;
    case Mode::Stressor:fn_name += "_CreateStressor";
        break;
    }

    using FactoryFunction = void (*)(std::shared_ptr<BaseOperation>&);
    auto fn = (FactoryFunction)(dlsym(lib, fn_name.c_str()));
    if ( fn ) {
        std::shared_ptr<BaseOperation> op;
        fn(op);
        op->_operation_id = operation_id;
        op->_suite_id = suite_id;
        op->_suite_desc = suite_description;
        op->_mode = mode;
        return op;
    } else {
        FatalError(
                TAG, "Unable to dlsym() operation named \"" + fn_name +
                        "\" - did you remember to EXPORT_ANCER_OPERATION() it?");
    }

    return nullptr;
}

//==============================================================================

BaseOperation::~BaseOperation() = default;

//==============================================================================

namespace {
    struct TestStartEvent {
        const std::string& name;
        const std::string& description;
    };

    JSON_WRITER(TestStartEvent) {
        JSON_SETVAR(event, "Test Start");
        JSON_REQVAR(name);
        JSON_REQVAR(description);
    }
}

void BaseOperation::Start() {
    _start_time = SteadyClock::now();
    Report(TestStartEvent{this->_suite_id, this->_suite_desc});
}

//==============================================================================

void BaseOperation::Draw(double delta_seconds) {
    if ( !IsStopped()) {
        const auto now = SteadyClock::now();

        // determine if it's time to invoke the heartbeat callback
        if ( _heartbeat_timestamp == Timestamp{} ) _heartbeat_timestamp = now;
        if ( _heartbeat_period > Duration::zero()) {
            auto elapsed = now - _heartbeat_timestamp;
            if ( elapsed > _heartbeat_period ) {
                OnHeartbeat(elapsed);
                // Reset timestamp to after the above finishes
                _heartbeat_timestamp = SteadyClock::now();
            }
        }

        // if running in test mode, check if we've run out our clock
        if ( GetMode() == Mode::DataGatherer ) {
            auto duration = GetDuration();
            if ((duration > Duration::zero()) &&
                (now - _start_time > duration)) {
                Stop();
            }
        }
    }
}

//==============================================================================

void BaseOperation::OnGlContextResized(int width, int height) {
    _width = width;
    _height = height;
}

//==============================================================================

namespace {
    std::atomic<int> _datum_issue_id = 0;

    int GetNextDatumIssueId() {
        return _datum_issue_id++;
    }
}

void BaseOperation::ReportImpl(const Json& custom_payload) const {
    if ( GetMode() == Mode::DataGatherer ) {
        auto datum_issue_id = GetNextDatumIssueId();
        auto now = SteadyClock::now();

        {
            // write a clock-sync trace
            // TODO(shamyl@google.com) If we can determine *when* sytrace begins
            // writing its buffers, we can write just one clock sync marker; but until
            // then, we'll emit a clock sync with each datum save (fortunately, traces
            // are cheap)

            auto clock_sync_name = std::string("ancer::clock_sync(") + std::to_string(datum_issue_id) + ")";
            auto timestamp_ns =
                    static_cast<int64_t>(now.time_since_epoch().count());
            gamesdk::Trace::getInstance()->setCounter(clock_sync_name.c_str(), timestamp_ns);
        }

        std::stringstream ss;
        ss << std::this_thread::get_id();
        auto t_id = ss.str();

        reporting::Datum datum;
        datum.issue_id = datum_issue_id;
        datum.suite_id = _suite_id;
        datum.operation_id = _operation_id;
        datum.cpu_id = sched_getcpu();
        datum.thread_id = t_id;
        datum.timestamp = SteadyClock::now();
        datum.custom = custom_payload;

        reporting::WriteToReportLog(datum);
    }
}
