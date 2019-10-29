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

#include "SwappyRenderer.hpp"

#include <vector>
#include <EGL/egl.h>
#include <android/native_window.h>
#include <GLES3/gl32.h>

#include "util/Error.hpp"
#include "ancer/util/Log.hpp"

#include "swappy/swappyGL.h"
#include "swappy/swappyGL_extra.h"

#include "BaseOperation.hpp"

using namespace ancer;
using namespace std::chrono_literals;


constexpr Log::Tag TAG{"SwappyRenderer"};

namespace ancer::swappy {

    std::unique_ptr<Renderer> Renderer::Create() {
        return std::move(std::make_unique<Renderer>(ConstructorTag{}));
    }

    void Renderer::SetWindow(ANativeWindow* window, int32_t width, int32_t height) {
        _worker_thread.Run(
                [=](ThreadState* thread_state) {
                    thread_state->ClearSurface();

                    if ( !window ) return;

                    Log::D(TAG, "Creating window surface %dx%d", width, height);

                    thread_state->surface =
                            eglCreateWindowSurface(
                                    thread_state->display, thread_state->config, window,
                                    nullptr);
                    ANativeWindow_release(window);
                    if ( !thread_state->MakeCurrent(thread_state->surface)) {
                        thread_state->surface = EGL_NO_SURFACE;
                        FatalError(TAG, "Unable to eglMakeCurrent");
                    }

                    thread_state->width = width;
                    thread_state->height = height;

                    glViewport(0, 0, width, height);

                    for ( const auto& op : _operations ) {
                        op->OnGlContextReady();
                        op->OnGlContextResized(width, height);
                    }
                });
    }

    void Renderer::Start() {
        _worker_thread.Run(
                [this](ThreadState* thread_state) {
                    thread_state->is_started = true;
                    _fps_calculator.Reset();
                    RequestDraw();
                });
    }

    void Renderer::Stop() {
        for ( const auto& op : _operations ) {
            op->Stop();
        }

        _worker_thread.Run([=](ThreadState* thread_state) { thread_state->is_started = false; });
    }

    void Renderer::RequestDraw() {
        _worker_thread.Run(
                [=](ThreadState* thread_state) {
                    if ( thread_state->is_started )
                        Draw(thread_state);
                });
    }

    void Renderer::AddOperation(const std::shared_ptr<BaseOperation>& operation) {
        _operations.push_back(operation);
    }

    void Renderer::RemoveOperation(const std::shared_ptr<BaseOperation>& operation) {
        std::lock_guard<std::mutex> lockGuard(_operations_lock);
        _operations.erase(
                std::remove(_operations.begin(), _operations.end(), operation),
                _operations.end());
    }

    void Renderer::ClearOperations() {
        _operations.clear();
    }

    Renderer::ThreadState::ThreadState() {
        display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
        eglInitialize(display, 0, 0);

        const EGLint config_attributes[] = {
                EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
                EGL_BLUE_SIZE, 8,
                EGL_GREEN_SIZE, 8,
                EGL_RED_SIZE, 8,
                EGL_DEPTH_SIZE, 24,
                EGL_NONE
        };

        EGLint num_configs = 0;
        eglChooseConfig(display, config_attributes, nullptr, 0, &num_configs);
        std::vector<EGLConfig> supportedConfigs(static_cast<size_t>(num_configs));
        eglChooseConfig(
                display, config_attributes, supportedConfigs.data(), num_configs,
                &num_configs);

        // Choose a config, either a match if possible or the first config otherwise

        const auto config_matches = [&](EGLConfig config) {
            if ( !ConfigHasAttribute(EGL_RED_SIZE, 8)) return false;
            if ( !ConfigHasAttribute(EGL_GREEN_SIZE, 8)) return false;
            if ( !ConfigHasAttribute(EGL_BLUE_SIZE, 8)) return false;
            return ConfigHasAttribute(EGL_DEPTH_SIZE, 24);
        };

        const auto config_iter = std::find_if(
                supportedConfigs.cbegin(), supportedConfigs.cend(),
                config_matches);

        config = (config_iter != supportedConfigs.cend()) ? *config_iter : supportedConfigs[0];

        const EGLint context_attributes[] = {
                EGL_CONTEXT_CLIENT_VERSION, 3,
                EGL_NONE
        };

        context = eglCreateContext(display, config, nullptr, context_attributes);
        if ( context == EGL_NO_CONTEXT) {
            FatalError(TAG, "Unable to create EGL context");
        }
    }

    Renderer::ThreadState::~ThreadState() {
        ClearSurface();
        if ( context != EGL_NO_CONTEXT) eglDestroyContext(display, context);
        if ( display != EGL_NO_DISPLAY) eglTerminate(display);
    }

    void Renderer::ThreadState::ClearSurface() {
        if ( surface == EGL_NO_SURFACE) return;

        MakeCurrent(EGL_NO_SURFACE);
        eglDestroySurface(display, surface);
        surface = EGL_NO_SURFACE;
    }

    bool Renderer::ThreadState::ConfigHasAttribute(EGLint attribute, EGLint value) {
        EGLint outValue = 0;
        EGLBoolean result = eglGetConfigAttrib(display, config, attribute, &outValue);
        return result && (outValue == value);
    }

    EGLBoolean Renderer::ThreadState::MakeCurrent(EGLSurface surface) {
        return eglMakeCurrent(display, surface, surface, context);
    }

    void Renderer::Draw(ThreadState* thread_state) {
        // Don't render if we have no surface
        if ( thread_state->surface == EGL_NO_SURFACE) {
            // Sleep a bit so we don't churn too fast
            std::this_thread::sleep_for(50ms);
            RequestDraw();
            return;
        }

        SwappyGL_recordFrameStart(thread_state->display, thread_state->surface);
        double deltaS = _fps_calculator.UpdateFps();

        // clear, and draw all our gl operations
        glClearColor(0, 0, 0, 1);
        glClearDepthf(1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        {
            std::lock_guard<std::mutex> lockGuard(_operations_lock);
            for ( const auto& op : _operations ) {
                if ( !op->IsStopped()) {
                    op->Draw(deltaS);
                }
            }
        }

        SwappyGL_swap(thread_state->display, thread_state->surface);

        // request next frame dispatch
        RequestDraw();
    }
} // namespace ancer::swappy
