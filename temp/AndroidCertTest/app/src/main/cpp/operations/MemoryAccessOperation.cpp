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
#include <limits>
#include <random>
#include <thread>
#include <variant>

#include <ancer/BaseOperation.hpp>
#include <ancer/System.hpp>
#include <ancer/util/Basics.hpp>
#include <ancer/util/Bitmath.hpp>
#include <ancer/util/Json.hpp>

using namespace ancer;
using Bytes = bitmath::Bytes;


//==============================================================================

namespace {
    constexpr Log::Tag TAG{"MemoryAccessOperation"};

    using FixedBuffer = std::vector<char>;
}

//==============================================================================

namespace {
    constexpr Bytes kMaxReadWrite = 2048;

//------------------------------------------------------------------------------

    enum class ThreadSetup {
        kOneCore, kAllCores, kBigCores, kLittleCores, kMiddleCores
    };
    constexpr const char* const kThreadSetupNames[] = {
            "Single Core", "All Cores", "Big Cores", "Little Cores", "MiddleCores"
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
        ReadOrWrite read_write;
        Bytes rw_size;
        BufferOpType operation_type;
        std::variant<FixedOpConfig, IrregularOpConfig,
                     RandomOpConfig, GreedyOpConfig> config;
    };

    JSON_READER(BufferOpConfig) {
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

        if (data.rw_size > kMaxReadWrite) {
            FatalError(TAG, "rw_size(%zu) is larger than the maximum supported (%zu)",
                       data.rw_size, kMaxReadWrite);
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
        // We log cumulative instead of since-last-datum so we can use the
        // reset-to-zero point to mark where phases start without anything
        // fancy.
        Bytes cumulative_bytes;
    };

    JSON_WRITER(Datum) {
        JSON_REQVAR(cumulative_bytes);
    }
}

//==============================================================================
// Read/Write

namespace {
    // TODO(tmillican@google.com): Similar to I/O logging, and it's a bit of an
    //  ugly setup. Probably worth investing a bit of time for an "official"
    //  helper for this pattern.
    struct RWLogger {
        Reporter& reporter;
        Milliseconds report_rate;
        Bytes rw_bytes = 0;
        Timestamp last_report{};

        ~RWLogger() {
            if (rw_bytes != Bytes{0}) {
                reporter(Datum{rw_bytes});
            }
        }

        void operator () (Bytes new_bytes) {
            rw_bytes += new_bytes;
            const auto now = SteadyClock::now();
            if (last_report + report_rate < now) {
                reporter(Datum{rw_bytes});
                last_report = now;
            }
        }
    };

    struct ReadOperation {
        RWLogger logger;
        const FixedBuffer& buffer;
        Bytes size;

        bool operator () (Bytes offset) {
            if ((offset + size).count() <= buffer.size()) {
                char data[kMaxReadWrite.count()];
                memcpy(data, buffer.data() + offset.count(), size.count());
                logger(size);
                return true;
            } else {
                return false;
            }
        }

        [[nodiscard]] auto BufferSize() const { return Bytes(buffer.size()); }
    };

    struct WriteOperation {
        RWLogger logger;
        FixedBuffer& buffer;
        Bytes size;

        bool operator () (Bytes offset) {
            if ((offset + size).count() <= buffer.size()) {
                char data[kMaxReadWrite.count()]; // Just writing garbage is fine.
                memcpy(buffer.data() + offset.count(), data, size.count());
                logger(size);
                return true;
            } else {
                return false;
            }
        }

        [[nodiscard]] auto BufferSize() const { return Bytes(buffer.size()); }
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
        const auto rw_size = read_write.size;

        const auto effective_buffer_size = buffer_size - config.initial_offset;
        const auto num_advances = (effective_buffer_size / config.advance).count();
        const auto max_ops =
                (effective_buffer_size % num_advances) > rw_size
                ? num_advances + 1
                : num_advances;
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


    template <typename ReadWrite>
    void RunEvenDivision(int cpu, int thread_count, ReadWrite& read_write,
                         const BaseOperation& op, const FixedOpConfig& config) {
        const auto total_times = CheckTotalTimes(read_write, config);
        const auto local_times = DetermineLocalTimes(cpu, thread_count, total_times);

        int count = 0;
        auto offset = config.initial_offset +
                TimesBeforeThread(cpu, thread_count, total_times) * config.advance;
        while ( !op.IsStopped() && count < local_times && read_write(offset) ) {
            offset += config.advance;
        }
    }

    //--------------

    template <typename ReadWrite>
    void RunInterleaved(int cpu, int thread_count, ReadWrite& read_write,
                        const BaseOperation& op, const FixedOpConfig& config) {
        auto offset = config.initial_offset + config.advance * cpu;
        while ( !op.IsStopped() && read_write(offset) ) {
            offset += config.advance * thread_count;
        }
    }

    //==============

