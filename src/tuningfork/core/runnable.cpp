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

#include "runnable.h"

#define LOG_TAG "TuningFork"
#include "Log.h"

namespace tuningfork {

void Runnable::Start() {
    if (thread_) {
        ALOGW("Can't start an already running thread");
        return;
    }
    do_quit_ = false;
    thread_ = std::make_unique<std::thread>([&] { Run(); });
}
void Runnable::Run() {
    while (!do_quit_) {
        std::unique_lock<std::mutex> lock(mutex_);
        auto wait_time = DoWork();
        if (time_provider_ == nullptr) {
            cv_.wait_for(lock, wait_time);
        } else {
        }
    }
}
void Runnable::Stop() {
    if (!thread_->joinable()) {
        ALOGW("Can't stop a thread that's not started");
        return;
    }
    do_quit_ = true;
    cv_.notify_one();
    thread_->join();
}

}  // namespace tuningfork