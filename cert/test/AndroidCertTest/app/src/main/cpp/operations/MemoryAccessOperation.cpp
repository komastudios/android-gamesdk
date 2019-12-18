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
#include <ancer/BaseOperation.hpp>

#include <limits>
#include <random>
#include <thread>
#include <variant>

#include <ancer/Reporter.hpp>
#include <ancer/System.hpp>
#include <ancer/util/Basics.hpp>
#include <ancer/util/Bitmath.hpp>
#include <ancer/util/Json.hpp>

using namespace ancer;
using Bytes = bitmath::Bytes;


//==============================================================================

namespace {
    constexpr Log::Tag TAG{"MemoryAccessOperation"};
}

//==============================================================================

namespace {
    // We use this instead of something like std::vector to avoid the overhead
    // of initialization: The initial write is usually something we'll actually
    // handle as a part of the test.
    class FixedBuffer {
    public:
        FixedBuffer(size_t size)
        : _size(size)
        , _buffer((char*)malloc(size)) {
        }
        FixedBuffer(const FixedBuffer&) = delete;
        ~FixedBuffer() {
            free(_buffer);
        }

        [[nodiscard]] auto size() const noexcept { return _size; }
        [[nodiscard]] auto data() const noexcept { return _buffer; }
    private:
        size_t _size;
        char* _buffer;
    };
}

//==============================================================================

namespace {
    enum class ThreadSetup {
        kOneCore, kAllCores, kBigCores, kLittleCores, kMiddleCores
    };
    constexpr const char* const kThreadSetupNames[] = {
        "Single Core", "All Cores", "Big Cores", "Little Cores", "Middle Cores"
    };
    constexpr auto ToAffinity(ThreadSetup setup) {
        switch (setup) {
            case ThreadSetup::kBigCores:    return ThreadAffinity::kBigCore;
            case ThreadSetup::kLittleCores: return ThreadAffinity::kLittleCore;
            case ThreadSetup::kMiddleCores: return ThreadAffinity::kMiddleCore;
            case ThreadSetup::kAllCores:
            case ThreadSetup::kOneCore:     return ThreadAffinity::kAnyCore;
            default:
                FatalError(TAG, "Unknown thread setup %d", (int)setup);
        }
    }

    enum class WorkScheme {
        kDividedEvenly, kInterleaved
    };
    constexpr const char* const kWorkSchemeNames[] = {"Divided", "Interleaved"};


    // Performs operations that have a regular pattern.
    struct FixedOpConfig {
        ThreadSetup threads;
        WorkScheme work_scheme;

        // How many read/writes to perform.
        // Note that we will not continue past the end of the buffer.
        int times = std::numeric_limits<int>::max();
        Bytes advance; // Advance by this much per read/write.
        Bytes initial_offset = 0; // How far off to start the first read/write.
    };

    JSON_READER(FixedOpConfig) {
        JSON_REQENUM(threads, kThreadSetupNames);
        JSON_OPTENUM(work_scheme, kWorkSchemeNames);
        JSON_OPTVAR(times);
        JSON_REQVAR(advance);
        JSON_OPTVAR(initial_offset);
    }

    //--------------

    // Performs operations while advancing irregularly through the buffer.
    // Single-threaded only to avoid introducing too much chaos.
    struct IrregularOpConfig {
        // Note: If we reach the end of the buffer, we'll loop if necessary.
        size_t times;
        Bytes rw_align;
        Bytes min_advance;
        Bytes max_advance;
    };

    JSON_READER(IrregularOpConfig) {
        JSON_REQVAR(times);
        JSON_REQVAR(rw_align);
        JSON_REQVAR(min_advance);
        JSON_REQVAR(max_advance);
    }

    //--------------

    // Performs operations at random points within the buffer.
    // Single-threaded only to avoid introducing too much chaos.
    struct RandomOpConfig {
        size_t times;
        Bytes rw_align;
    };