    template <typename ReadWrite>
    void Run(ReadWrite& read_write, const BaseOperation& op,
             const FixedOpConfig& config) {
        if (config.threads == ThreadSetup::kOneCore) {
            int count = 0;
            auto offset = config.initial_offset;
            while (!op.IsStopped() && count++ < config.times && read_write(offset) ) {
                offset += config.advance;
            }
        } else {
            const ThreadAffinity affinity = ToAffinity(config.threads);
            std::vector<std::thread> threads;

            const auto num_cores = NumCores(affinity);
            threads.reserve(num_cores);

            for ( int cpu = 0 ; cpu < num_cores ; ++cpu ) {
                threads.push_back(std::thread{[cpu, num_cores, config,
                                               &read_write, &op] {
                    SetThreadAffinity(cpu);

                    switch (config.work_scheme) {
                        case WorkScheme::kDividedEvenly:
                            return RunEvenDivision(cpu, num_cores, read_write, op, config);
                        case WorkScheme::kInterleaved:
                            return RunInterleaved(cpu, num_cores, read_write, op, config);
                        default:
                            FatalError(TAG, "Unknown work scheme: %d", config.work_scheme);
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

    template <typename ReadWrite>
    void Run(ReadWrite& read_write, const BaseOperation& op,
             const IrregularOpConfig& config) {
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

            read_write(offset);
        }
    }

//------------------------------------------------------------------------------
// Random

    template <typename ReadWrite>
    void Run(ReadWrite& read_write, const BaseOperation& op,
             const RandomOpConfig& config) {
        std::mt19937 random_generator{};

        for ( int count = 0 ; count < config.times && !op.IsStopped() ; ++count ) {
            const auto rand = Bytes(random_generator());
            auto offset = NextAlignedValue(rand % read_write.BufferSize(),
                                           config.rw_align);
            // We might have landed / aligned too close to the end.
            // TODO(tmillican@google.com): This solution makes the last bit of
            //  the buffer a 'hotspot', but in practice it shouldn't matter
            //  unless the read/align are absurdly close to the buffer's size.
            while (offset + read_write.size > read_write.BufferSize()) {
                offset -= config.rw_align;
            }

            read_write(offset);
        }
    }

//------------------------------------------------------------------------------
// Greedy

    // Hands out offsets to threads on a first-come-first-served basis.
    struct GreedyDispenser {
        Bytes advance;
        std::mutex mutex;
        Bytes next_offset = 0;

        auto operator () () {
            std::scoped_lock lock{mutex};
            auto ret = next_offset;
            next_offset += advance;
            return ret;
        }
    };

    template <typename ReadWrite>
    void Run(ReadWrite&& read_write, const BaseOperation& op,
             const GreedyOpConfig& config) {
        const auto affinity = ToAffinity(config.threads);

        std::vector<std::thread> threads;
        const auto thread_count = config.threads != ThreadSetup::kOneCore ?
                NumCores(affinity) : 1;
        threads.reserve(thread_count);

        GreedyDispenser greedy_dispenser{config.advance};
        for ( int cpu = 0 ; cpu < thread_count ; ++cpu ) {
            threads.push_back(std::thread{[cpu, affinity, config,
                                           &read_write, &op, &greedy_dispenser] {
                SetThreadAffinity(cpu, affinity);
                auto offset = greedy_dispenser();
                while (!op.IsStopped() && read_write(offset)) {
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
    template <typename ConfigType>
    void Run(RWLogger&& logger, ReadOrWrite read_write, FixedBuffer& buffer,
             Bytes rw_size, const BaseOperation& op, const ConfigType& config) {
        switch (read_write) {
            case ReadOrWrite::kRead: {
                ReadOperation read{std::move(logger), buffer, rw_size};
                return Run(read, op, config);
            }
            case ReadOrWrite::kWrite: {
                ReadOperation write{std::move(logger), buffer, rw_size};
                return Run(write, op, config);
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
            FixedBuffer buffer;
            buffer.resize(_config.buffer_size.count());

            for (const auto& op : _config.operations) {
                Log::D(TAG, "%s %s", kBufferOpTypeNames[(size_t)op.operation_type],
                       kReadWriteNames[(size_t)op.read_write]);
                Reporter reporter{*this};
                RWLogger logger{reporter, _config.report_rate};

                switch (op.operation_type) {
                    case BufferOpType::kFixedOp:
                        Run(std::move(logger), op.read_write, buffer, op.rw_size,
                            *this, std::get<FixedOpConfig>(op.config));
                        break;
                    case BufferOpType::kIrregularOp:
                        Run(std::move(logger), op.read_write, buffer, op.rw_size,
                            *this, std::get<IrregularOpConfig>(op.config));
                        break;
                    case BufferOpType::kRandomOp:
                        Run(std::move(logger), op.read_write, buffer, op.rw_size,
                            *this, std::get<RandomOpConfig>(op.config));
                        break;
                    case BufferOpType::kGreedyOp:
                        Run(std::move(logger), op.read_write, buffer, op.rw_size,
                            *this, std::get<GreedyOpConfig>(op.config));
                        break;
                    default:
                        FatalError(TAG, "Unknown buffer operation: %d", op.operation_type);
                }
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
