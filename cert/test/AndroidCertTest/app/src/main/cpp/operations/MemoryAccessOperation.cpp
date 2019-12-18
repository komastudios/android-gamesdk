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

#include <ancer/BaseOperation.hpp>
#include <ancer/DatumReporting.hpp>
#include <ancer/Reporter.hpp>
#include <ancer/System.hpp>
#include <ancer/util/Basics.hpp>
#include <ancer/util/Bitmath.hpp>
#include <ancer/util/Json.hpp>
#include <ancer/util/ThreadSyncPoint.hpp>

using namespace ancer;
using Bytes = bitmath::Bytes;


//==============================================================================

namespace {
    constexpr Log::Tag TAG{"MemoryAccessOperation"};
}

//==============================================================================

namespace {
    // We use this instead of something like std::vector to avoid actually
    // touching the memory unless explicitly requested.
    class FixedBuffer {
    public:
        FixedBuffer(size_t size, bool prewrite)
        : _size(size)
        , _buffer((char*)malloc(size)) {
            if (prewrite) {
                RandomizeMemory(_buffer, _size);
            }
        }
        FixedBuffer(const FixedBuffer&) = delete;
        ~FixedBuffer() {
            free(_buffer);
        }

        // TODO(tmillican@google.com): Switch to bytes
        [[nodiscard]] auto size() const noexcept { return _size; }
        [[nodiscard]] auto data() const noexcept { return _buffer; }
    private:
        size_t _size;
        char* _buffer;
    };
}

//==============================================================================

namespace {
    enum class WorkScheme { kDividedEvenly, kInterleaved };
    constexpr std::array kWorkSchemeNames = {"Divided", "Interleaved"};


    // Performs operations that have a regular pattern.
    struct FixedOpConfig {
        WorkScheme scheme;
        Bytes advance; // Advance by this much per read/write.
        Bytes initial_offset = 0; // How far off to start the first read/write.
    };

    JSON_READER(FixedOpConfig) {
        JSON_OPTENUM(scheme, kWorkSchemeNames);
        JSON_REQVAR(advance);
        JSON_OPTVAR(initial_offset);
    }

    void Verify(const FixedOpConfig& cfg, Bytes wr_sz, Bytes buf_sz) {
        if (buf_sz % cfg.advance != 0) {
            FatalError(TAG, "Advance (%d) doesn't work with buffer size (%d).",
                       cfg.advance, buf_sz.count());
        }
        if (cfg.initial_offset % cfg.advance != 0) {
            FatalError(TAG, "Advance (%d) doesn't go evenly into initial offset (%d).",
                       cfg.advance, cfg.initial_offset);
        }
    }

    //--------------

    // Performs operations while advancing irregularly through the buffer.
    // Note: Single-threaded only, largely untested.
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

    void Verify(const IrregularOpConfig& cfg, Bytes wr_sz, Bytes buf_sz) {
        if (cfg.rw_align % wr_sz != 0 || buf_sz % cfg.rw_align != 0) {
            FatalError(TAG, "Alignment (%d) doesn't work with op/buf size (%d/%d).",
                       cfg.rw_align.count(), wr_sz.count(), buf_sz.count());
        }
    }

    //--------------

    // Performs operations at random points within the buffer.
    // Note: Single-threaded only, largely untested.
    struct RandomOpConfig {
        size_t times;
        Bytes rw_align;
    };

    JSON_READER(RandomOpConfig) {
        JSON_REQVAR(times);
        JSON_REQVAR(rw_align);
    }

    void Verify(const RandomOpConfig& cfg, Bytes wr_sz, Bytes buf_sz) {
        if (cfg.rw_align % wr_sz != 0 || cfg.rw_align % buf_sz != 0) {
            FatalError(TAG, "Alignment (%d) doesn't work with op/buf size (%d/%d).",
                       cfg.rw_align.count(), wr_sz.count(), buf_sz.count());
        }
    }

//------------------------------------------------------------------------------

    enum class ReadOrWrite { kRead, kWrite };
    constexpr std::array kReadWriteNames = { "Read", "Write" };

    enum TouchSetup { kChar, kShort, kInt, kLong, kLongLong, kBuffer };
    constexpr std::array kTouchSetupNames = {
        "char", "short", "int", "long", "long long", "buffer"
    };
    constexpr std::array kTouchSizes = {
        sizeof(char), sizeof(short), sizeof(int), sizeof(long), sizeof(long long)
    };