    JSON_READER(RandomOpConfig) {
        JSON_REQVAR(times);
        JSON_REQVAR(rw_align);
    }

    //--------------

    // Threads gobble up the data in-order as quickly as they can.
    // Multithreaded only since it doesn't make much sense otherwise.
    struct GreedyOpConfig {
        ThreadSetup threads;
        Bytes advance;
    };

    JSON_READER(GreedyOpConfig) {
        JSON_REQENUM(threads, kThreadSetupNames);
        JSON_REQVAR(advance);
    }

//------------------------------------------------------------------------------

    enum class ReadOrWrite { kRead, kWrite };
    constexpr const char* const kReadWriteNames[] = {"Read", "Write"};

    enum class BufferOpType { kFixedOp, kIrregularOp, kRandomOp, kGreedyOp };
    constexpr const char* const kBufferOpTypeNames[] = {
        "Fixed", "Irregular", "Random", "Greedy"
    };


    struct BufferOpConfig {
        std::string description;
        bool append_graph = false;
        ReadOrWrite read_write;
        Bytes rw_size;
        BufferOpType operation_type;
        std::variant<FixedOpConfig, IrregularOpConfig,
                     RandomOpConfig, GreedyOpConfig> config;
    };

    JSON_READER(BufferOpConfig) {
        JSON_OPTVAR(description);
        JSON_OPTVAR(append_graph);
        JSON_REQENUM(read_write, kReadWriteNames);
        JSON_REQVAR(rw_size);
        JSON_REQENUM(operation_type, kBufferOpTypeNames);
        switch(data.operation_type) {
            case BufferOpType::kFixedOp:
                JSON_REQVAR_AT("config", data.config.template emplace<FixedOpConfig>());
                break;
            case BufferOpType::kIrregularOp:
                JSON_REQVAR_AT("config", data.config.template emplace<IrregularOpConfig>());
                break;
            case BufferOpType::kRandomOp:
                JSON_REQVAR_AT("config", data.config.template emplace<RandomOpConfig>());
                break;
            case BufferOpType::kGreedyOp:
                JSON_REQVAR_AT("config", data.config.template emplace<GreedyOpConfig>());
                break;
            default:
                FatalError(TAG, "Unknown operation type %d", data.operation_type);
        }
    }

    //--------------

    struct Configuration {
        Bytes buffer_size;
        Milliseconds report_rate;
        std::vector<BufferOpConfig> operations;
    };

    JSON_READER(Configuration) {
        JSON_REQVAR(buffer_size);
        JSON_REQVAR(report_rate);
        JSON_REQVAR(operations);
    }

//------------------------------------------------------------------------------

    struct Datum {
        int pass_id;
        int thread_id;
        Bytes bytes_update = {0};
        Bytes bytes_cumulative = {0};
    };

    void Reset(Datum& datum) {
        datum.bytes_update = Bytes{0};
    }

    JSON_WRITER(Datum) {
        JSON_REQVAR(pass_id);
        JSON_REQVAR(thread_id);
        JSON_REQVAR(bytes_update);
        JSON_REQVAR(bytes_cumulative);
    }
}

//==============================================================================
// Read/Write

namespace {
    // TODO(tmillican@google.com): Similar to I/O logging, and it's a bit of an
    //  ugly setup. Probably worth investing a bit of time for an "official"
    //  helper for this pattern.
    struct WRReporter {
        Reporter<Datum> reporter;
        Milliseconds report_rate;
        Timestamp last_report = SteadyClock::now();

        ~WRReporter() {
            reporter();
        }

        void operator () (Bytes new_bytes) {
            reporter.datum.bytes_update += new_bytes;
            reporter.datum.bytes_cumulative += new_bytes;
            const auto now = SteadyClock::now();
            if (CheckReportRate(last_report, report_rate)) {
                reporter();
            }
        }
    };

    class ReadOperation {
    public:
        ReadOperation(FixedBuffer& buffer, Bytes size)
        : data(malloc(size.count()))
        , buffer(buffer)
        , size(size) {
        }

