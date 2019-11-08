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
    enum class FileSetup {
        kBaseApk, kSplitApk, kObb, kCreatedFile
    };
    constexpr const char *kFileSetupNames[] = {
            "Base APK", "Split APK", "OBB", "Created File"
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
        kCApi, kCppStreams, kPosix
    };
    constexpr const char *kFileApiNames[] = {
            "CAPI", "C++ Streams", "posix"
    };

    struct Configuration {
        FileSetup file_setup;
        ThreadSetup thread_setup;
        WorkScheme work_scheme;
        FileApi file_api;
        // If we're creating a new file, how big should we make it?
        // TODO(tmillican@google.com): A size helper like we have with time
        //  would make writing configuration files a lot easier.
        size_t created_file_size = 0;

        // The section of the file to read from. read_area_end will default to
        // the end of the file if 0, so 0/0 will read from the entire file.
        size_t read_area_start = 0;
        size_t read_area_end = 0;
        // How much to read in total. We will repeat reading the above area *in
        // full* until we've reached/exceded this amount.
        size_t total_read;

        size_t read_align = 0;    // Pad so file reads start at this alignment.
        size_t buffer_size;       // Read this many bytes per read.
        size_t buffer_align = -1; // The alignment of the buffer allocation. -1
                                  // writes directly to the shared data; 0 uses
                                  // default malloc.

        // Should we pin threads to a specific core?
        // Note that big/little thread setups will still pin to big/little cores
        // without this flag. Threads just won't be pinned to a single specific
        // core.
        bool pin_affinity = true;
        // Lock before writing to shared data?
        // Depending on your read/alignment/etc., locking access to the final
        // buffer may be unnecessary. Even if it isn't, since this test is
        // primarily about file I/O we give the option to avoid that little bit
        // of extra overhead.
        bool lock_on_data_write = false;

        // Calculated/determined internally
        std::filesystem::path file_path; // Note: Currently single-file only
        size_t total_area_size;
        size_t padded_read_size;
        size_t total_data_size;
        size_t num_threads;
    };

    JSON_READER(Configuration) {
        JSON_REQENUM(file_setup, kFileSetupNames);
        JSON_REQENUM(thread_setup, kThreadSetupNames);
        JSON_REQENUM(work_scheme, kWorkSchemeNames);
        JSON_REQENUM(file_api, kFileApiNames);

        if (data.file_setup == FileSetup::kCreatedFile) {
            JSON_REQVAR(created_file_size);
        }
        JSON_OPTVAR(read_area_start);
        JSON_OPTVAR(read_area_end);
        JSON_REQVAR(total_read);

        JSON_OPTVAR(pin_affinity);
        JSON_OPTVAR(lock_on_data_write);
        JSON_OPTVAR(read_align);
        JSON_REQVAR(buffer_size);
        JSON_OPTVAR(buffer_align);

        // We can determine a few of these before verifying the file info.
        data.padded_read_size =
                NextAlignedValue(data.buffer_size, data.read_align);
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

    // A place to get the next file/data offsets from when using the greedy scheme.
    // TODO(tmillican@google.com): There has to be a better way of doing this...
    struct GreedyDispenser {
        std::mutex mutex;
        FileDataOffsets offsets;

        [[nodiscard]] FileDataOffsets GrabOffsets(const Configuration& config) {
            std::scoped_lock lock{mutex};
            auto prev_offsets = offsets;
            offsets.file += config.padded_read_size;
            offsets.data += config.buffer_size;
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
                // TODO(tmillican@google.com): Need to decipher the build setup
                //  for device_info to support this.
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
        , _data_chunk{config.buffer_size}
        , _file_division{config.total_area_size / config.num_threads}
        , _data_division{config.total_data_size / config.num_threads}
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
        : _offsets{config.padded_read_size * thread, config.buffer_size * thread}
        , _file_advance{config.padded_read_size * config.num_threads}
        , _data_advance{config.buffer_size * config.num_threads}
        , _file_end{config.total_area_size} {
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
                : _config(config), _greedy(greedy), _file_end{config.total_area_size} {
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
        : _buf_size(config.buffer_size)
        , _buffer(/*config.buffer_align
                ? aligned_alloc(config.buffer_align, _buffer_size)
                : */(char *) malloc(_buf_size))
        , _databuffer{data, config, scheme} {
            if (config.buffer_align != 0) {
                FatalError(TAG, "buffer_align is not currently supported.");
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
        , _buf_size(config.buffer_size)
        , _avoid_seek(config.work_scheme == WorkScheme::kDividedEvenly &&
                      config.padded_read_size == config.buffer_size)
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
        , _buf_size(config.buffer_size)
        , _avoid_seek(config.work_scheme == WorkScheme::kDividedEvenly &&
                      config.padded_read_size == config.buffer_size)
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
        , _buf_size(config.buffer_size)
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
            PrepareFileAndData();

            const auto thread_configs = DetermineThreadSetups(_config);
            _config.num_threads = std::size(thread_configs);

            // Perform the operation we've set up until we've reached/exceeded
            // our quota.
            std::vector<std::thread> threads;
            for ( auto read = 0 ; read < _config.total_read ;
                  read += _config.total_area_size ) {
                Log::D(TAG, "Performing new read (%zu / %zu)",
                       read, _config.total_read);

                threads.reserve(_config.num_threads);

                for (auto &thread_config : thread_configs) {
                    threads.emplace_back([this, thread_config] {
                        DoWork(thread_config);
                    });
                }

                for (auto &thread : threads) {
                    thread.join();
                }
                threads.clear();
            }

            Log::D(TAG, "Read complete");

            CleanupFileAndData();
        }};
    }

    void Wait() override {
        _thread.join();
    }

private:

//------------------------------------------------------------------------------
// File & Data creation & cleanup

    static auto CheckPreexistingFile(FileSetup setup) {
        std::filesystem::path file_path = [setup] {
            switch (setup) {
                case FileSetup::kBaseApk: return RawResourcePath();
                case FileSetup::kSplitApk: FatalError(TAG, "Currently unsupported");
                case FileSetup::kObb: FatalError(TAG, "Currently unsupported");
                default:
                    FatalError(TAG, "Bad file location %d", static_cast<int>(setup));
            }
        }();

        std::ifstream file{file_path, std::ios_base::binary};
        if (!file.is_open()) {
            FatalError(TAG, "Failed to open file '%s' for writing",
                       file_path.c_str());
        }
        file.ignore(std::numeric_limits<std::streamsize>::max());
        auto file_size = static_cast<size_t>(file.gcount());

        Log::D(TAG, "Verified %s file exists with size %zu",
               file_path.c_str(), file_size);

        return std::pair{file_path, file_size};
    }

    static auto CreateTemporaryFile(size_t file_size) {
        const auto file_path = std::filesystem::path{InternalDataPath() + "/test.bin"};
        std::ofstream file{file_path, std::ios_base::binary | std::ios_base::trunc};
        if (!file.is_open()) {
            FatalError(TAG, "Failed to open file '%s' for writing",
                       file_path.c_str());
        }

        for (size_t i = 0; i < file_size; ++i) {
            file.put(static_cast<char>(i));
        }

        Log::D(TAG, "Created %s with size %zu", file_path.c_str(), file_size);

        return std::pair{file_path, file_size};
    }

    //--------------

    void PrepareFileAndData() {
        const auto [file_path, file_size] =
                _config.file_setup == FileSetup::kCreatedFile
                ? CreateTemporaryFile(_config.created_file_size)
                : CheckPreexistingFile(_config.file_setup);

        _config.file_path = file_path;


        if (_config.read_area_end == 0) {
            _config.read_area_end = file_size;
        }
        if (_config.read_area_start > _config.read_area_end ||
            _config.read_area_end > file_size) {
            FatalError(TAG,
                    "Area start/end (%zu/%zu) are invalid for file of size %zu",
                    _config.read_area_start, _config.read_area_end, file_size);
        }
        _config.total_area_size = _config.read_area_end - _config.read_area_start;


        // TODO(tmillican@google.com): Might lose a bit of data on the end.
        _config.total_data_size =
                (_config.total_area_size / _config.padded_read_size) *
                _config.buffer_size;

        _data.data.resize(_config.total_data_size);
        if (_config.lock_on_data_write) {
            _data.mutex.emplace();
        }

        Log::D(TAG, "File & data prepared.");
    }


    void CleanupFileAndData() {
        _data.data.resize(0);
        _data.mutex = std::nullopt;

        if (_config.file_setup == FileSetup::kCreatedFile &&
            remove(_config.file_path.c_str()) != 0) {
            Log::W(TAG, "Failed to delete created file '%s' with errno '%d'",
                    _config.file_path.c_str(), errno);
        }

        Log::D(TAG, "Cleanup complete.");
    }

//------------------------------------------------------------------------------
// Putting everything together

    void DoWork(const ThreadConfiguration &thread_config) {
        if (CPU_EQUAL(&thread_config.affinity, &null_affinity)) {
            if (sched_setaffinity(0, sizeof(thread_config.affinity),
                                  &thread_config.affinity) == -1) {
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
        if (_config.buffer_align == -1) {
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
