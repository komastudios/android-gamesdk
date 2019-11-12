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

/*
 * Current state compared to spec (see Android Predictability Performance Tests)
 *
 * - Big/Little core are unsupported. We need to figure out if this is something
 *   we can even determine programmatically.
 * - nio reads are unsupported. This seems to be a Java thing (Android is still
 *   a new platform to me); if that's the case, it seems like an in-Java test is
 *   the proper setup for that.
 * - Data is only in a test-created file. Our current test framework is all-or-
 *   nothing with the tests, so adding data (especially the amount requested)
 *   will require work on that front.
 * - Random reads aren't implemented. This is mostly an oversight.
 */
#include <ancer/BaseOperation.hpp>

#include <cstdlib>
#include <fcntl.h>
#include <filesystem>
#include <fstream>
#include <limits>
#include <optional>
#include <sched.h>
#include <thread>
#include <unistd.h>

#include <cpu-features.h>

#include <ancer/System.hpp>
#include <ancer/util/Json.hpp>

using namespace ancer;


//==============================================================================

namespace {
    constexpr Log::Tag TAG{"IOPerformanceOperation"};

    // TODO(tmillican@google.com): Probably belongs in a utility header somewhere.
    template<typename T>
    [[nodiscard]] constexpr auto NextAlignedValue(T value, T align) {
        const auto offset = align ? value % align : 0;
        return value + (offset ? align - offset : 0);
    }
}

//==============================================================================
// Config & Datum

namespace {
    enum class FileLocation {
        kBaseApk, kSplitApk, kObb, kCreatedFile
    };
    constexpr const char *kFileLocationNames[] = {
            "Base APK", "Split APK", "OBB", "Create File"
    };

    enum class ThreadSetup {
        kOneCore, kAllCores, kBigCores, kLittleCores
    };
    constexpr const char *kThreadSetupNames[] = {
            "Single Core", "All Cores", "Big Cores", "Little Cores"
    };

    enum class WorkScheme {
        kDividedEvenly, kInterleaved, kGreedy
    };
    constexpr const char *kWorkSchemeNames[] = {
            "Divided", "Interleaved", "Greedy"
    };

    enum class FileApi {
        kCApi, kCppStreams, kPosix, kNio
    };
    constexpr const char *kFileApiNames[] = {
            "CAPI", "C++ Streams", "posix", "nio"
    };

    struct Configuration {
        FileLocation file_location; // Where is the file we're reading from?
        ThreadSetup thread_setup;   // How many threads should we have?
        WorkScheme work_scheme;     // How should we divide work between the threads?
        FileApi file_api;           // Which file API are we using?
        bool pin_affinity = true;   // Should we lock each thread to a specific CPU?
        bool lock_data = false;     // Should we lock shared data before writing to it?
        size_t data_size;      // How much data should we read?
        size_t read_align = 0; // Pad the file so reads start at this alignment.
        size_t buf_size;       // Read this many bytes per read
        size_t buf_align = -1; // The alignment of the buffer allocation. -1
                               // writes directly to the shared data; 0 uses
                               // default malloc.

        // Determined internally
        size_t padded_read_size;
        std::filesystem::path file_path;
        size_t file_size;
        size_t num_threads;
    };

    JSON_READER(Configuration) {
        JSON_REQENUM(file_location, kFileLocationNames);
        JSON_REQENUM(thread_setup, kThreadSetupNames);
        JSON_REQENUM(work_scheme, kWorkSchemeNames);
        JSON_REQENUM(file_api, kFileApiNames);
        JSON_OPTVAR(pin_affinity);
        JSON_OPTVAR(lock_data);
        JSON_REQVAR(data_size);
        JSON_OPTVAR(read_align);
        JSON_REQVAR(buf_size);
        JSON_OPTVAR(buf_align);
    }

//------------------------------------------------------------------------------

    struct Datum {
        // TODO(tmillican@google.com): Need to review to see what info is valuable.
    };

    JSON_WRITER(Datum) {
    }
}

//==============================================================================
// Shared datatypes

