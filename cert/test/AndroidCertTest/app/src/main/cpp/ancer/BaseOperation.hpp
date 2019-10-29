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

#include <condition_variable>
#include <exception>
#include <memory>
#include <string>

#include <EGL/egl.h>
#include <GLES3/gl32.h>
#include <Trace.h>

#include "Reporting.hpp"
#include "util/Error.hpp"
#include "util/FpsCalculator.hpp"
#include "util/GLHelpers.hpp"
#include "util/Json.hpp"
#include "util/Log.hpp"
#include "util/Time.hpp"

#define CONCAT_IMPL(x, y) x##y
#define MACRO_CONCAT(x, y) CONCAT_IMPL( x, y )
#define ANCER_SCOPED_TRACE(desc) samples::ScopedTrace MACRO_CONCAT( trace_, __COUNTER__ )( desc )

namespace ancer {
    /**
     * Base class for Operations
     */
    class BaseOperation {
    public:

        enum class Mode {
            /**
             * Run the operation in data gathering mode, e.g., it will record profiling data at runtime,
             * and return it when the test is done and wait() is called
             */
                    DataGatherer,
            /**
             * Run the operation in Stressor mode; no logging or profiling is recorded or returned
             */
                    Stressor,
        };

    public:
        /**
         * Classload a BaseOperation implementation by name
         * @param operation_id name of the operation impl, e.g., "CalculatePIOperation" (minus "operations" namespace)
         * @param suite_id name of the suite this operation will be running in
         * @param mode the mode this operation should run in (see BaseOperation::Mode)
         * @return instance of the operation, or nullptr
         */
        static std::shared_ptr<BaseOperation> Load(
                const std::string& operation_id, const std::string& suite_id, Mode mode);

    public:

        BaseOperation() = default;

        virtual ~BaseOperation();

        /**
         * Initialize an operation
         * @param duration the time to execute for when start() is called. If 0, run continuously until stop() is called.
         * @param json_configuration the operation's domain-specific configuration json string
         */
        void Init(Duration duration, const std::string& json_configuration) {
            _duration = duration;
            _json_configuration = json_configuration;
        }

        /**
         * Asynchronously start execution of the operation on a thread. Operation will continue
         * until the specified execution duration times out, or until stop() is called.
         * NOTE: Always call inherited implementation
         */
        virtual void Start();

        /**
         * Wait (blocking) for execution to complete. If the operation has a > 0 duration, this will
         * block until the operation's execution has determined the duration has expired, otherwise,
         * will block until stop() is called signalling immediate termination
         */
        virtual void Wait() = 0;

        /**
         * Invoked when the hosting android activity receives an OnTrimMemory call
         * @param level the level received by the host activity
         */
        virtual void OnTrimMemory(int level) {}

        /**
         * Flag that the operation is canceled. Implementations should monitor this to quit their work.
         */
        virtual void Stop() {
            _stop_ordered.store(true, std::memory_order_relaxed);
        }

        /**
         * @return true if this operation has been flagged to stop
         */
        bool IsStopped() const { return _stop_ordered.load(std::memory_order_relaxed); }

        /**
         * Callback invoked when the opengl context is ready.
         * NOTE: Non-OpenGL operations can ignore this.
         */
        virtual void OnGlContextReady() {}

        /**
         * Callback invoked when its time to render.
         * NOTE: ALWAYS call inherited implementation
         * NOTE: Non-OpenGL operations can ignore this.
         * @param delta_seconds elapsed time in seconds since last draw call
         */
        virtual void Draw(double delta_seconds);

        /**
         * Invoked when the OpenGL context size has changed. This will be called
         * after OnGlContextReady
         * NOTE: ALWAYS call inherited implementation
         * NOTE: Non-OpenGL operations can ignore this.
         * @param width
         * @param height
         */
        virtual void OnGlContextResized(int width, int height);

        /**
         * Get current size of opengl context. If context isn't ready yet, this will return (0,0)
         */
        glm::ivec2 GetGlContextSize() const { return glm::ivec2(_width, _height); }

        /**
         * If > 0, OnHeartbeat will be called regularly
         * @param period How often to call OnHeartbeat
         */
        void SetHeartbeatPeriod(Duration period) {
            _heartbeat_period = period;
        }

        [[nodiscard]] auto GetHeartbeatPeriod() const noexcept { return _heartbeat_period; }

        /**
         * This method will be called periodically while running at a rate set via SetHeartbeatPeriod
         * @param delta The time since the completion of the last time this was invoked.
         */
        virtual void OnHeartbeat(Duration elapsed) {}

        /**
         * @return the execution mode of this operation, see BaseOperation::Mode
         */
        Mode GetMode() const { return _mode; }

    protected:

        template <typename T>
        T GetConfiguration() {
            try {
                return Json::parse(_json_configuration).get<T>();
            } catch ( std::exception& e ) {
                FatalError(
                        Log::Tag{"configuration"},
                        "Encounted an exception when reading the configuration: %s", e.what());
            }
        }

        /**
         * @return the duration in this operation should run for, if it is a Mode::DataGatherer.
         */
        [[nodiscard]] auto GetDuration() const noexcept { return _duration; }

        [[nodiscard]] auto GetStartTime() const noexcept { return _start_time; }

        //
        //  Helper functions
        //

        template <typename T>
        void Report(const T& payload) {
            ReportImpl(Json(payload));
        }

    private:

        void ReportImpl(const Json& custom_payload);

    private:

        std::atomic<bool> _stop_ordered{false};

        Mode _mode = Mode::Stressor;
        std::string _suite_id;
        std::string _operation_id;
        std::string _json_configuration;

        int _width = 0;
        int _height = 0;
        Duration _duration = Duration::zero();
        Timestamp _start_time;
        Duration _heartbeat_period = Duration::zero();
        Timestamp _heartbeat_timestamp;
    };
}

#define EXPORT_ANCER_OPERATION(name) \
    extern "C" __attribute__((visibility ("default"))) \
    void name ## _CreateDataGatherer(std::shared_ptr<BaseOperation> &into){ into = std::make_shared<name>(); } \
    extern "C" __attribute__((visibility ("default"))) \
    void name ## _CreateStressor(std::shared_ptr<BaseOperation> &into){ into = std::make_shared<name>(); } \
