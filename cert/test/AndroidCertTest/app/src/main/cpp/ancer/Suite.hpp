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

#pragma once

#include <string>

#include "BaseOperation.hpp"
#include "util/Time.hpp"

namespace ancer {
    class Renderer;
}


namespace ancer::internal {
    void InitializeSuite();
    void ShutdownSuite();

    /// If the renderer changes, we need to make sure it knows about all of our
    /// operations so it can pass along render calls, etc.
    void SuiteUpdateRenderer();

    int CreateOperation(const std::string& suite, const std::string& operation,
                        BaseOperation::Mode mode);

    void StartOperation(int id, Duration duration, const std::string& config);
    void StopOperation(int id);
    bool OperationIsStopped(int id);

    void WaitForOperation(int id);

    template <typename Func>
    void ForEachOperation(Func&&);
}

#include "Suite.inl"
