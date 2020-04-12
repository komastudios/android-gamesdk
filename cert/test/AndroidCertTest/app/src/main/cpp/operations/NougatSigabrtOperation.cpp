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

/**
 * This operation is only meant to Android Nougat devices, although it's not prevented to run on
 * any device as well (although, it's expected to pass).
 *
 * Context: the first Nougat releases came with a bug that, on certain devices, caused a
 * RenderThread to raise a signal abort (SIGABRT) under memory-constrained scenarios. That signal
 * led to a crash. The bug was ultimately fixed on subsequent releases, although not all existing
 * Nougat models got the alleged update.
 * Based on some community threads, one of the many ways to repro the crash was to launch the camera
 * from an app in the midst of a memory-constrained scenario. That's exactly how this test work.
 *
 * Upon creation, this operation traps SIGABRT signal in order to stay in control in case the signal
 * that leads to this crash it's raised. Next, it starts allocating memory in order to provoke a
 * memory constraint. Once the memory constraint occurs, it launches a camera activity, provided
 * that that would raise the SIGABRT signal if the Nougat device isn't patched.
 *
 * If the signal is raised, as it's trapped, the operation recovers the control and reports that the
 * crash still occurs for that device model.
 *
 *
 *
 * Input configuration: none.
 *
 * Output report: one or more traces containing
 * - event: a string whose values range between:
 *   - "SIGABRT": a SIGABRT signal got captured. Namely, the crash we were looking for.
 *   - "on_trim_memory": the operation got an OnTrimMemory request forwarded.
 *   - "test_ends": the operation is finishing. The absence of this one would mean that the
 *                  operation didn't finish normally.
 *
 *
 *
 * Special considerations:
 * - Due to specific needs only achievable from a host activity, this operation must be run from a
 *   NougatSigabrtHostActivity exclusively.
 * - In order to generate the conditions for an OnTrimMemory(level) call, while gathering data from
 *   this operation, a stressor on MemoryAllocOperation is recommended.
 */

#include <csetjmp>
#include <csignal>
#include <cstdlib>
#include <functional>
#include <list>
#include <mutex>
#include <string>
#include <thread>

#include <ancer/BaseOperation.hpp>
#include <ancer/DatumReporting.hpp>
#include <ancer/System.hpp>
#include <ancer/util/Json.hpp>
#include <ancer/util/Log.hpp>

using namespace ancer;

#define CRASH_WAS_DETECTED()    (setjmp(::jump_buffer))

namespace {
constexpr Log::Tag TAG{"NougatSigabrtOperation"};
}

//==================================================================================================

namespace {

/**
 * Event description when the OS broadcasted an onTrimMemory call.
 */
constexpr const char *kOnTrimMemoryEvent{"on_trim_memory"};

/**
 * Event description when a component raised a signal abort (likely, the RenderThread.)
 */
constexpr const char *kSigabrtEvent{"SIGBART"};

/**
 * Event description when this data gatherer operation finishes.
 */
constexpr const char *kTestEndsEvent{"test_ends"};

struct Datum {
  std::string event;
};

void WriteDatum(report_writers::Struct w, const Datum &d) {
  ADD_DATUM_MEMBER(w, d, event);
}

}

//==================================================================================================

namespace {

using event_function_t = std::function<void(Datum &&)>;
event_function_t event_function{nullptr};

std::jmp_buf jump_buffer;

/**
 * After calling this function, if a SIGABRT is raised, instead of crashing it will be trapped and
 * reported. This function receives a reference to a function that reports events.
 */
void StartTrappingAbortSignal(const event_function_t &);

/**
 * Upon trapped SIGABRT signals, this function is invoked to report the event.
 */
void SigabrtHandler(int);

/**
 * After calling this function, if a SIGABRT is raised, the consequent crash won't be trapped
 * anymore.
 */
void StopTrappingAbortSignal();

}

//--------------------------------------------------------------------------------------------------

namespace {

void StartTrappingAbortSignal(const event_function_t &event_fn) {
  ::event_function = event_fn;
  std::signal(SIGABRT, ::SigabrtHandler);
  Log::I(TAG, "Started trapping abort signals (SIGABRT)");
}

void SigabrtHandler(int signal) {
  if (SIGABRT == signal) {
    event_function_t event_fn{::event_function};
    ::StopTrappingAbortSignal();

    Log::I(TAG, "Trapped signal is SIGABRT");

    if (event_fn) {
      event_fn(::Datum{kSigabrtEvent});
    }

    longjmp(::jump_buffer, 1);
  } else {
    Log::I(TAG, "Trapped signal (%d)", signal);
  }
}

void StopTrappingAbortSignal() {
  std::signal(SIGABRT, SIG_DFL);
  ::event_function = nullptr;
  Log::I(TAG, "Stopped trapping abort signals (SIGABRT)");
}

}

//==================================================================================================

/**
 * Functional test 1.20, Nougat Crash (SIGABRT) Operation. Needs to be run from its special, ad-hoc
 * NougatSigabrtHostActivity.
 */
class NougatSigabrtOperation : public BaseOperation {
 public:

  NougatSigabrtOperation() = default;

  ~NougatSigabrtOperation() override {
    ::StopTrappingAbortSignal();
  }

  void Start() override {
    BaseOperation::Start();

    _thread = std::thread{[this] {
      Log::I(TAG, "Working thread started");

      ::StartTrappingAbortSignal(std::bind(&NougatSigabrtOperation::Report<::Datum>,
                                           this,
                                           std::placeholders::_1));

      // Thread lives as long as the operation doesn't naturally time up or a crash isn't detected,
      // whichever happens first.
      while (not(IsStopped() || CRASH_WAS_DETECTED()));
      Log::I(TAG, "Working thread finished");
    }};

    Log::I(TAG, "Operation started");
  }

  void Wait() override {
    _thread.join();

    Report(::Datum{kTestEndsEvent});

    Log::I(TAG, "Operation wait completed");
  }

  /**
   * This is a precondition for the possible crash to happen: that memory was running low. It's not
   * a guarantee that the RenderThread crash will occur but, if it happens, it's because memory was
   * running low.
   * We procure memory to start faltering with a MemoryAllocOperation stressor. We register the
   * occurrence of a low-memory scenario in this function.
   */
  void OnTrimMemory(int level) override {
    Log::I(TAG, "OnTrimMemory invoked.");
    BaseOperation::OnTrimMemory(level);

    Report(::Datum{kOnTrimMemoryEvent});
  }

//--------------------------------------------------------------------------------------------------

 private:
  std::thread _thread;
};

EXPORT_ANCER_OPERATION(NougatSigabrtOperation)