namespace {
    struct FileDataOffsets {
        size_t file;
        size_t data;
    };
    static constexpr FileDataOffsets max_offsets{
            std::numeric_limits<size_t>::max(),
            std::numeric_limits<size_t>::max()
    };


    // A place to get the next file/data offsets from when using the greedy scheme.
    // TODO(tmillican@google.com): There has to be a better way of doing this...
    struct GreedyDispenser {
        std::mutex mutex;
        FileDataOffsets offsets;

        [[nodiscard]] FileDataOffsets GrabOffsets(const Configuration& config) {
            std::scoped_lock lock{mutex};
            auto prev_offsets = offsets;
            offsets.file += config.padded_read_size;
            offsets.data += config.buf_size;
            return prev_offsets;
        }
    };


    // The final location all data is being collected in.
    struct FinalData {
        std::optional<std::mutex> mutex;
        std::vector<char> data;
    };
}

//==============================================================================
// Thread setup & affinity

namespace {
    const/*expr*/ cpu_set_t null_affinity = [] {
        cpu_set_t affinity;
        CPU_ZERO(&affinity);
        return affinity;
    }();

    [[nodiscard]]
    constexpr auto CpuAffinity(int cpu, cpu_set_t affinity = null_affinity) {
        CPU_SET(cpu, &affinity);
        return affinity;
    }

//------------------------------------------------------------------------------

    struct ThreadConfiguration {
        int id;
        cpu_set_t affinity;
    };

    [[nodiscard]] auto DetermineThreadSetups(const Configuration &config) {
        std::vector<ThreadConfiguration> threads;
        switch (config.thread_setup) {
            case ThreadSetup::kOneCore:
                threads.push_back(
                        {0,
                         config.pin_affinity ? CpuAffinity(0) : null_affinity});
                break;
            case ThreadSetup::kAllCores:
                for (int cpu = 0, cpu_count = android_getCpuCount();
                     cpu < cpu_count; ++cpu) {
                    const auto affinity = config.pin_affinity ? CpuAffinity(cpu)
                                                              : null_affinity;
                    threads.push_back({cpu, affinity});
                }
                break;
            case ThreadSetup::kBigCores:
            case ThreadSetup::kLittleCores:
                // TODO(tmillican@google.com): We need to figure out if it's
                //  even possible to determine this in code.
                FatalError(TAG,
                           "Big/Little core thread setup is currently unsupported.");
            default:
                FatalError(TAG, "Unknown thread setup requested: '%d'",
                           config.thread_setup);
        }
        return threads;
    }
}

//==============================================================================
// Advancing in the file/data based on our thread & scheme

namespace {
    // Minor note: The logic was written with even chunks/divisions in mind, but
    // it will still work with 'unbalanced' loads. There may be a tiny bit of
    // re-reading at division edges in some setups, but it's effectively a non-
    // issue.
    class EvenDivision {
    public:
        EvenDivision(int thread, const Configuration& config)
        : _file_chunk{config.padded_read_size}
        , _data_chunk{config.buf_size}
        , _file_division{config.file_size / config.num_threads}
        , _data_division{config.data_size / config.num_threads}
        , _offsets{_file_division * thread, _data_division * thread}
        , _file_end{_offsets.file + _file_division} {
        }

        [[nodiscard]]
        const FileDataOffsets& GetOffsetView() const noexcept { return _offsets; }

        void Update() {
            _offsets.file += _file_chunk;
            _offsets.data += _data_chunk;
        }

        [[nodiscard]] bool ShouldContinue() const noexcept {
            return _offsets.file < _file_end;
        }
    private:
        size_t _file_chunk;
        size_t _data_chunk;
        size_t _file_division;
        size_t _data_division;
        FileDataOffsets _offsets;
        size_t _file_end;
    };


    class Interleaved {
    public:
        Interleaved(int thread, const Configuration& config)
        : _offsets{config.padded_read_size * thread, config.buf_size * thread}
        , _file_advance{config.padded_read_size * config.num_threads}
        , _data_advance{config.buf_size * config.num_threads}
        , _file_end{config.file_size} {
        }