        ~ReadOperation() {
            free(data);
        }

        bool operator () (WRReporter& logger, Bytes offset) const {
            const auto to_read = std::min(size, Bytes(buffer.size()) - offset);
            if (to_read > Bytes{0} ) {
                memcpy(data, buffer.data() + offset.count(), size.count());
                logger(size);
            }
            return to_read >= size;
        }

        [[nodiscard]] auto BufferSize() const { return Bytes(buffer.size()); }
        [[nodiscard]] auto RWSize() const { return size; }

    private:
        void* data;
        const FixedBuffer& buffer;
        Bytes size;
    };

    class WriteOperation {
    public:
        WriteOperation(FixedBuffer& buffer, Bytes size)
        : data(malloc(size.count()))
        , buffer(buffer)
        , size(size) {
        }

        ~WriteOperation() {
            free(data);
        }

        bool operator () (WRReporter& logger, Bytes offset) const {
            const auto to_read = std::min(size, Bytes(buffer.size()) - offset);
            if (to_read > Bytes{0} ) {
                memcpy(buffer.data() + offset.count(), data, size.count());
                logger(size);
            }
            return to_read >= size;
        }

        [[nodiscard]] auto BufferSize() const { return Bytes(buffer.size()); }
        [[nodiscard]] auto RWSize() const { return size; }

    private:
        void* data;
        FixedBuffer& buffer;
        Bytes size;
    };
}

//==============================================================================
// Operations

namespace {

//------------------------------------------------------------------------------
// Fixed

    // It's possible config.times may be too high, either by user error or to
    // indicate we should read/write until the end of the buffer.
    template <typename ReadWrite>
    auto CheckTotalTimes(const ReadWrite& read_write, const FixedOpConfig& config) {
        const auto buffer_size = read_write.BufferSize();
        const auto rw_size = read_write.RWSize();

        const auto effective_buffer_size = buffer_size - config.initial_offset;
        const auto num_advances = (effective_buffer_size / config.advance).count();
        const auto max_ops = static_cast<decltype(config.times)>(
                (effective_buffer_size % num_advances) > rw_size
                ? num_advances + 1
                : num_advances);
        return std::min(max_ops, config.times);
    }

    // Helpers: The first X threads may include an extra op if things don't
    // divide evenly.
    auto DetermineLocalTimes(int cpu, int thread_count, int op_count) {
        const auto num_oversized_threads = op_count % thread_count;
        return op_count / thread_count + (cpu < num_oversized_threads ? 1 : 0);
    }

    auto TimesBeforeThread(int cpu, int thread_count, int op_count) {
        int ops = 0;
        for (int i = 0 ; i < cpu ; ++i) {
            ops += DetermineLocalTimes(i, thread_count, op_count);
        }
        return ops;
    }


    template <typename ReporterCreator, typename ReadWrite>
    void RunEvenDivision(ReporterCreator make_reporter, int cpu, int thread_count,
                         ReadWrite& read_write, const BaseOperation& op,
                         const FixedOpConfig& config) {
        const auto total_times = CheckTotalTimes(read_write, config);
        const auto local_times = DetermineLocalTimes(cpu, thread_count, total_times);

        auto reporter = make_reporter(cpu);
        int count = 0;
        auto offset = config.initial_offset +
                TimesBeforeThread(cpu, thread_count, total_times) * config.advance;
        while ( !op.IsStopped() && count++ < local_times ) {
            if (!read_write(reporter, offset)) {
                FatalError(TAG, "We expected to perform %d operations, but we "
                                "only ran %d", local_times, count);
            }
            offset += config.advance;
        }
    }

    //--------------

