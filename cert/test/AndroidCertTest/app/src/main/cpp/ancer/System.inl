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

#pragma once
#include "System.hpp"

#include <memory>

namespace ancer::internal {
    // In suite.hpp, re-declared here to avoid includes.
    void SuiteUpdateRenderer();
}


namespace ancer::internal {
    inline std::unique_ptr<Renderer> _system_renderer;

    template <typename T, typename... Args>
    void CreateRenderer(Args&&... args) {
        assert(_system_renderer == nullptr);
        _system_renderer = T::Create(std::forward<Args>(args)...);
        SuiteUpdateRenderer();
    }

    inline Renderer* GetRenderer() {
        return _system_renderer.get();
    }

    inline void DestroyRenderer() {
        _system_renderer.reset();
    }
}