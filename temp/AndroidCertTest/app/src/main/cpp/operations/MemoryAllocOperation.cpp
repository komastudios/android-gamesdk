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
#include <thread>
#include <mutex>
#include <sstream>
#include <list>

#include <ancer/BaseOperation.hpp>
#include <ancer/System.hpp>
#include <ancer/util/Json.hpp>

using namespace ancer;


//==================================================================================================

namespace {
    constexpr Log::Tag TAG{"MemoryAllocOperation"};
    constexpr auto BYTES_PER_MB = 1024.0 * 1024.0;
    constexpr auto MB_PER_BYTE = 1 / BYTES_PER_MB;
    constexpr auto SCENARIO_TRY_ALLOC_BYTES = 32 * 1024 * 1024;

    constexpr const char* SCENARIO_OOM = "oom";
    constexpr const char* SCENARIO_LOW_MEMORY = "low_memory";
    constexpr const char* SCENARIO_TRY_ALLOC = "try_alloc";
    constexpr const char* SCENARIO_COMMIT_LIMIT = "commit_limit";
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

        explicit sys_mem_info(const MemoryInfo& i) :
                native_allocated(i.native_heap_allocation_size), available_memory(i.available_memory)
                , oom_score(i._oom_score), low_memory(i.low_memory) {}
    };

    JSON_WRITER(sys_mem_info) {
        JSON_REQVAR(native_allocated);
        JSON_REQVAR(available_memory);
        JSON_REQVAR(oom_score);
        JSON_REQVAR(low_memory);
    }

//--------------------------------------------------------------------------------------------------

    struct datum {
        Timestamp timestamp;
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
        on_trim(int trim_level, int total_on_trims, const std::map<int, int>& trim_hist) {
            datum d;
            d.is_on_trim = true;
            d.on_trim_level = trim_level;
            d.total_on_trims = total_on_trims;

            for ( auto th : trim_hist ) {
                d.trim_level_histogram["level_" + std::to_string(th.first)] = th.second;
            }

            return d;
        }

        static datum on_free(const std::string& cause) {
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

    JSON_WRITER(datum) {
        if ( data.is_free ) {
            JSON_SETVAR(is_free, true);
            JSON_REQVAR(free_cause);
        } else if ( data.is_restart ) {
            JSON_SETVAR(is_restart, true);
        } else if ( data.is_on_trim ) {
            JSON_SETVAR(is_on_trime, true);
            JSON_REQVAR(on_trim_level);
            JSON_REQVAR(total_on_trims);
            JSON_REQVAR(trim_level_histogram);
        } else if ( data.is_malloc_fail ) {
            JSON_SETVAR(is_malloc_fail, true);
        }

        JSON_REQVAR(timestamp);
        JSON_REQVAR(total_allocation_bytes);
        JSON_SETVAR(total_allocation_mb, data.total_allocation_bytes * MB_PER_BYTE);
        JSON_REQVAR(sys_mem_info);
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
                    while ( !IsStopped()) {
                        auto now = SteadyClock::now();
                        if ( now - GetStartTime() > GetDuration()) {
                            Stop();
                            return;
                        }
                        std::this_thread::sleep_for(50ms);
                    }
                });
    }

    void Wait() override {
        for ( std::thread& t : _threads ) {
            if ( t.joinable()) {
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

        if ( _configuration.on_trim_triggers_free ) {
            // now that we've received an OnTrim, free, and then wait for a bit
            FreeBuffers("on_trim");

            ReportDatum(datum::on_restart());
            _pause_allocations = true;
        }
    }

private:

    void _loop() {
        while ( !IsStopped()) {
            if ( _pause_allocations ) {
                std::this_thread::sleep_for(_configuration.restart_pause);
                _pause_allocations = false;
            }

            // check for cleanup scenarios
            auto info = GetMemoryInfo();
            if ( _configuration.scenario == SCENARIO_OOM && info._oom_score > 650 ) {
                FreeBuffers(SCENARIO_OOM);
            } else if ( _configuration.scenario == SCENARIO_LOW_MEMORY && info.low_memory ) {
                FreeBuffers(SCENARIO_LOW_MEMORY);
            } else if ( _configuration.scenario == SCENARIO_TRY_ALLOC
                    && !TryAlloc(SCENARIO_TRY_ALLOC_BYTES)) {
                FreeBuffers(SCENARIO_TRY_ALLOC);
            } else if ( _configuration.scenario == SCENARIO_COMMIT_LIMIT
                    && info.native_heap_allocation_size > info.commit_limit ) {
                FreeBuffers(SCENARIO_COMMIT_LIMIT);
            }

            if ( !IsStopped()) {
                std::this_thread::sleep_for(_configuration.alloc_period);
                Alloc(static_cast<size_t>(_configuration.alloc_size_bytes));
            }
        }
    }

    void ReportDatum(datum&& d) {
        if ( GetMode() == Mode::DataGatherer ) {
            // stuff in the common information all reports will bear
            d.sys_mem_info = sys_mem_info(GetMemoryInfo());
            d.timestamp = SteadyClock::now();
            d.total_allocation_bytes = _total_allocation_bytes;
            Report(d);

            Log::D(TAG, "_report: " + Json(d).dump());
        }
    }

    void Alloc(size_t byte_count) {
        std::lock_guard<std::mutex> lock(_buffer_lock);
        auto data = static_cast<char*>(malloc(byte_count));
        if ( !data ) {
            Log::W(
                    TAG,
                    "malloc() returned null; current allocation total is: %.2f (mb) - stopping...",
                    (_total_allocation_bytes * MB_PER_BYTE));
            ReportDatum(datum::malloc_fail());
            FreeBuffers("malloc_fail");
            Stop();
        } else {
            _buffers.push_back(data);
            _total_allocation_bytes += byte_count;
            for ( size_t count = 0 ; count < byte_count ; count++ ) {
                data[count] = (char)count;
            }

            ReportDatum(datum::success());
        }
    }

    bool TryAlloc(size_t byte_count) {
        char* data = static_cast<char*>(malloc(byte_count));
        if ( data ) {
            free(data);
            return true;
        }
        return false;
    }

    void FreeBuffers(const std::string& cause) {
        ReportDatum(datum::on_free(cause));

        std::lock_guard<std::mutex> lock(_buffer_lock);
        RunSystemGc();
        Cleanup();
        _total_allocation_bytes = 0;
    }

    void Cleanup() {
        while ( !_buffers.empty()) {
            free(_buffers.front());
            _buffers.pop_front();
        }
    }

private:

    std::vector<std::thread> _threads;

    std::mutex _buffer_lock;
    std::list<char*> _buffers;

    configuration _configuration;
    long _total_on_trims = 0;
    long _total_allocation_bytes = 0;
    bool _pause_allocations = false;
    std::map<int, int> _trim_histogram;
};

EXPORT_ANCER_OPERATION(MemoryAllocOperation)