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
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>

#include <GLES3/gl32.h>
#include <jni.h>

#include "BaseOperation.hpp"
#include "util/GLHelpers.hpp"
#include "util/FpsCalculator.hpp"


namespace ancer {
    /**
     * BaseGLOperation
     * Base class for operations which intend to render OpenGLES output
     */
    class BaseGLES3Operation : public BaseOperation {
    public:
        BaseGLES3Operation() = default;

        void Stop() override {
            BaseOperation::Stop();
            std::lock_guard<std::mutex> guard(_stop_mutex);
            _stop_signal.notify_one();
        }

        void Wait() override {
            if ( !IsStopped()) {
                std::unique_lock<std::mutex> lock(_stop_mutex);
                _stop_signal.wait(lock);
            }
        }

    private:

        std::mutex _stop_mutex;
        std::condition_variable _stop_signal;
    };
}