        [[nodiscard]]
        const FileDataOffsets& GetOffsetView() const noexcept { return _offsets; }

        void Update() {
            _offsets.file += _file_advance;
            _offsets.data += _data_advance;
        }

        [[nodiscard]] bool ShouldContinue() const noexcept {
            return _offsets.file < _file_end;
        }
    private:
        FileDataOffsets _offsets;
        size_t _file_advance;
        size_t _data_advance;
        size_t _file_end;
    };


    class Greedy {
    public:
        Greedy(const Configuration& config, GreedyDispenser& greedy)
                : _config(config), _greedy(greedy), _file_end{config.file_size} {
            Update();
        }

        [[nodiscard]]
        const FileDataOffsets& GetOffsetView() const noexcept { return _offsets; }

        void Update() {
            _offsets = _greedy.GrabOffsets(_config);
        }

        [[nodiscard]] bool ShouldContinue() const noexcept {
            return _offsets.file < _file_end;
        }
    private:
        const Configuration& _config;
        GreedyDispenser& _greedy;
        size_t _file_end;
        FileDataOffsets _offsets;
    };
}

//==============================================================================
// Handling a shared & intermediate data buffers.

namespace {
    // Doubles as a post-read helper for OwnedBuffer.
    class DirectBuffer {
    public:
        template <typename WorkScheme>
        DirectBuffer(FinalData &data, const Configuration &config,
                     const WorkScheme& scheme)
        : _data(data)
        , _offsets(scheme.GetOffsetView()) {
        }

        template <typename File>
        void ReadFrom(File&& file) {
            if (_data.mutex) {
                std::scoped_lock lock{*_data.mutex};
                file.Read(GetData());
            } else {
                file.Read(GetData());
            }
        }

        void CopyFrom(const char* buffer, size_t size) {
            if (_data.mutex) {
                std::scoped_lock lock{*_data.mutex};
                memcpy(GetData(), buffer, size);
            } else {
                memcpy(GetData(), buffer, size);
            }
        }

    private:
        [[nodiscard]] char *GetData() noexcept {
            return _data.data.data() + _offsets.data;
        }

        FinalData &_data;
        const FileDataOffsets &_offsets;
    };


    // Reads to a thread-owned buffer and moves into the shared buffer afterward.
    class OwnedBuffer {
    public:
        template <typename WorkScheme>
        OwnedBuffer(FinalData &data, const Configuration &config,
                    const WorkScheme& scheme)
        : _buf_size(config.buf_size)
        , _buffer(/*config.buf_align
                ? aligned_alloc(config.buf_align, _buf_size)
                : */(char *) malloc(_buf_size))
        , _databuffer{data, config, scheme} {
            if (config.buf_align != 0) {
                FatalError(TAG, "buf_align is not currently supported.");
            }
        }

        ~OwnedBuffer() {
            free(_buffer);
        }

        template <typename File>
        void ReadFrom(File&& file) {
            file.Read(_buffer);
            _databuffer.CopyFrom(_buffer, _buf_size);
        }

    private:
        size_t _buf_size;
        char *_buffer;
        DirectBuffer _databuffer;
    };
}

//==============================================================================
// Different file APIs

namespace {
    class CApi {
    public:
        template <typename Scheme>
        CApi(const Configuration &config, const Scheme& scheme)
        : _offsets(scheme.GetOffsetView())
        , _buf_size(config.buf_size)
        , _avoid_seek(config.work_scheme == WorkScheme::kDividedEvenly &&
                      config.padded_read_size == config.buf_size)
        , _file(fopen(config.file_path.c_str(), "rb")) {
            if (_file == nullptr) {
                FatalError(TAG, "Failed to open file '%s'",
                           config.file_path.c_str());
            }
        }

        ~CApi() {
            fclose(_file);
        }

        void Read(char *buffer) {
            if (!_avoid_seek) {
                fseek(_file, _offsets.file, SEEK_SET);
            }
            fread(buffer, sizeof(char), _buf_size, _file);
        }

