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

#include <cpu-features.h>

#include <ancer/BaseOperation.hpp>
#include <ancer/System.hpp>
#include <ancer/util/Json.hpp>

using namespace ancer;

//==============================================================================

namespace {
    constexpr Log::Tag TAG{"MemoryAccessOperation"};

    // TODO(tmillican@google.com): Probably belongs in a utility header somewhere.
    template<typename T>
    [[nodiscard]] constexpr auto NextAlignedValue(T value, T align) {
        const auto offset = align ? value % align : 0;
        return value + (offset ? align - offset : 0);
    }

    using FixedBuffer = std::vector<char>;
}

//==============================================================================

namespace {
    enum class OpType { kRead, kWrite };
    enum class ThreadSetup {
        kOneCore, kAllCores, kBigCores, kLittleCores
    };
    enum class WorkScheme {
        kDividedEvenly, kInterleaved
    };

    constexpr size_t kMaxReadWrite = 2048;

    // Performs operations that have a regular pattern.
    struct FixedOpConfig {
        OpType type;
        ThreadSetup threads;
        WorkScheme work_scheme;

        // How many read/writes to perform. Will automatically stop at the end
        // of the buffer for safety.
        int times = std::numeric_limits<int>::max();
        size_t size;    // How much to read/write at once.
        size_t offset;  // How far off to start the first read/write.
        size_t advance; // Advance by this much per read/write.
    };

    // Performs operations while advancing irregularly through the buffer.
    // Single-threaded only to avoid introducing too much chaos.
    struct IrregularOpConfig {
        OpType type;

        // Note: If we reach the end of the buffer, we'll loop if necessary.
        int times;
        size_t size;
        size_t align;
        size_t min_advance;
        size_t max_advance;
    };

    // Performs operations at random points within the buffer.
    // Single-threaded only to avoid introducing too much chaos.
    struct RandomOpConfig {
        OpType type;

        int times;
        size_t size;
        size_t align;
    };

    //--------------

    // Threads gobble up the data in-order as quickly as they can.
    // Multithreaded only since it doesn't make much sense otherwise.
    struct GreedyOpConfig {
        OpType type;

        size_t size;
        // Helper: Based on user-defined advance, hands out offsets to threads.
        struct {
            size_t advance;

            std::mutex mutex;
            size_t next_offset = 0;

            auto operator () () {
                std::scoped_lock lock{mutex};
                auto ret = next_offset;
                next_offset += advance;
                return ret;
            }
        } dispenser;
    };

//------------------------------------------------------------------------------

    enum BufferOpType { kFixedOp, kIrregularOp, kRandomOp, kGreedyOp };
    struct BufferOpConfig {
        // Bleh, keep it JSON-friendly
        BufferOpType type;
        std::variant<FixedOpConfig, IrregularOpConfig, RandomOpConfig, GreedyOpConfig> config;
    };

    struct Configuration {
        size_t buffer_size;
        std::vector<BufferOpConfig> operations;
    };

    JSON_READER(Configuration) {
        // STUB
        // assert max read/write
        // assert reads * advance doesn't overflow our data
    }

//------------------------------------------------------------------------------

    struct Datum {
    };

    JSON_WRITER(Datum) {
        // STUB
    }
}

//==============================================================================
// Read/Write

namespace {
    struct ReadOperation {
        void operator () (const FixedBuffer& src, size_t offset, size_t size) const {
            char buffer[kMaxReadWrite];
            memcpy(buffer, src.data() + offset, size);
        }
    };

    struct WriteOperation {
        void operator () (FixedBuffer& dst, size_t offset, size_t size) const {
            char buffer[kMaxReadWrite];
            memcpy(dst.data() + offset, buffer, size);
        }
    };
}

//==============================================================================
// Operations

namespace {

//------------------------------------------------------------------------------
// Fixed

    // Helpers: The first X threads may include an extra op if things don't
    // divide evenly.
    auto OperationsInThread(int cpu, int thread_count, int op_count) {
        const auto oversized_threads = op_count % thread_count;
        return op_count / thread_count + (cpu < oversized_threads ? 1 : 0);
    }

    auto OperationsBeforeThread(int cpu, int thread_count, int op_count) {
        size_t ops = 0;
        for (int i = 0 ; i < cpu ; ++i) {
            ops += OperationsInThread(i, thread_count, op_count);
        }
        return ops;
    }


    template <typename ReadWrite>
    void RunEvenDivision(int cpu, int thread_count, ReadWrite&& read_write,
                         FixedBuffer& buffer, const FixedOpConfig& config) {
        const auto num_ops = OperationsInThread(cpu, thread_count, config.times);
        auto offset = config.offset +
                OperationsBeforeThread(cpu, thread_count, config.times) * config.advance;
        for ( int i = 0 ; i < num_ops ; ++i ) {
            assert(offset + config.size <= buffer.size());
            read_write(buffer, offset, config.size);
            offset += config.advance;
        }
    }

