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

#include "Suite.hpp"

#include <map>
#include <memory>
#include <mutex>
#include <set>

#include "BaseOperation.hpp"
#include "Reporting.hpp"
#include "SwappyRenderer.hpp"
#include "util/Error.hpp"

using namespace ancer;


//==================================================================================================

namespace ancer::internal {
    // For ForEachOperation
    std::mutex _operations_lock;
    std::map<int, std::shared_ptr<BaseOperation>> _operations;
}

namespace {
    Log::Tag TAG{"ancer::suite"};

    int _id_counter = 0;
    
    std::set<int> _stopped_operations;

    // TODO(tmillican@google.com): Refactor this to better separate concerns
    swappy::Renderer* _swappy_renderer = nullptr;
}

//==================================================================================================

void internal::InitializeSuite() {
    // ...
}

void internal::ShutdownSuite() {
    // ...
}

//==================================================================================================

void internal::SetSwappyRenderer(swappy::Renderer* swappy_renderer) {
    _swappy_renderer = swappy_renderer;
    if ( _swappy_renderer ) {
        for ( auto& op : _operations ) {
            _swappy_renderer->AddOperation(op.second);
        }
    }
}

//==================================================================================================

int internal::CreateOperation(
        const std::string& suite,
        const std::string& operation,
        ancer::BaseOperation::Mode mode) {
    auto op = BaseOperation::Load(operation, suite, mode);
    if ( op ) {
        {
            int id = _id_counter++;
            std::lock_guard<std::mutex> lock(_operations_lock);
            _operations[id] = op;
            return id;
        }
    } else {
        FatalError(
                TAG, "createOperation - Unable to load operation named \""
                        + operation + "\"");
    }
}

//==================================================================================================

void internal::StartOperation(
        int id,
        Duration duration,
        const std::string& config) {
    if ( auto pos = _operations.find(id); pos != _operations.end()) {
        auto& op = *pos->second;

        Log::I(
                TAG,
                "starting operation id: " + std::to_string(id) + " mode: " +
                        (op.GetMode() == BaseOperation::Mode::DataGatherer
                         ? "DATA_GATHERER" : "STRESSOR"));

        op.Init(duration, config);
        op.Start();
    } else {
        FatalError(
                TAG,
                "startOperation - No operation with id " + std::to_string(id));
    }
}

//==================================================================================================

void internal::StopOperation(int id) {
    if ( auto pos = _operations.find(id); pos != _operations.end()) {

        {
            std::lock_guard<std::mutex> lock(_operations_lock);
            _stopped_operations.insert(id);
            pos->second->Stop();
        }

        if ( _swappy_renderer ) {
            _swappy_renderer->RemoveOperation(pos->second);
        }
    } else {
        FatalError(
                TAG,
                "stopOperation - No operation with id " + std::to_string(id));
    }
}

//==================================================================================================

bool internal::OperationIsStopped(int id) {
    std::lock_guard<std::mutex> lock(_operations_lock);
    auto pos = _operations.find(id);
    if ( pos != _operations.end()) {
        return pos->second->IsStopped();
    } else if ( _stopped_operations.count(id)) {
        return true;
    } else {
        FatalError(
                TAG,
                "isOperationStopped - No active or stopped operation with id "
                        + std::to_string(id));
    }
}

//==================================================================================================

void internal::WaitForOperation(int id) {
    auto pos = _operations.find(id);
    if ( pos != _operations.end()) {
        pos->second->Wait();
        reporting::FlushReportLogQueue();

        {
            std::lock_guard<std::mutex> lock(_operations_lock);
            _stopped_operations.insert(id);
            _operations.erase(pos);
        }

    } else {
        FatalError(
                TAG, "waitForOperation - No operation with id "
                        + std::to_string(id));
    }
}