    private:
        const FileDataOffsets &_offsets;
        size_t _buf_size;
        bool _avoid_seek;
        FILE *_file;
    };


    class CppStreams {
    public:
        template <typename Scheme>
        CppStreams(const Configuration &config, const Scheme& scheme)
        : _offsets(scheme.GetOffsetView())
        , _buf_size(config.buf_size)
        , _avoid_seek(config.work_scheme == WorkScheme::kDividedEvenly &&
                      config.padded_read_size == config.buf_size)
        , _file(config.file_path, std::ios::binary) {
            if (!_file.is_open()) {
                FatalError(TAG, "Failed to open file '%s'",
                           config.file_path.c_str());
            }
        }

        void Read(char *buffer) {
            if (!_avoid_seek) {
                _file.seekg(_offsets.file);
            }
            _file.read(buffer, _buf_size);
        }

    private:
        const FileDataOffsets &_offsets;
        size_t _buf_size;
        bool _avoid_seek;
        std::ifstream _file;
    };


    class Posix {
    public:
        template <typename Scheme>
        Posix(const Configuration &config, const Scheme& scheme)
        : _offsets(scheme.GetOffsetView())
        , _buf_size(config.buf_size)
        , _file(open(config.file_path.c_str(), O_RDONLY)) {
            if (_file == -1) {
                FatalError(TAG, "Failed to open file '%s' with errno %d",
                           config.file_path.c_str(), errno);
            }
        }

        ~Posix() {
            close(_file);
        }

        void Read(char *buffer) {
            pread(_file, buffer, _buf_size, _offsets.file);
        }

    private:
        const FileDataOffsets &_offsets;
        size_t _buf_size;
        int _file;
    };


    class Nio {
    public:
        template <typename Scheme>
        Nio(const Configuration&, const Scheme&) {
            FatalError(TAG, "nio is unimplemented");
        }

        void Read(char *) {}
    };
}

//==============================================================================
// The main class

class IOPerformanceOperation : public BaseOperation {
public:
    IOPerformanceOperation() = default;

    void Start() override {
        BaseOperation::Start();
        _config = GetConfiguration<Configuration>();

        _thread = std::thread{[this] {
            CreateFileAndData();

            const auto thread_configs = DetermineThreadSetups(_config);
            _config.num_threads = std::size(thread_configs);

            std::vector<std::thread> threads;
            threads.reserve(_config.num_threads);
            for (auto &thread_config : thread_configs) {
                threads.emplace_back([this, thread_config] {
                    DoWork(thread_config);
                });
            }

            for (auto &thread : threads) {
                thread.join();
            }

            VerifyData();
            DeleteFileAndData();
        }};
    }

    void Wait() override {
        _thread.join();
    }

private:

//------------------------------------------------------------------------------
// File & Data creation, verification, & cleanup

    void CreateFileAndData() {
        // It's possible to support 'unbalanced' loads, but for now we're just
        // aiming to implement something with the logic as simple as possible.
        assert(_config.data_size % _config.buf_size == 0);
        const auto num_reads = _config.data_size / _config.buf_size;

        _config.padded_read_size =
                NextAlignedValue(_config.buf_size, _config.read_align);
        _config.file_size = _config.padded_read_size * num_reads;

        switch (_config.file_location) {
            case FileLocation::kBaseApk:
            case FileLocation::kSplitApk:
            case FileLocation::kObb:
                FatalError(TAG, "Currently only Created File is supported");
            case FileLocation::kCreatedFile: {
                _config.file_path = InternalDataPath() + "/test.bin";
                std::ofstream file{_config.file_path,
                                   std::ios_base::binary |
                                   std::ios_base::trunc};
                if (!file.is_open()) {
                    FatalError(TAG, "Failed to open file '%s' for writing",
                               _config.file_path.c_str());
                }

                // "Real" data will increment & cycle with every byte, padding is
                // always 0.
                const auto pad_size =
                        _config.padded_read_size - _config.buf_size;
                for (size_t i = 0; i < _config.data_size;) {
                    for (size_t j = 0; j < _config.buf_size; ++j)
                        file.put(static_cast<char>(i++));
                    for (size_t j = 0; j < pad_size; ++j)
                        file.put(0);
                }
                break;
            }
            default:
                FatalError(
                        TAG, "Unsupported file location %d",
                        static_cast<int>(_config.file_location));
        }


        _data.data.resize(_config.data_size);
        if (_config.lock_data) {
            _data.mutex.emplace();
        }
    }


