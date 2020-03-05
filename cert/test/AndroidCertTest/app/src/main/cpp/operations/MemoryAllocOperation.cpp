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

/**
 * This test intends to exhaust the device memory through recurring allocation. It's ready to free
 * all allocated memory upon a system sign that is based on different test scenarios. These are:
 *
 * * Out-of-Memory (OOM). Android has a scoring mechanism to measure chances for an app to hit the
 *                        memory heap limit. We release memory when the score reaches certain limit.
 * * Low Memory. The OS turns on an indication that the process is under a low memory scenario. We
 *               release memory when such indication is on.
 * * Try Alloc. We allocate memory until the OS denegates further allocation. Then we release all.
 * * Commit Limit. The amount of RAM plus pagefile (virtual memory). When the total allocated memory
 *                 exceeds this limit, we release all.
 *
 * Input configuration:
 * - scenario: which of the constrained memory scenarios this test is about. Possible values are
 *             oom, low_memory, try_alloc and memory_limit.
 * - alloc_size_bytes: how much memory, in bytes, to allocate at a time. Default: zero.
 * - alloc_period: interval between two allocation loop iterations. Default: no wait.
 * - on_trim_triggers_free: if true, if the system issues an onTrimMemory() callback, it releases
 *                          all the allocated memory so far.
 * - restart_pause: if on_trim_triggers_free is true, time to wait before resuming the loop.
 *                  Default is one second.
 *
 * Output report:
 * - total_allocation_bytes: how much memory is being allocated since last release.
 * - sys_mem_info: memory stats gathered from the OS:
 *   - native_allocated: in bytes, system-wise.
 *   - available_memory: in bytes, system-wise.
 *   - oom_score: OS metric of proximity to the heap size limit.
 *   - low_memory: system indication of overall memory getting exhausted.
 * Besides these data points, the following are optional event indicators:
 * - is_malloc_fail: the last memory allocation was unsuccesful.
 * - is_free: if present, the trace is written upon memory release.
 *   - free_cause: the cause that corresponds to the chosen scenario.
 * - is_on_trim: Android just issued an onTrimMemory() callback.
 *   - on_trim_level: which level of trimming the OS called back with.
 *   - total_on_trims: accumulative counter.
 *   - trim_level_histogram: map of on_trim_level and the many times each was trimmed.
 * - is_restart: the allocation loop resumes after the pause imposed by an onTrimMemory() event.
 */

#include <thread>
#include <mutex>
#include <sstream>
#include <list>

#include <ancer/BaseOperation.hpp>
#include <ancer/DatumReporting.hpp>
#include <ancer/System.hpp>
#include <ancer/util/Json.hpp>

using namespace ancer;


//==================================================================================================

namespace {
constexpr Log::Tag TAG{"MemoryAllocOperation"};

constexpr auto kBytesPerMb = 1024.0*1024.0;
constexpr auto kMbPerByte = 1/kBytesPerMb;
constexpr auto kOomScoreLimit = 650;
constexpr auto kTryAllocSize = 32*1024*1024;

constexpr const char *kScenarioOom = "oom";
constexpr const char *kScenarioLowMemory = "low_memory";
constexpr const char *kScenarioTryAlloc = "try_alloc";
constexpr const char *kScenarioCommitLimit = "commit_limit";
}

//==================================================================================================

namespace {
struct configuration {
  long alloc_size_bytes = 0;
  Milliseconds alloc_period = Milliseconds::zero();
  Milliseconds restart_pause = 1000ms;
  std::string scenario;
  bool on_trim_triggers_free = false;
};

JSON_READER(configuration) {
  JSON_REQVAR(scenario);
  JSON_REQVAR(alloc_size_bytes);
  JSON_REQVAR(alloc_period);
  JSON_REQVAR(on_trim_triggers_free);
  JSON_OPTVAR(restart_pause);
}

//--------------------------------------------------------------------------------------------------

struct sys_mem_info {
  long native_allocated = 0;
  long available_memory = 0;
  bool low_memory = false;
  int oom_score = 0;

  sys_mem_info() = default;

  explicit sys_mem_info(const MemoryInfo &i) :
      native_allocated(i.native_heap_allocation_size), available_memory(i.available_memory),
      oom_score(i._oom_score), low_memory(i.low_memory) {
  }
};

void WriteDatum(report_writers::Struct w, const sys_mem_info &i) {
  ADD_DATUM_MEMBER(w, i, native_allocated);
  ADD_DATUM_MEMBER(w, i, available_memory);
  ADD_DATUM_MEMBER(w, i, oom_score);
  ADD_DATUM_MEMBER(w, i, low_memory);
}

//--------------------------------------------------------------------------------------------------

struct datum {
  sys_mem_info sys_mem_info;
  long total_allocation_bytes = 0;

  bool is_on_trim = false;
  int on_trim_level = 0;
  int total_on_trims = 0;
  std::map<std::string, int> trim_level_histogram;
  bool is_free = false;
  std::string free_cause;
  bool is_restart = false;
  bool is_malloc_fail = false;

  static datum success() {
    return datum{};
  }

  static datum malloc_fail() {
    datum d;
    d.is_malloc_fail = true;
    return d;
  }

  static datum
  on_trim(int trim_level, int total_on_trims, const std::map<int, int> &trim_hist) {
    datum d;
    d.is_on_trim = true;
    d.on_trim_level = trim_level;
    d.total_on_trims = total_on_trims;

    for (auto th : trim_hist) {
      d.trim_level_histogram["level_" + std::to_string(th.first)] = th.second;
    }

    return d;
  }

  static datum on_free(const std::string &cause) {
    datum d;
    d.is_free = true;
    d.free_cause = cause;
    return d;
  }

