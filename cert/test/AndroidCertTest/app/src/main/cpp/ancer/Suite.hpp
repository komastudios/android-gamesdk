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

namespace ancer::swappy {
    class Renderer;
}


namespace ancer::internal {
    void InitializeSuite();
    void ShutdownSuite();

    /**
     * Assign the current SwappyRenderer instance to enable gl lifecycle
     * dispatching.
     * TODO(shamyl@google.com, tmillican@google.com): Let's investigate a
     * better mechanism for this managing this dependency
     * @param swappy_renderer
     */
    void SetSwappyRenderer(swappy::Renderer* swappy_renderer);

    int CreateOperation(
            const std::string& suite, const std::string& operation, BaseOperation::Mode mode);

    void StartOperation(int id, Duration duration, const std::string& config);
    void StopOperation(int id);
    bool OperationIsStopped(int id);

    void WaitForOperation(int id);

    template <typename Func>
    void ForEachOperation(Func&&);
} // namespace ancer::internal

#include "Suite.inl"