    void VerifyData() {
        for (size_t i = 0; i < _config.data_size; ++i) {
            if (_data.data[i] != static_cast<char>(i)) {
                FatalError(TAG, "Data[%zu] is not what it should be!", i);
            }
        }
        Log::D(TAG, "%zu bytes verified!", _config.data_size);
    }


    void DeleteFileAndData() {
        _data.data.resize(0);
        _data.mutex = std::nullopt;

        if (_config.file_location == FileLocation::kCreatedFile &&
            remove(_config.file_path.c_str()) != 0) {
            Log::W(TAG, "Failed to delete created file '%s' with errno '%d'",
                    _config.file_path.c_str(), errno);
        }
    }

//------------------------------------------------------------------------------
// Putting everything together

    void DoWork(const ThreadConfiguration &thread_config) {
        if (CPU_EQUAL(&thread_config.affinity, &null_affinity)) {
            if (sched_setaffinity(0, sizeof(thread_config.affinity),
                                  &thread_config.affinity)
                == -1) {
                FatalError(TAG, "setaffinity failed with errno %d", errno);
            }
            // TODO(tmillican@google.com): Periodically check to make sure our
            //  affinity 'stuck'? There's an explicit test for that elsewhere
            //  and our standard logging notes the affinity, but it may be worth
            //  explicitly noting if it changes.
        }

        switch (_config.work_scheme) {
            case WorkScheme::kDividedEvenly: {
                EvenDivision scheme{thread_config.id, _config};
                return DoWork_(scheme);
            }
            case WorkScheme::kInterleaved: {
                Interleaved scheme{thread_config.id, _config};
                return DoWork_(scheme);
            }
            case WorkScheme::kGreedy: {
                Greedy scheme{_config, _greedy};
                return DoWork_(scheme);
            }
            default:
                FatalError(TAG, "Invalid work scheme '%d'", _config.work_scheme);
        }
    }

    template <typename Scheme>
    void DoWork_(Scheme& scheme) {
        if (_config.buf_align == -1) {
            DirectBuffer buffer{_data, _config, scheme};
            DoWork_(scheme, buffer);
        } else {
            OwnedBuffer buffer{_data, _config, scheme};
            DoWork_(scheme, buffer);
        }
    }

    template <typename Scheme, typename Buffer>
    void DoWork_(Scheme& scheme, Buffer& buffer) {
        switch (_config.file_api) {
            case FileApi::kCApi: {
                CApi api{_config, scheme};
                return DoWork_(scheme, buffer, api);
            }
            case FileApi::kCppStreams: {
                CppStreams api{_config, scheme};
                return DoWork_(scheme, buffer, api);
            }
            case FileApi::kPosix: {
                Posix api{_config, scheme};
                return DoWork_(scheme, buffer, api);
            }
            case FileApi::kNio: {
                Nio api{_config, scheme};
                return DoWork_(scheme, buffer, api);
            }
            default:
                FatalError(TAG, "Invalid File API '%d'", _config.file_api);
        }
    }

    template<typename Scheme, typename Buffer, typename File>
    void DoWork_(Scheme& scheme, Buffer& buffer, File& file) {
        while (!IsStopped() && scheme.ShouldContinue()) {
            buffer.ReadFrom(file);
            scheme.Update();
        }
    }

//------------------------------------------------------------------------------

    Configuration _config;
    std::thread _thread;

    GreedyDispenser _greedy;
    FinalData _data;
};

//==============================================================================

EXPORT_ANCER_OPERATION(IOPerformanceOperation)
