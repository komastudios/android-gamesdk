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

#include <memory>
#include <string>
#include <sys/types.h>

namespace ancer {
namespace gzip {
/**
 * Inflates a deflated asset (e.g., .gzip file).
 *
 * @param asset_path route, relative to the project assets folder.
 * @param p_deflated_size output parameter with the asset size in its deflated form.
 * @param p_inflated_size output parameter with the asset size in its inflated form.
 * @return a unique pointer to the inflated asset data.
 */
std::unique_ptr<u_char> InflateAsset(const std::string &asset_path,
                                     size_t *const p_deflated_size,
                                     size_t *const p_inflated_size);
}
}