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
#include "Log.h"

namespace tuningfork {

ActivityLifecycleState::ActivityLifecycleState() {}
ActivityLifecycleState::~ActivityLifecycleState() {}

void ActivityLifecycleState::SetNewState(TuningFork_LifecycleState new_state) {
    current_state_ = new_state;
    if (current_state_ == TUNINGFORK_STATE_ONSTART)
        app_on_foreground_ = true;
    else if (current_state_ == TUNINGFORK_STATE_ONSTOP)
        app_on_foreground_ = false;
    ALOGV(
        "New lifecycle state recorded: %s, app on foreground: %d",
        GetStateName(current_state_),
        app_on_foreground_);
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

TuningFork_LifecycleState ActivityLifecycleState::GetCurrentState() {
    return current_state_;
}

bool ActivityLifecycleState::IsAppOnForeground() {
    return app_on_foreground_;
}

}  // namespace tuningfork
