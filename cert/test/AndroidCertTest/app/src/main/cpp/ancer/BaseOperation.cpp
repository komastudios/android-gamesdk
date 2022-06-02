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

#include <ancer/util/Error.hpp>
#include <ancer/util/Json.hpp>

using namespace ancer;


//==============================================================================

namespace {
    constexpr Log::Tag TAG{"operations_load"};

    void *OpenSelfLibrary(void) {
        // TODO: Have CMake generate this.
        constexpr const char *this_lib_name = "libcert-lib.so";
        void *lib = dlopen(this_lib_name, RTLD_LAZY);
        if (lib == nullptr) {
            FatalError(TAG, "Failed to load self library");
        }
        return lib;
    }
}

std::unique_ptr<BaseOperation> BaseOperation::Load(
        const std::string &operation_id, const std::string &suite_id,
        const std::string& suite_description, Mode mode) {
    void *lib = OpenSelfLibrary();

    std::string fn_name = operation_id;
    switch (mode) {
        case Mode::DataGatherer:
            fn_name += "_CreateDataGatherer";
            break;
        case Mode::Stressor:
            fn_name += "_CreateStressor";
            break;
    }

    using FactoryFunction = void (*)(std::unique_ptr<BaseOperation> &);
    auto fn = (FactoryFunction) (dlsym(lib, fn_name.c_str()));
    if (fn) {
        std::unique_ptr<BaseOperation> op;
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

void BaseOperation::Start() {
    _start_time = SteadyClock::now();
}

void BaseOperation::Draw(double delta_seconds) {
    if (!IsStopped()) {
        const auto now = SteadyClock::now();

        // determine if it's time to invoke the heartbeat callback
        if (_heartbeat_timestamp == Timestamp{}) _heartbeat_timestamp = now;
        if (_heartbeat_period > Duration::zero()) {
            auto elapsed = now - _heartbeat_timestamp;
            if (elapsed > _heartbeat_period) {
                OnHeartbeat(elapsed);
                // Reset timestamp to after the above finishes
                _heartbeat_timestamp = SteadyClock::now();
            }
        }

        // if running in test mode, check if we've run out our clock
        if (GetMode() == Mode::DataGatherer) {
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