    enum class BufferOpType { kFixedOp, kIrregularOp, kRandomOp };
    constexpr std::array kBufferOpTypeNames = { "Fixed", "Irregular", "Random" };


    struct CoreSetup {
        // Which cores we should work on. For mixed-affinity tests, threads will
        // always be locked to the proper subset of cores even if fixed_cores is
        // disabled.
        ThreadAffinity affinity = ThreadAffinity::kAll;
        // How many cores to use, 0 for as many as possible
        int max_affinity_cores = 0;
        // If big/medium/little counts are different, use the smaller.
        bool match_cores = false;
        // If threads should be locked to a specific core.
        bool fixed_cores = true;
    };

    JSON_READER(CoreSetup) {
        JSON_OPTENUM(affinity, kAffinityNames);
        JSON_OPTVAR(max_affinity_cores);
        JSON_OPTVAR(match_cores);
        JSON_OPTVAR(fixed_cores);
    }


    struct BufferOpConfig {
        ReadOrWrite read_write;
        CoreSetup core_setup;
        TouchSetup touch;
        Bytes touch_size;
        // How many times to run this operation.
        int run_count = 1;
        // If this is a write, randomize before each write.
        // If this is a read, randomize the buffer being read to after each read.
        bool always_randomize = false;
        BufferOpType operation_type;
        std::variant<FixedOpConfig, IrregularOpConfig, RandomOpConfig> config;
    };

    JSON_READER(BufferOpConfig) {
        JSON_REQENUM(read_write, kReadWriteNames);
        JSON_OPTVAR(core_setup);
        JSON_REQENUM(touch, kTouchSetupNames);
        if (data.touch == TouchSetup::kBuffer) {
            JSON_REQVAR(touch_size);
        } else {
            data.touch_size = kTouchSizes[(int)data.touch];
        }
        JSON_OPTVAR(run_count);
        JSON_OPTVAR(always_randomize);
        JSON_REQENUM(operation_type, kBufferOpTypeNames);
        switch(data.operation_type) {
            case BufferOpType::kFixedOp:
                JSON_REQVAR_AT("type_config", data.config.template emplace<FixedOpConfig>());
                break;
            case BufferOpType::kIrregularOp:
                JSON_REQVAR_AT("type_config", data.config.template emplace<IrregularOpConfig>());
                break;
            case BufferOpType::kRandomOp:
                JSON_REQVAR_AT("type_config", data.config.template emplace<RandomOpConfig>());
                break;
            default:
                FatalError(TAG, "Unknown operation type %d", data.operation_type);
        }
    }

    //--------------

    struct Configuration {
        Bytes buffer_size;
        bool prewrite_buffer = false;
        // Report every X operations
        unsigned report_rate = 100;
        std::vector<BufferOpConfig> operations;
    };

    JSON_READER(Configuration) {
        JSON_REQVAR(buffer_size);
        JSON_OPTVAR(prewrite_buffer);
        JSON_OPTVAR(report_rate);
        JSON_REQVAR(operations);

        for (auto& op : data.operations) {
            // Force a good size/alignment for now to keep this simple.
            if (data.buffer_size % op.touch_size != 0) {
                FatalError(TAG, "Buffer/touch size mismatch.");
            }
            std::visit([&data, &op] (const auto& cfg) {
                Verify(cfg, op.touch_size, data.buffer_size);
            }, op.config);
        }
    }

//------------------------------------------------------------------------------

    struct Datum {
        int pass_id;
        int thread_id;
        Duration time = Duration::zero();
        Bytes bytes = {0};
    };

    void WriteDatum(report_writers::Struct w, const Datum& d) {
        ADD_DATUM_MEMBER(w, d, pass_id);
        ADD_DATUM_MEMBER(w, d,thread_id);
        ADD_DATUM_MEMBER(w, d, time);
        ADD_DATUM_MEMBER(w, d, bytes);
    }
}

//==============================================================================
// Read/Write

namespace {
    // Handles updating and reporting our datum as we read/write.
    struct WRReporter {
        Reporter<Datum> reporter;
        // To keep branching/work to a minimum, we use an unsigned rate/count
        // (rollover is safe) and do a modulo to see if we should report. This
        // can cause the report around the rollover to come too soon if we run
        // that many operations, but that's barely even noticeable.
        unsigned report_rate;
        unsigned report_ct = 0;
        Timestamp last_begin;