  static datum on_restart() {
    datum d;
    d.is_restart = true;
    return d;
  }
};

void WriteDatum(report_writers::Struct w, const datum &d) {
  if (d.is_free) {
    w.AddItem("is_free", true);
    ADD_DATUM_MEMBER(w, d, free_cause);
  } else if (d.is_restart) {
    w.AddItem("is_restart", true);
  } else if (d.is_on_trim) {
    w.AddItem("is_on_trim", true);
    ADD_DATUM_MEMBER(w, d, on_trim_level);
    ADD_DATUM_MEMBER(w, d, total_on_trims);
    ADD_DATUM_MEMBER(w, d, trim_level_histogram);
  } else if (d.is_malloc_fail) {
    w.AddItem("is_malloc_fail", true);
  }

  ADD_DATUM_MEMBER(w, d, total_allocation_bytes);
  w.AddItem("total_allocation_mb", d.total_allocation_bytes*kMbPerByte);
  ADD_DATUM_MEMBER(w, d, sys_mem_info);
}
}

//==================================================================================================

/*
 * MemoryAllocOperation
 */
class MemoryAllocOperation : public BaseOperation {
 public:

  MemoryAllocOperation() = default;

  ~MemoryAllocOperation() override {
    Cleanup();
  }

  void Start() override {
    BaseOperation::Start();

    _configuration = GetConfiguration<configuration>();

    // start a thread to perform allocations
    _threads.emplace_back(
        [=]() {
          _loop();
        });

    // wait for execution duration to time out
    _threads.emplace_back(
        [this]() {
          while (!IsStopped()) {
            auto now = SteadyClock::now();
            if (now - GetStartTime() > GetDuration()) {
              Stop();
              return;
            }
            std::this_thread::sleep_for(50ms);
          }
        });
  }

  void Wait() override {
    for (std::thread &t : _threads) {
      if (t.joinable()) {
        t.join();
      }
    }
  }

  void OnTrimMemory(int level) override {
    BaseOperation::OnTrimMemory(level);
    Log::I(TAG, "OnTrimMemory(%d)", level);

    {
      std::lock_guard<std::mutex> lock(_buffer_lock);

      _total_on_trims++;
      _trim_histogram[level]++;

      ReportDatum(datum::on_trim(level, _total_on_trims, _trim_histogram));
    }

    if (_configuration.on_trim_triggers_free) {
      // now that we've received an OnTrim, free, and then wait for a bit
      FreeBuffers("on_trim");

      ReportDatum(datum::on_restart());
      _pause_allocations = true;
    }
  }

 private:

  void _loop() {
    while (!IsStopped()) {
      if (_pause_allocations) {
        std::this_thread::sleep_for(_configuration.restart_pause);
        _pause_allocations = false;
      }

      // check for cleanup scenarios
      auto info = GetMemoryInfo();
      if (_configuration.scenario==kScenarioOom && info._oom_score > kOomScoreLimit) {
        FreeBuffers(kScenarioOom);
      } else if (_configuration.scenario==kScenarioLowMemory && info.low_memory) {
        FreeBuffers(kScenarioLowMemory);
      } else if (_configuration.scenario==kScenarioTryAlloc
          && !TryAlloc(kTryAllocSize)) {
        FreeBuffers(kScenarioTryAlloc);
      } else if (_configuration.scenario==kScenarioCommitLimit
          && info.native_heap_allocation_size > info.commit_limit) {
        FreeBuffers(kScenarioCommitLimit);
      }

      if (!IsStopped()) {
        std::this_thread::sleep_for(_configuration.alloc_period);
        Alloc(static_cast<size_t>(_configuration.alloc_size_bytes));
      }
    }
  }

  void ReportDatum(datum &&d) {
    if (GetMode()==Mode::DataGatherer) {
      // stuff in the common information all reports will bear
      d.sys_mem_info = sys_mem_info(GetMemoryInfo());
      d.total_allocation_bytes = _total_allocation_bytes;
      Report(d);
    }
  }

  void Alloc(size_t byte_count) {
    std::lock_guard<std::mutex> lock(_buffer_lock);
    auto data = static_cast<char *>(malloc(byte_count));
    if (!data) {
      Log::W(
          TAG,
          "malloc() returned null; current allocation total is: %.2f (mb) - stopping...",
          (_total_allocation_bytes*kMbPerByte));
      ReportDatum(datum::malloc_fail());
      FreeBuffers("malloc_fail");
      Stop();
    } else {
      _buffers.push_back(data);
      _total_allocation_bytes += byte_count;
      for (size_t count = 0; count < byte_count; count++) {
        data[count] = (char) count;
      }

      ReportDatum(datum::success());
    }
  }

  bool TryAlloc(size_t byte_count) {
    char *data = static_cast<char *>(malloc(byte_count));
    if (data) {
      free(data);
      return true;
    }
    return false;
  }

  void FreeBuffers(const std::string &cause) {
    ReportDatum(datum::on_free(cause));

    std::lock_guard<std::mutex> lock(_buffer_lock);
    RunSystemGc();
    Cleanup();
    _total_allocation_bytes = 0;
  }

  void Cleanup() {
    while (!_buffers.empty()) {
      free(_buffers.front());
      _buffers.pop_front();
    }
  }

 private:

  std::vector<std::thread> _threads;

  std::mutex _buffer_lock;
  std::list<char *> _buffers;

  configuration _configuration;
  long _total_on_trims = 0;
  long _total_allocation_bytes = 0;
  bool _pause_allocations = false;
  std::map<int, int> _trim_histogram;
};

EXPORT_ANCER_OPERATION(MemoryAllocOperation)
