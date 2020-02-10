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

#include <cstddef>
#include <filesystem>
#include <string>

#include <jni.h>
#include <sstream>

#include "Renderer.hpp"
#include "System.Cpu.hpp"
#include "System.Gpu.hpp"
#include "System.Memory.hpp"
#include "System.Temperature.hpp"

namespace ancer {
namespace internal {
// init/deinit calls for the framework.
void InitSystem(jobject activity, jstring internal_data_path,
                jstring raw_data_path, jstring obb_path);
void DeinitSystem();

template<typename T, typename... Args>
void CreateRenderer(Args &&...);
Renderer *GetRenderer();
void DestroyRenderer();
}

// TODO(tmillican@google.com): Would prefer to return filesystem::path, but
//  that was causing weird linker errors. :/
[[nodiscard]] std::string InternalDataPath();
[[nodiscard]] std::string RawResourcePath();
[[nodiscard]] std::string ObbPath();

/**
 * Load the text from a file in the application's assets/ folder
 */
std::string LoadText(const char *file_name);

} // namespace ancer

#include "System.inl"