        ~WRReporter() {
            reporter();
        }

        // Manual pump to give us a zero point.
        // Not in the constructor so we can call this immediately after the
        // thread sync.
        void BeginReporting() {
            reporter();
        }


        // TODO(tmillican@google.com): Need to guarantee begin/end aren't
        //  reordered by the optimizer.
        void BeginOp() {
            last_begin = SteadyClock::now();
        }

        void EndOp(Bytes new_bytes) {
            const auto now = SteadyClock::now();
            reporter.datum.time += now - last_begin;
            reporter.datum.bytes += new_bytes;
            if ((++report_ct % report_rate) == 0) {
                reporter();
            }
        }
    };

//------------------------------------------------------------------------------

    template <typename T>
    class ReadOperation {
    public:
        ReadOperation(const FixedBuffer& buffer, const BufferOpConfig& cfg)
        : _buffer(buffer)
        , _randomize(cfg.always_randomize) {
            assert(cfg.touch_size.count() == sizeof(T));
        }

        [[nodiscard]] auto BufferSize() const { return Bytes(_buffer.size()); }
        [[nodiscard]] auto BufferLeft(Bytes offset) const { return BufferSize() - offset; }
        [[nodiscard]] constexpr auto RWSize() const { return Bytes(sizeof(T)); }

        // TODO(tmillican@google.com): Deal with duplication vs void* version.
        bool operator () (WRReporter& logger, Bytes offset) const {
            const auto to_read = std::min(RWSize(), BufferLeft(offset));
            if (to_read > Bytes{0}) {
                assert(to_read.count() == sizeof(T));
                logger.BeginOp();
                    T local_buf = *reinterpret_cast<T*>(_buffer.data() + offset.count());
                logger.EndOp(RWSize());
                if (_randomize) {
                    RandomizeMemory(&local_buf, RWSize());
                }
                ForceCompute(local_buf);
                return true;
            } else {
                return false;
            }
        }
    private:
        const FixedBuffer& _buffer;
        bool _randomize;
    };

    template <>
    class ReadOperation<void*> {
    public:
        ReadOperation(const FixedBuffer& buffer, const BufferOpConfig& cfg)
        : _buffer(buffer)
        , _local_buf(malloc(cfg.touch_size.count()))
        , _size(cfg.touch_size)
        , _randomize(cfg.always_randomize) {
        }

        ~ReadOperation() {
            free(_local_buf);
        }

        [[nodiscard]] auto BufferSize() const { return Bytes(_buffer.size()); }
        [[nodiscard]] auto BufferLeft(Bytes offset) const { return BufferSize() - offset; }
        [[nodiscard]] auto RWSize() const { return _size; }

        bool operator () (WRReporter& logger, Bytes offset) const {
            const auto to_read = std::min(RWSize(), BufferLeft(offset));
            if (to_read > Bytes{0}) {
                memcpy(_local_buf, _buffer.data() + offset.count(), to_read.count());
                if (_randomize) {
                    RandomizeMemory(_local_buf, RWSize());
                }
                logger.BeginOp();
                    ForceCompute(_local_buf);
                logger.EndOp(to_read);
                return true;
            } else {
                return false;
            }
        }
    private:
        const FixedBuffer& _buffer;
        void* _local_buf;
        Bytes _size;
        bool _randomize;
    };

//------------------------------------------------------------------------------

    template <typename T>
    class WriteOperation {
    public:
        WriteOperation(const FixedBuffer& buffer, const BufferOpConfig& cfg)
        : _buffer(buffer)
        , _randomize(cfg.always_randomize) {
            assert(cfg.touch_size.count() == sizeof(T));
            if (!_randomize) {
                RandomizeMemory(&_value, sizeof(T));
            }
        }

        [[nodiscard]] auto BufferSize() const { return Bytes(_buffer.size()); }
        [[nodiscard]] auto BufferLeft(Bytes offset) const { return BufferSize() - offset; }
        [[nodiscard]] constexpr auto RWSize() const { return Bytes(sizeof(T)); }

