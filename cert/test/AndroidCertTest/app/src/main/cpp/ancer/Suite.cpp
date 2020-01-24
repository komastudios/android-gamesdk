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
#include "Renderer.hpp"
#include "Reporting.hpp"
#include "System.hpp"
#include "util/Error.hpp"

using namespace ancer;
using namespace ancer::internal;


//==============================================================================

namespace {
    Log::Tag TAG{"ancer::suite"};

    int _id_counter = 0;
    std::set<int> _stopped_operations;
}

//==============================================================================

void internal::InitializeSuite() {
    assert(_operations.empty() && _stopped_operations.empty());
    _id_counter = 0;
}

void internal::ShutdownSuite() {
    if ( auto* renderer = GetRenderer() ) {
        renderer->ClearOperations();
    }

    // TODO(tmillican@google.com): Operation lifetime management should really
    //  be under Suite so we can call Stop/Wait here.

    _stopped_operations.clear();
    _operations.clear();
}

//==============================================================================

void internal::SuiteUpdateRenderer() {
    if ( auto* renderer = GetRenderer() ) {
        // TODO(tmillican@google.com): Tie to StartOperation() to match the
        //  removal when StopOperation() is called.
        for ( auto& op : _operations ) {
            renderer->AddOperation(*op.second);
        }
    }
}

//==============================================================================

int internal::CreateOperation(
        const std::string& suite,
        const std::string& operation,
        ancer::BaseOperation::Mode mode) {
    auto op = BaseOperation::Load(operation, suite, mode);
    if ( op ) {
        std::lock_guard<std::mutex> lock(_operations_lock);
        int id = _id_counter++;
        _operations[id] = std::move(op);
        return id;
    } else {
        FatalError(TAG, "CreateOperation - Unable to load operation named \"%s\"",
                   operation.c_str());
    }
}

//==============================================================================

void internal::StartOperation(int id, Duration duration,
                              const std::string& config) {
    if (auto pos = _operations.find(id); pos != _operations.end()) {
        auto& op = *pos->second;

        Log::I(TAG, "Starting operation id: %d mode %s", id,
               op.GetMode() == BaseOperation::Mode::DataGatherer
                   ? "DATA_GATHERER" : "STRESSOR");

        op.Init(duration, config);
        op.Start();
    } else {
        FatalError(TAG, "StartOperation - No operation with id %d", id);
    }
}

//==============================================================================

void internal::StopOperation(int id) {
    if ( auto pos = _operations.find(id); pos != _operations.end()) {
        if ( auto* renderer = GetRenderer() ) {
            renderer->RemoveOperation(*pos->second);
        }

        {
            std::lock_guard<std::mutex> lock(_operations_lock);
            _stopped_operations.insert(id);
            pos->second->Stop();
        }
    } else {
        FatalError(TAG,"StopOperation - No operation with id %d", id);
    }
}

//==============================================================================

bool internal::OperationIsStopped(int id) {
    std::lock_guard<std::mutex> lock(_operations_lock);
    auto pos = _operations.find(id);
    if ( pos != _operations.end()) {
        return pos->second->IsStopped();
    } else if ( _stopped_operations.count(id)) {
        return true;
    } else {
        FatalError(TAG, "OperationIsStopped - No active or stopped operation with id %d",
                   id);
    }
}

//==============================================================================

void internal::WaitForOperation(int id) {
    auto pos = _operations.find(id);
    if ( pos != _operations.end()) {
        pos->second->Wait();
        reporting::FlushReportLogQueue();

        if ( auto* renderer = GetRenderer() ) {
            renderer->RemoveOperation(*pos->second);
        }

        {
            std::lock_guard<std::mutex> lock(_operations_lock);
            _stopped_operations.insert(id);
            _operations.erase(pos);
        }
    } else {
        FatalError(TAG, "WaitForOperation - No operation with id %d", id);
    }
}
