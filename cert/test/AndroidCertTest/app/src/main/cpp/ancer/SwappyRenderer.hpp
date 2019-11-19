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
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <EGL/egl.h>

#include "System.hpp"
#include "Thread.h"
#include "util/WorkerThread.hpp"
#include "util/FpsCalculator.hpp"

namespace ancer {
class BaseOperation;
}

namespace ancer::swappy {

class Renderer {
  // Allows construction with std::unique_ptr from a static method, but disallows construction
  // outside of the class since no one else can construct a ConstructorTag
  struct ConstructorTag {
  };

  class ThreadState {
   public:
    ThreadState(GLContextConfig preferred_ctx_config,
                GLContextConfig fallback_ctx_config);

    ~ThreadState();

    void ClearSurface();

    bool ConfigHasAttribute(EGLint attribute, EGLint value);

    EGLBoolean MakeCurrent(EGLSurface surface);

    EGLDisplay display = EGL_NO_DISPLAY;
    EGLConfig config = static_cast<EGLConfig>(0);
    EGLSurface surface = EGL_NO_SURFACE;
    EGLContext context = EGL_NO_CONTEXT;

    bool is_started = false;
    int32_t width = 0;
    int32_t height = 0;
    GLContextConfig using_gl_context_config;
   private:
    bool TryCreateContext(GLContextConfig config);
  };

 public:
  explicit Renderer(ConstructorTag,
                    const GLContextConfig& preferred_ctx_config,
                    const GLContextConfig& fallback_ctx_config) :
      _preferred_ctx_config(preferred_ctx_config),
      _fallback_ctx_config(fallback_ctx_config),
      _worker_thread(

          "Renderer",

          // TODO(shamyl@google.com): Find a better way to assign render thread core
          samples::Affinity::Odd,

          [preferred_ctx_config, fallback_ctx_config]() {
            return ThreadState{preferred_ctx_config, fallback_ctx_config};
          }
      ) {}

  static std::unique_ptr<Renderer> Create(
      const GLContextConfig& preferred_ctx_config,
      const GLContextConfig& fallback_ctx_config);

  // Sets the active window to render into
  // Takes ownership of window and will release its reference
  void SetWindow(ANativeWindow* window, int32_t width, int32_t height);

  void Start();

  void Stop();

  void RequestDraw();

  void AddOperation(const std::shared_ptr<BaseOperation>& operation);

  void RemoveOperation(const std::shared_ptr<BaseOperation>& operation);

  void ClearOperations();

 private:

  void Draw(ThreadState* thread_state);

  WorkerThread<ThreadState> _worker_thread;
  std::mutex _operations_lock;
  std::vector<std::shared_ptr<BaseOperation>> _operations;
  FpsCalculator _fps_calculator;
  GLContextConfig _preferred_ctx_config;
  GLContextConfig _fallback_ctx_config;

};
} // namespace ancer::swappy