        // TODO(tmillican@google.com): Deal with duplication vs void* version.
        bool operator () (WRReporter& logger, Bytes offset) {
            const auto to_write = std::min(RWSize(), BufferLeft(offset));
            if (to_write > Bytes{0}) {
                assert (to_write.count() == sizeof(T));
                if (_randomize) {
                    RandomizeMemory(&_value, RWSize());
                }
                logger.BeginOp();
                    *reinterpret_cast<T*>(_buffer.data() + offset.count()) = _value;
                logger.EndOp(to_write);
                return true;
            } else {
                return false;
            }
        }
    private:
        const FixedBuffer& _buffer;
        T _value;
        bool _randomize;
    };


    template <>
    class WriteOperation<void*> {
    public:
        WriteOperation(FixedBuffer& buffer, const BufferOpConfig& cfg)
        : _buffer(buffer)
        , _local_buffer(malloc(cfg.touch_size.count()))
        , _size(cfg.touch_size)
        , _randomize(cfg.always_randomize) {
            if (!_randomize) {
                RandomizeMemory(_local_buffer, _size.count());
            }
        }

        ~WriteOperation() {
            free(_local_buffer);
        }

        [[nodiscard]] auto BufferSize() const { return Bytes(_buffer.size()); }
        [[nodiscard]] auto BufferLeft(Bytes offset) const { return BufferSize() - offset; }
        [[nodiscard]] auto RWSize() const { return _size; }

        bool operator () (WRReporter& logger, Bytes offset) const {
            const auto to_write = std::min(RWSize(), BufferLeft(offset));
            if (to_write > Bytes{0} ) {
                if (_randomize) {
                    RandomizeMemory(_local_buffer, _size);
                }
                logger.BeginOp();
                    memcpy(_buffer.data() + offset.count(), _local_buffer, to_write.count());
                logger.EndOp(to_write);
            }
            return to_write >= _size;
        }
    private:
        FixedBuffer& _buffer;
        Bytes _size;
        void* _local_buffer;
        bool _randomize;
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
        const auto effective_buffer_size = read_write.BufferSize() - config.initial_offset;
        return effective_buffer_size / config.advance;
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