    template <typename ReporterCreator, typename ReadWrite>
    void RunInterleaved(ReporterCreator make_reporter, int cpu, int thread_count,
                        ReadWrite& read_write, const BaseOperation& op,
                        const FixedOpConfig& config) {
        auto reporter = make_reporter(cpu);
        auto offset = config.initial_offset + config.advance * cpu;
        while ( !op.IsStopped() && read_write(reporter, offset) ) {
            offset += config.advance * thread_count;
        }
    }

    //==============

    template <typename ReporterCreator, typename ReadWrite>
    void Run(const BaseOperation& op, ReporterCreator make_reporter,
             ReadWrite& read_write, const FixedOpConfig& config) {
        if (config.threads == ThreadSetup::kOneCore) {
            auto reporter = make_reporter(0);
            int count = 0;
            auto offset = config.initial_offset;
            while (!op.IsStopped() && count++ < config.times &&
                   read_write(reporter, offset) ) {
                offset += config.advance;
            }
        } else {
            const ThreadAffinity affinity = ToAffinity(config.threads);
            std::vector<std::thread> threads;

            const auto num_cores = NumCores(affinity);
            threads.reserve(num_cores);

            for ( int cpu = 0 ; cpu < num_cores ; ++cpu ) {
                threads.push_back(std::thread{[&op, &read_write, make_reporter,
                                               cpu, affinity, num_cores, config] {
                    SetThreadAffinity(cpu, affinity);
                    switch (config.work_scheme) {
                        case WorkScheme::kDividedEvenly:
                            return RunEvenDivision(make_reporter, cpu, num_cores,
                                                   read_write, op, config);
                        case WorkScheme::kInterleaved:
                            return RunInterleaved(make_reporter, cpu, num_cores,
                                                  read_write, op, config);
                        default:
                            FatalError(TAG, "Unknown work scheme: %d",
                                       config.work_scheme);
                    }
                }});
            }

            for (auto& thread : threads) {
                thread.join();
            }
        }
    }

//------------------------------------------------------------------------------
// Irregular

    template <typename ReporterCreator, typename ReadWrite>
    void Run(const BaseOperation& op, ReporterCreator make_reporter,
             ReadWrite& read_write, const IrregularOpConfig& config) {
        auto reporter = make_reporter(0);
        std::mt19937 random_generator{};
        const auto range = config.max_advance - config.min_advance;

        int count = 0;
        Bytes offset = 0;

        while (count++ < config.times && !op.IsStopped()) {
            const auto rand = Bytes(random_generator());
            offset += NextAlignedValue((rand % range) + config.min_advance,
                                       config.rw_align);
            // Irregular loops if we hit the end of the buffer.
            while (offset >= read_write.BufferSize()) offset -= read_write.BufferSize();
            // TODO(tmillican@google.com): Not technically safe/correct, but if
            //  we actually run into the bug then our buffer and align/rw_size
            //  are absurdly close to one another.
            offset = NextAlignedValue(offset, config.rw_align);

            read_write(reporter, offset);
        }
    }

//------------------------------------------------------------------------------
// Random

    template <typename ReporterCreator, typename ReadWrite>
    void Run(const BaseOperation& op, ReporterCreator make_reporter,
             ReadWrite read_write, const RandomOpConfig& config) {
        auto reporter = make_reporter(0);
        std::mt19937 random_generator{};

        for ( int count = 0 ; count < config.times && !op.IsStopped() ; ++count ) {
            const auto rand = Bytes(random_generator());
            auto offset = NextAlignedValue(rand % read_write.BufferSize(),
                                           config.rw_align);
            // We might have landed / aligned too close to the end.
            // TODO(tmillican@google.com): This solution makes the last bit of
            //  the buffer a 'hotspot', but in practice it shouldn't matter
            //  unless the read/align are absurdly close to the buffer's size.
            while (offset + read_write.RWSize() > read_write.BufferSize()) {
                offset -= config.rw_align;
            }

            read_write(reporter, offset);
        }
    }

//------------------------------------------------------------------------------
// Greedy

    // Hands out offsets to threads on a first-come-first-served basis.
    struct GreedyDispenser {
        Bytes advance;
        std::atomic_int segment = 0;