    //--------------

    template <typename ReadWrite>
    void RunInterleaved(int cpu, int thread_count, ReadWrite&& read_write,
                        FixedBuffer& buffer, const FixedOpConfig& config) {
        auto offset = config.offset + cpu * config.advance;
        while ( offset < buffer.size() ) {
            read_write(buffer, offset, config.size);
            offset += config.advance * thread_count;
        }
    }

    //==============

    template <typename ReadWrite>
    void Run(ReadWrite&& read_write, FixedBuffer& buffer, const FixedOpConfig& config) {
        assert(config.threads == ThreadSetup::kOneCore);

        if (config.threads == ThreadSetup::kOneCore) {
            int count = 0;
            size_t offset = config.offset;
            while (count < config.times && offset < buffer.size()) {
                read_write(buffer, offset, config.size);
                ++count;
                offset += config.advance;
            }
        } else {
            if (config.threads != ThreadSetup::kAllCores) {
                FatalError(TAG, "BigCore & LittleCore setups aren't currently supported.");
            }
            std::vector<std::thread> threads;
            const auto thread_count = android_getCpuCount();
            threads.reserve(thread_count);

            for ( int cpu = 0 ; cpu < thread_count ; ++cpu ) {
                threads.push_back(std::thread{[cpu, thread_count, read_write,
                                               config, &buffer] {
                    cpu_set_t affinity;
                    CPU_ZERO(&affinity);
                    CPU_SET(cpu, &affinity);
                    sched_setaffinity(0, sizeof(affinity), &affinity);

                    switch (config.work_scheme) {
                        case WorkScheme::kDividedEvenly:
                            return RunEvenDivision(cpu, thread_count, read_write, buffer, config);
                        case WorkScheme::kInterleaved:
                            return RunInterleaved(cpu, thread_count, read_write, buffer, config);
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
    void Run(ReadWrite&& read_write, FixedBuffer& buffer, const IrregularOpConfig& config) {
        std::mt19937 random{};
        const auto range = config.max_advance - config.min_advance;

        int count = 0;
        size_t offset = 0;

        while (count < config.times) {
            offset += NextAlignedValue((random() % range) + config.min_advance, config.align);
            while (offset >= buffer.size()) offset -= buffer.size();
            read_write(buffer, offset, config.size);
            ++count;
        }
    }

//------------------------------------------------------------------------------
// Random

    template <typename ReadWrite>
    void Run(ReadWrite&& read_write, FixedBuffer& buffer, const RandomOpConfig& config) {
        std::mt19937 random{};

        for (int count = 0 ; count < config.times ; ++count) {
            auto offset = NextAlignedValue(random() % buffer.size(), config.align);
            if (offset >= buffer.size()) offset -= buffer.size(); // Might align to/past end.
            read_write(buffer, offset, config.size);
        }
    }

//------------------------------------------------------------------------------
// Greedy

    template <typename ReadWrite>
    void Run(ReadWrite&& read_write, FixedBuffer& buffer, GreedyOpConfig& config) {
        for ( auto offset = config.dispenser() ; offset < buffer.size() ;
              offset = config.dispenser() ) {
            read_write(buffer, offset, config.size);
        }
    }

//------------------------------------------------------------------------------

    // Helper to handle the common read/write determination logic.
    template <typename ConfigType>
    void Run(FixedBuffer& buffer, ConfigType&& config) {
        switch (config.type) {
            case OpType::kRead:
                return Run(ReadOperation{}, buffer, std::forward<ConfigType>(config));
            case OpType::kWrite:
                return Run(WriteOperation{}, buffer, std::forward<ConfigType>(config));
            default:
                FatalError(TAG, "Unknown type: %d", config.type);
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
            buffer.resize(_config.buffer_size);

            for (auto& op : _config.operations) {
                switch (op.type) {
                    case BufferOpType::kFixedOp:
                        Run(buffer, std::get<FixedOpConfig>(op.config)); break;
                    case BufferOpType::kIrregularOp:
                        Run(buffer, std::get<IrregularOpConfig>(op.config)); break;
                    case BufferOpType::kRandomOp:
                        Run(buffer, std::get<RandomOpConfig>(op.config)); break;
                    case BufferOpType::kGreedyOp:
                        Run(buffer, std::get<GreedyOpConfig>(op.config)); break;
                    default:
                        FatalError(TAG, "Unknown buffer operation: %d", op.type);
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