    template <typename Reporter, typename ReadWrite>
    void RunEvenDivision(Reporter& reporter, ThreadSyncPoint& sync_point,
                         int cpu, int thread_count, ReadWrite& read_write,
                         const BaseOperation& op, const FixedOpConfig& config) {
        const auto total_times = CheckTotalTimes(read_write, config);
        const auto local_times = DetermineLocalTimes(cpu, thread_count, total_times);

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

    template <typename Reporter, typename ReadWrite>
    void RunInterleaved(Reporter& reporter, ThreadSyncPoint& sync_point,
                        int cpu, int thread_count, ReadWrite& read_write,
                        const BaseOperation& op, const FixedOpConfig& config) {
        auto offset = config.initial_offset + config.advance * cpu;
        while ( !op.IsStopped() && read_write(reporter, offset) ) {
            offset += config.advance * thread_count;
        }
    }

    //==============

    namespace {
        // Returns the minimum & maximum number of the active core types.
        auto CoreMinMax() {
            const auto core_min = [] {
                int min = std::numeric_limits<int>::max();
                for (int i = 0; i < (int)ThreadAffinity::kAffinityCount; ++i) {
                    const int ct = NumCores((ThreadAffinity)i);
                    if (ct != 0) {
                        min = std::min(min, ct);
                    }
                }
                return min;
            } ();
            const auto core_max = [] {
                int max = 0;
                for (int i = 0; i < (int)ThreadAffinity::kAffinityCount; ++i) {
                    max = std::max(max, NumCores((ThreadAffinity)i));
                }
                return max;
            } ();
            return std::pair{core_min, core_max};
        }

        int NumThreads(const CoreSetup& cfg, const ThreadAffinity affinity) {
            if (affinity == ThreadAffinity::kAll) {
                int count = 0;
                for (int i = 0; i < (int)ThreadAffinity::kAffinityCount; ++i) {
                    count += NumThreads(cfg, (ThreadAffinity)i);
                }
                return count;
            } else {
                if (cfg.affinity == ThreadAffinity::kAll) {
                    const auto min_max = CoreMinMax();
                    int count = NumCores(affinity);
                    if (cfg.max_affinity_cores != 0) {
                        count = std::min(count, cfg.max_affinity_cores);
                    }
                    if (cfg.match_cores) {
                        count = std::min(count, min_max.first);
                    }
                    return count;
                } else if (cfg.affinity == affinity) {
                    return std::min(NumCores(affinity), cfg.max_affinity_cores);
                } else {
                    return 0;
                }
            }
        }
    }

    template <typename ReporterCreator, typename ReadWrite>
    void Run(const BaseOperation& op, ReporterCreator make_reporter,
             ReadWrite& read_write, const BufferOpConfig& op_config,
             const FixedOpConfig& sub_config) {
        const auto num_threads = NumThreads(op_config.core_setup, ThreadAffinity::kAll);
        std::vector<std::thread> threads;
        threads.reserve(num_threads);
        ThreadSyncPoint sync_point{num_threads};

        for (int i = 0; i < (int)ThreadAffinity::kAffinityCount; ++i) {
            const auto affinity = (ThreadAffinity)i;
            const auto thread_idx = threads.size();
            const auto core_ct = NumThreads(op_config.core_setup, affinity);

            for (int core = 0 ; core < core_ct ; ++core) {
                const int core_id = (op_config.core_setup.fixed_cores ? core : -1);
                threads.push_back(std::thread{[&op, &read_write, &sync_point,
                                               make_reporter, core_id, affinity,
                                               thread_idx, num_threads, sub_config,
                                               run_count = op_config.run_count] {
                    SetThreadAffinity(core_id, affinity);
                    auto reporter = make_reporter(thread_idx);
                    sync_point.Sync(std::this_thread::get_id());
                    reporter.BeginReporting();

                    for (int i = 0 ; i < run_count ; ++i) {
                        switch (sub_config.scheme) {
                            case WorkScheme::kDividedEvenly:
                                return RunEvenDivision(reporter, sync_point,
                                                       thread_idx, num_threads,
                                                       read_write, op, sub_config);
                            case WorkScheme::kInterleaved:
                                return RunInterleaved(reporter, sync_point,
                                                      thread_idx, num_threads,
                                                      read_write, op, sub_config);
                            default:
                                FatalError(TAG, "Unknown work scheme: %d",
                                           (int)sub_config.scheme);
                        }
                    }
                }});
            }
        }

        assert(threads.size() ==  num_threads);
        for (auto& thread : threads) {
            thread.join();
        }
    }

//------------------------------------------------------------------------------
// Irregular

    template <typename ReporterCreator, typename ReadWrite>
    void Run(const BaseOperation& op, ReporterCreator make_reporter,
             ReadWrite& read_write, const BufferOpConfig& op_config,
             const IrregularOpConfig& sub_config) {
        const auto& core_setup = op_config.core_setup;
        if (NumCores(core_setup.affinity) == 0) {
            return;
        }
        SetThreadAffinity(core_setup.fixed_cores ? 0 : -1, core_setup.affinity);

        auto reporter = make_reporter(0);
        std::mt19937 random_generator{};
        const auto range = sub_config.max_advance - sub_config.min_advance;

        int count = 0;
        Bytes offset = 0;
        reporter.BeginReporting();

        while (count++ < op_config.run_count * sub_config.times && !op.IsStopped()) {
            const auto rand = Bytes(random_generator());
            offset += NextAlignedValue(Bytes(rand % range) + sub_config.min_advance,
                                       sub_config.rw_align);
            // Irregular loops if we hit the end of the buffer.
            while (offset >= read_write.BufferSize()) offset -= read_write.BufferSize();
            // TODO(tmillican@google.com): Not technically safe/correct, but if
            //  we actually run into the bug then our buffer and align/rw_size
            //  are absurdly close to one another.
            offset = NextAlignedValue(offset, sub_config.rw_align);

            read_write(reporter, offset);
        }
    }

//------------------------------------------------------------------------------
// Random

    template <typename ReporterCreator, typename ReadWrite>
    void Run(const BaseOperation& op, ReporterCreator make_reporter,
             ReadWrite read_write, const BufferOpConfig& op_config,
             const RandomOpConfig& sub_config) {
        const auto& core_setup = op_config.core_setup;
        if (NumCores(core_setup.affinity) == 0) {
            return;
        }
        SetThreadAffinity(core_setup.fixed_cores ? 0 : -1, core_setup.affinity);

        auto reporter = make_reporter(0);
        std::mt19937 random_generator{};

        for ( int count = 0 ; count < op_config.run_count * sub_config.times
                              && !op.IsStopped() ; ++count ) {
            const auto rand = Bytes(random_generator());
            auto offset = NextAlignedValue(Bytes(rand % read_write.BufferSize().count()),
                                           sub_config.rw_align);
            // We might have landed / aligned too close to the end.
            // TODO(tmillican@google.com): This solution makes the last bit of
            //  the buffer a 'hotspot', but in practice it shouldn't matter
            //  unless the read/align are absurdly close to the buffer's size.
            while (offset + read_write.RWSize() > read_write.BufferSize()) {
                offset -= sub_config.rw_align;
            }

            read_write(reporter, offset);
        }
    }

//------------------------------------------------------------------------------
// Helpers to go through the various templates to actually build the code.

    // Determine the size of the read/write and actually run the operation.
    template <template <typename> typename ReadWrite, typename ReporterCreator,
              typename ConfigType>
    void RunWithRW(const BaseOperation& op, ReporterCreator make_reporter, FixedBuffer& buffer,
                   const BufferOpConfig& op_config, const ConfigType& sub_config) {
        switch (op_config.touch) {
            case TouchSetup::kChar: {
                ReadWrite<char> rw{buffer, op_config};
                return Run(op, make_reporter, rw, op_config, sub_config);
            }
            case TouchSetup::kShort: {
                ReadWrite<short> rw{buffer, op_config};
                return Run(op, make_reporter, rw, op_config, sub_config);
            }
            case TouchSetup::kInt: {
                ReadWrite<int> rw{buffer, op_config};
                return Run(op, make_reporter, rw, op_config, sub_config);
            }
            case TouchSetup::kLong: {
                ReadWrite<long> rw{buffer, op_config};
                return Run(op, make_reporter, rw, op_config, sub_config);
            }
            case TouchSetup::kLongLong: {
                ReadWrite<long long> rw{buffer, op_config};
                return Run(op, make_reporter, rw, op_config, sub_config);
            }
            case TouchSetup::kBuffer: {
                ReadWrite<void*> rw{buffer, op_config};
                return Run(op, make_reporter, rw, op_config, sub_config);
            }

            default:
                FatalError(TAG, "Unknown touch size %d.", (int)op_config.touch);
        }
    }

    // Determine if we should use the read or write template.
    template <typename ReporterCreator, typename ConfigType>
    void RunWithSubconfig(const BaseOperation& op, ReporterCreator make_reporter,
                          FixedBuffer& buffer, const BufferOpConfig& op_config,
                          const ConfigType& sub_config) {
        switch (op_config.read_write) {
            case ReadOrWrite::kRead: {
                return RunWithRW<ReadOperation>(op, make_reporter, buffer,
                                                op_config, sub_config);
            }
            case ReadOrWrite::kWrite: {
                return RunWithRW<WriteOperation>(op, make_reporter, buffer,
                                                 op_config, sub_config);
            }
            default:
                FatalError(TAG, "Unknown read/write value: %d",
                           op_config.read_write);
        }
    }

    // Determine the type of operation.
    template <typename ReporterCreator>
    void RunWithConfig(const BaseOperation& op, ReporterCreator make_reporter,
                       FixedBuffer& buffer, const BufferOpConfig& op_config) {
        switch (op_config.operation_type) {
            case BufferOpType::kFixedOp:
                RunWithSubconfig(op, make_reporter, buffer, op_config,
                                 std::get<FixedOpConfig>(op_config.config));
                break;
            case BufferOpType::kIrregularOp:
                RunWithSubconfig(op, make_reporter, buffer, op_config,
                                 std::get<IrregularOpConfig>(op_config.config));
                break;
            case BufferOpType::kRandomOp:
                RunWithSubconfig(op, make_reporter, buffer, op_config,
                                 std::get<RandomOpConfig>(op_config.config));
                break;
            default:
                FatalError(TAG, "Unknown buffer operation: %d",
                           (int)op_config.operation_type);
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
            FixedBuffer buffer(_config.buffer_size.count(), _config.prewrite_buffer);

            int pass_id = 0;
            for (const auto& op : _config.operations) {
                Log::D(TAG, "%s %s", kBufferOpTypeNames[(size_t)op.operation_type],
                       kReadWriteNames[(size_t)op.read_write]);

                const auto ReporterCreator = [this, pass_id] (int thread_id) {
                    return WRReporter{Reporter<Datum>{*this, pass_id, thread_id},
                                      _config.report_rate};
                };

                RunWithConfig(*this, ReporterCreator, buffer, op);

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