        auto operator () () {
            return segment++ * advance;
        }
    };

    template <typename ReporterCreator, typename ReadWrite>
    void Run(const BaseOperation& op, ReporterCreator make_reporter,
             ReadWrite read_write, const GreedyOpConfig& config) {
        const auto affinity = ToAffinity(config.threads);

        std::vector<std::thread> threads;
        const auto thread_count = config.threads != ThreadSetup::kOneCore ?
                NumCores(affinity) : 1;
        threads.reserve(thread_count);

        GreedyDispenser greedy_dispenser{config.advance};
        for ( int cpu = 0 ; cpu < thread_count ; ++cpu ) {
            threads.push_back(std::thread{[&op, &greedy_dispenser, make_reporter,
                                           cpu, affinity, config, read_write] {
                SetThreadAffinity(cpu, affinity);
                auto reporter = make_reporter(cpu);
                auto offset = greedy_dispenser();
                while (!op.IsStopped() && read_write(reporter, offset)) {
                    offset = greedy_dispenser();
                }
            }});
        }

        for (auto& thread : threads) {
            thread.join();
        }
    }

//------------------------------------------------------------------------------

    // Helper to handle the common read/write determination logic.
    template <typename ReporterCreator, typename ConfigType>
    void Run(const BaseOperation& op, ReporterCreator make_reporter, ReadOrWrite read_write,
             FixedBuffer& buffer, Bytes rw_size, const ConfigType& config) {
        switch (read_write) {
            case ReadOrWrite::kRead: {
                ReadOperation read{buffer, rw_size};
                return Run(op, make_reporter, read, config);
            }
            case ReadOrWrite::kWrite: {
                ReadOperation write{buffer, rw_size};
                return Run(op, make_reporter, write, config);
            }
            default:
                FatalError(TAG, "Unknown read/write value: %d", read_write);
        }
    }
}

//==============================================================================

class MemoryAccessOperation : public BaseOperation {
public:
    MemoryAccessOperation() = default;

    void Start() override {
        _config = GetConfiguration<Configuration>();

        _thread = std::thread{[this] {
            FixedBuffer buffer(_config.buffer_size.count());

            int pass_id = 0;
            for (const auto& op : _config.operations) {
                Log::D(TAG, "%s %s", kBufferOpTypeNames[(size_t)op.operation_type],
                       kReadWriteNames[(size_t)op.read_write]);
                Report(Json{{"event", "New Pass"}, {"description", op.description},
                            {"append_graph", op.append_graph}});

                const auto ReporterCreator = [this, pass_id] (int thread_id) {
                    return WRReporter{ Reporter<Datum>{*this, pass_id, thread_id},
                                     _config.report_rate};
                };

                switch (op.operation_type) {
                    case BufferOpType::kFixedOp:
                        Run(*this, ReporterCreator, op.read_write, buffer,
                            op.rw_size, std::get<FixedOpConfig>(op.config));
                        break;
                    case BufferOpType::kIrregularOp:
                        Run(*this, ReporterCreator, op.read_write, buffer,
                            op.rw_size, std::get<IrregularOpConfig>(op.config));
                        break;
                    case BufferOpType::kRandomOp:
                        Run(*this, ReporterCreator, op.read_write, buffer,
                            op.rw_size, std::get<RandomOpConfig>(op.config));
                        break;
                    case BufferOpType::kGreedyOp:
                        Run(*this, ReporterCreator, op.read_write, buffer,
                            op.rw_size, std::get<GreedyOpConfig>(op.config));
                        break;
                    default:
                        FatalError(TAG, "Unknown buffer operation: %d", op.operation_type);
                }
                ++pass_id;
            }
        }};
    }

    void Wait() override {
        _thread.join();
    }

private:
    Configuration _config;
    std::thread _thread;
};


EXPORT_ANCER_OPERATION(MemoryAccessOperation)
