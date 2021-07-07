/*
 * Copyright 2021 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     https://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef gamecontrollersample_imguimanager_hpp
#define gamecontrollersample_imguimanager_hpp

#include "util.hpp"

/*
 * Manages the status and rendering of the ImGui system
 */
class ImGuiManager {
public:
    ImGuiManager();

    ~ImGuiManager();

    void SetDisplaySize(const int displayWidth, const int displayHeight, const int displayDpi);

    void BeginImGuiFrame();

    void EndImGuiFrame();

    float GetFontScale();

    void SetFontScale(const float fontScale);

private:
    DeltaClock mDeltaClock;
};

#endif
