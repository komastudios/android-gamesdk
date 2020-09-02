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

#include "activity_lifecycle_state.h"

#define LOG_TAG "TuningFork_ActivityLifecycleState"
#include <fstream>
#include <sstream>

#include "Log.h"
#include "tuningfork_utils.h"

namespace tuningfork {

ActivityLifecycleState::ActivityLifecycleState() {
    std::stringstream lifecycle_path_builder;
    lifecycle_path_builder << file_utils::GetAppCacheDir() << "/tuningfork";
    file_utils::CheckAndCreateDir(lifecycle_path_builder.str());
    lifecycle_path_builder << "/lifecycle.bin";
    tf_lifecycle_path_str_ = lifecycle_path_builder.str();
    ALOGV("Path to lifecycle file: %s", tf_lifecycle_path_str_.c_str());
}

ActivityLifecycleState::~ActivityLifecycleState() {}

bool ActivityLifecycleState::SetNewState(TuningFork_LifecycleState new_state) {
    current_state_ = new_state;
    if (current_state_ == TUNINGFORK_STATE_ONSTART)
        app_on_foreground_ = true;
    else if (current_state_ == TUNINGFORK_STATE_ONSTOP)
        app_on_foreground_ = false;
    TuningFork_LifecycleState saved_state = GetStoredState();
    StoreStateToDisk(current_state_);
    ALOGV(
        "New lifecycle state recorded: %s, app on foreground: %d, saved state: "
        "%s",
        GetStateName(current_state_), app_on_foreground_,
        GetStateName(saved_state));
    if (current_state_ == TUNINGFORK_STATE_ONCREATE &&
        saved_state == TUNINGFORK_STATE_ONSTART) {
        ALOGV("Discrepancy in lifecycle states, reporting as a crash");
        return false;
    } else {
        return true;
    }
}

const char *ActivityLifecycleState::GetStateName(
    TuningFork_LifecycleState state) {
    switch (state) {
        case TUNINGFORK_STATE_UNINITIALIZED:
            return "uninitialized";
        case TUNINGFORK_STATE_ONCREATE:
            return "onCreate";
        case TUNINGFORK_STATE_ONSTART:
            return "onStart";
        case TUNINGFORK_STATE_ONSTOP:
            return "onStop";
        case TUNINGFORK_STATE_ONDESTROY:
            return "onDestroy";
    }
}

TuningFork_LifecycleState ActivityLifecycleState::GetStateFromString(
    std::string name) {
    if (name.compare("onCreate") == 0) {
        return TUNINGFORK_STATE_ONCREATE;
    } else if (name.compare("onStart") == 0) {
        return TUNINGFORK_STATE_ONSTART;
    } else if (name.compare("onStop") == 0) {
        return TUNINGFORK_STATE_ONSTOP;
    } else if (name.compare("onDestroy") == 0) {
        return TUNINGFORK_STATE_ONDESTROY;
    } else {
        return TUNINGFORK_STATE_UNINITIALIZED;
    }
}

TuningFork_LifecycleState ActivityLifecycleState::GetStoredState() {
    if (!file_utils::FileExists(tf_lifecycle_path_str_)) {
        return TUNINGFORK_STATE_UNINITIALIZED;
    }
    std::ifstream file(tf_lifecycle_path_str_);
    std::string state_name;
    file >> state_name;
    file.close();
    return GetStateFromString(state_name);
}

void ActivityLifecycleState::StoreStateToDisk(TuningFork_LifecycleState state) {
    std::ofstream file(tf_lifecycle_path_str_);
    file << GetStateName(state);
    file.close();
}

TuningFork_LifecycleState ActivityLifecycleState::GetCurrentState() {
    return current_state_;
}

bool ActivityLifecycleState::IsAppOnForeground() { return app_on_foreground_; }

}  // namespace tuningfork
