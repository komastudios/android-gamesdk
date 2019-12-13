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

#include <atomic>
#include <fstream>
#include <mutex>
#include <sstream>
#include <thread>
#include <variant>

#include "Reporting.hpp"
#include "util/Error.hpp"
#include "util/Json.hpp"
#include "util/Log.hpp"
#include "util/WorkerThread.hpp"


//==============================================================================

namespace ancer::reporting {
    namespace {
        constexpr Log::Tag TAG{"ancer::reporting"};
    }

//==============================================================================

    namespace {
        std::atomic<int> _issue_id{0};

        int GetNextIssueId() {
            return _issue_id++;
        }

        std::string to_string(std::thread::id id) {
            std::stringstream ss;
            ss << id;
            return ss.str();
        }
    }

    Datum::Datum(Json custom) : Datum{"", "", std::move(custom)} {}

    Datum::Datum(std::string suite, std::string operation, Json custom_data)
    : issue_id(GetNextIssueId())
    , suite_id(std::move(suite))
    , operation_id(std::move(operation))
    , timestamp(SteadyClock::now())
    , thread_id(to_string(std::this_thread::get_id()))
    , cpu_id(sched_getcpu())
    , custom(std::move(custom_data)) {

    }

//------------------------------------------------------------------------------

    namespace {
        // Returns the number of characters needed to represent the given,
        // integer, potentially including a negative sign.
        template <typename T>
        constexpr auto CharsRequired(T val, bool include_minus_sign = true)
        -> std::enable_if_t<std::is_integral_v<T>, size_t> {
            int len = (include_minus_sign && val < 0 ? 2 : 1);
            while (val /= 10) {
                ++len;
            }
            return len;
        }

        // Saves a datum to a JSON-style string with a newline at the end.
        // We do this for performance reasons: Periodic reporting can result in
        // a queue of hundreds of datums to parse, and our standard JSON setup
        // can give a very noticeable performance hitch. We improve on this by:
        // 1) Saving to a string to the side so we can avoid one-or-more memory
        //    allocations for every datum-to-string conversion.
        // 2) Skipping the JSON middleman and hard-coding the formatting to
        //    avoid unnecessary work. We don't *really* need a generic JSON
        //    object, just the final text that a specific type would give us.
        void SaveReportLine(std::string& s, const Datum& d) {
            // TODO(tmillican@google.com): Most of this could probably be a
            //  collection of generic helpers. That's a bit excessive for now,
            //  though.

            static_assert(std::is_same_v<Timestamp::rep, long long>,
                    "Mismatch in printf type for timestamp.");
            static constexpr const char* fmt =
                R"({"issue_id":"%d", "suite_id":"%s", "operation_id":"%s",)"
                R"( "timestamp":%lld, "thread_id":"%s", "cpu_id":%d,)"
                R"( "custom":%s})" "\n";
            // 6 %X + 1 %lld
            constexpr auto raw_fmt_size = std::string_view{fmt}.size();
            static constexpr auto specifier_sz = 6*2 + 1*4;
            static constexpr auto fmt_sz = std::string_view{fmt}.size() - specifier_sz;

            const auto timestamp_ct = d.timestamp.time_since_epoch().count();
            const auto custom_str = d.custom.dump();
            const auto data_sz =
                    CharsRequired(d.issue_id) +
                    d.suite_id.size() +
                    d.operation_id.size() +
                    CharsRequired(timestamp_ct) +
                    d.thread_id.size() +
                    CharsRequired(d.cpu_id) +
                    custom_str.size();
            const auto string_sz = fmt_sz + data_sz;

            // TODO(tmillican@google.com): Some potential for unneeded
            //  initialization here, but I don't think there's a very good
            //  workaround short of writing our own string type. In our
            //  particular case it should be fairly minimal, though, and likely
            //  a lot better performance-wise than slowly expanding the string
            //  piece-by-piece.
            s.resize(string_sz);

            [[maybe_unused]]
            int result = snprintf(s.data(), s.size() + 1, fmt, d.issue_id,
                    d.suite_id.c_str(), d.operation_id.c_str(), timestamp_ct,
                    d.thread_id.c_str(), d.cpu_id, custom_str.c_str());
            assert(result == s.size() && string_sz == s.size());
        }

        std::string GetReportLine(const Datum& d) {
            std::string s;
            SaveReportLine(s, d);
            return s;
        }
    }

//==============================================================================

    namespace {
        constexpr Milliseconds DEFAULT_FLUSH_PERIOD_MILLIS = 1000ms;

        const/*expr*/ Json kFlushEvent = R"({ "report_event" : "flush" })"_json;

        class Writer {
        public:
            Writer(const std::string& file) :
                    _file(fopen(file.c_str(), "w")) {}

            Writer(int file_descriptor) :
                    _file(fdopen(file_descriptor, "w")) {}

            ~Writer() {
                fflush(_file);
                fclose(_file);
            }

            Writer& Write(const std::string& str) {
                fwrite(str.data(), sizeof(str[0]), str.length(), _file);
                return *this;
            }

            void Flush() {
                fflush(_file);
            }

        private:
            FILE* _file;
        };


        // ---------------------------------------------------------------------


        class ReportSerializerBase {
        public:
            explicit ReportSerializerBase(Writer* w) : _writer(w) {}

            virtual ~ReportSerializerBase() = default;

            virtual void Write(Datum&& d) = 0;

            virtual void Write(std::string&& s) = 0;

            virtual void Flush() { _writer->Flush(); }

            Writer& GetWriter() { return *_writer; }

        private:

            Writer* _writer;
        };

        /**
         * ReportSerializer_Immediate - writes logs immediately
         * to the specified Writer, flushing after every write
         */
        class ReportSerializer_Immediate : public ReportSerializerBase {
        public:
            explicit ReportSerializer_Immediate(Writer* writer) :
                    ReportSerializerBase(writer) {}

            void Write(Datum&& d) override {
                _worker.Run(
                        // TODO(tmillican@google.com): std::function doesn't support move
                        //[this, d = std::move(d)](state* s) {
                        [this, msg = GetReportLine(d)](state* s) {
                            GetWriter().Write(msg);
                        });
            }

            void Write(std::string&& msg) override {
                _worker.Run(
                        [this, msg = std::move(msg)](state* s) {
                            GetWriter().Write(msg).Write("\n").Flush();
                        });
            }

        private:
            struct state {
            };
            WorkerThread<state> _worker = {
                    "ReportSerializer_Immediate",
                    samples::Affinity::Odd
            };
        };

        /**
         * ReportSerializer_Manual - sends report data to ofstream
         * (which may flush when OS wants to), but explicitly flushed
         * when flush() is called.
         */
        class ReportSerializer_Manual : public ReportSerializerBase {
        public:
            explicit ReportSerializer_Manual(Writer* writer) :
                    ReportSerializerBase(writer) {}

            void Write(Datum&& d) override {
                _worker.Run(
                        // TODO(tmillican@google.com): std::function doesn't support move
                        //[this, d = std::move(d)](state* s) {
                        [this, msg = GetReportLine(d)](state* s) {
                            GetWriter().Write(msg);
                        });
            }

            void Write(std::string&& msg) override {
                _worker.Run(
                        [this, msg = std::move(msg)](state* s) {
                            GetWriter().Write(msg).Write("\n");
                        });
            }

        private:
            struct state {
            };
            WorkerThread<state> _worker = {
                    "ReportSerializer_Manual",
                    samples::Affinity::Odd
            };
        };

        // from https://en.cppreference.com/w/cpp/utility/variant/visit
        template <class... Ts>
        struct overloaded : Ts ... { using Ts::operator()...; };
        template <class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

        /**
         * ReportSerializer_Periodic - stores writes in memory, serializing and
         * flushing periodically. See SetFlushPeriod()
         */
        class ReportSerializer_Periodic : public ReportSerializerBase {
        public:
            explicit ReportSerializer_Periodic(Writer* writer) :
                    ReportSerializerBase(writer) {

                _write_thread = std::thread(
                        [this]() {

                            while ( _alive.load(std::memory_order_relaxed)) {
                                auto start_time = std::chrono::steady_clock::now();
                                Flush();
                                auto end_time = std::chrono::steady_clock::now();
                                auto elapsed = end_time - start_time;
                                auto sleep_time = _flush_period - elapsed;
                                if ( sleep_time > Duration::zero()) {
                                    std::this_thread::sleep_for(sleep_time);
                                }
                            }
                        });
            }

            ~ReportSerializer_Periodic() override {
                Log::V(
                        TAG,
                        "ReportSerializer_Periodic::dtor - _numWrites: %d - "
                        "stopping write thread...",
                        _num_writes);
                _alive.store(false, std::memory_order_relaxed);
                _write_thread.join();
                Log::V(TAG, "ReportSerializer_Periodic::dtor - DONE");
            }

            void SetFlushPeriod(Duration duration) {
                _flush_period = duration;
            }

            [[nodiscard]] auto
            GetFlushPeriod() const noexcept { return _flush_period; }

            void Write(Datum&& d) override {
                std::lock_guard<std::mutex> lock(_write_lock);
                ++_num_writes;
                _log_items.emplace_back(std::move(d));
            }

            void Write(std::string&& msg) override {
                std::lock_guard<std::mutex> lock(_write_lock);
                ++_num_writes;
                _log_items.emplace_back(std::move(msg));
            }

            void Flush() override {
                std::lock_guard<std::mutex> lock(_write_lock);

                if ( !_log_items.empty()) {

                    Log::V( TAG,
                            "ReportSerializer_Periodic::Flush - writing %d(+1) items",
                            _log_items.size());
                    // Add an extra report for this flush so testers can see if
                    // our reporting mechanism is impacting their results and
                    // adjust accordingly.
                    _log_items.emplace_back(Datum(kFlushEvent));

                    // Keep a buffer to the side for parsing datums into JSON
                    // strings so we don't need to allocate a new string for
                    // every datum (plus any additional allocations for capacity
                    // growth).
                    // 300 is just an arbitrary amount to get things started to
                    // hopefully avoid any extra allocations at all.
                    std::string datum_string;
                    datum_string.reserve(300);

                    auto& out = GetWriter();
                    for ( const auto& i : _log_items ) {
                        std::visit(
                                overloaded{
                                        [&out, &datum_string](const Datum& d) {
                                            SaveReportLine(datum_string, d);
                                            out.Write(datum_string);
                                        },
                                        [&out](const std::string& s) {
                                            out.Write(s).Write("\n");
                                        }
                                }, i);
                    }

                    _log_items.clear();
                    out.Flush();
                }
            }

        private:
            using log_item = std::variant<Datum, std::string>;

            std::atomic<bool> _alive{true};
            Duration _flush_period = DEFAULT_FLUSH_PERIOD_MILLIS;
            std::vector<log_item> _log_items;
            std::mutex _write_lock;
            std::thread _write_thread;
            size_t _num_writes = 0;
        };


        // ---------------------------------------------------------------------


        std::unique_ptr<ReportSerializerBase>
        MakeReportSerializer(ReportFlushMode mode, Writer* writer) {
            switch ( mode ) {
            case ReportFlushMode::Immediate:
                return std::move(
                        std::make_unique<ReportSerializer_Immediate>(writer));
            case ReportFlushMode::Manual:
                return std::move(
                        std::make_unique<ReportSerializer_Manual>(writer));
            case ReportFlushMode::Periodic:
                auto p = std::make_unique<ReportSerializer_Periodic>(
                        writer);
                p->SetFlushPeriod(GetPeriodicFlushModePeriod());
                return std::move(p);
            }
        }

        // ---------------------------------------------------------------------

        std::unique_ptr<Writer> _writer;
        ReportFlushMode _report_flush_mode = ReportFlushMode::Periodic;
        std::unique_ptr<ReportSerializerBase> _report_serializer;
        Duration _periodic_report_serializer_flush_period =
                DEFAULT_FLUSH_PERIOD_MILLIS;
    }

    void OpenReportLog(const std::string &file) {
        if (_writer) {
            // It's fine if it happens that the report writer is already
            // instantiated. Instantiation happens upon Android MainActivity
            // creation, that at times is recycled and re-created as Android
            // takes allocated resources back.
            Log::I(TAG, "OpenReportLog - File \"%s\" was already open.",
                   file.c_str());
        } else {
            _writer = std::make_unique<Writer>(file);
            Log::I(TAG, "OpenReportLog - Opened report log file \"%s\"",
                   file.c_str());
        }

        if (_report_serializer==nullptr) {
            _report_serializer =
                  MakeReportSerializer(_report_flush_mode, _writer.get());
        }
    }

    void OpenReportLog(int file_descriptor) {
        if (_writer) {
            Log::I(TAG, "OpenReportLog - File \"%d\" was already open.",
                   file_descriptor);
        } else {
            _writer = std::make_unique<Writer>(file_descriptor);
            Log::I(TAG, "OpenReportLog - Opened report log using file "
                        "descriptor %d", file_descriptor);
        }

        if (_report_serializer==nullptr) {
            _report_serializer =
                  MakeReportSerializer(_report_flush_mode, _writer.get());
        }
    }


    void CloseReportLog() {
        if ( _report_serializer ) {
            _report_serializer->Flush();
            _report_serializer = nullptr;
        }
        _writer = nullptr;
    }

    void SetReportLogFlushMode(ReportFlushMode mode) {
        if ( mode != _report_flush_mode ) {
            _report_flush_mode = mode;
            if (_writer) {
                _report_serializer = MakeReportSerializer(mode, _writer.get());
            }
        }
    }

    ReportFlushMode GetReportLogFlushMode() {
        return _report_flush_mode;
    }

    void SetPeriodicFlushModePeriod(Duration duration) {
        _periodic_report_serializer_flush_period = duration;
        switch ( _report_flush_mode ) {
        case ReportFlushMode::Periodic:
            dynamic_cast<ReportSerializer_Periodic*>(
                    _report_serializer.get())->SetFlushPeriod(
                        _periodic_report_serializer_flush_period);
        default:break;
        }
    }

    Duration GetPeriodicFlushModePeriod() noexcept {
        return _periodic_report_serializer_flush_period;
    }


    void WriteToReportLog(reporting::Datum&& d) {
        if ( !_report_serializer ) {
            FatalError(TAG, "Attempt to write to report log without opening "
                            "log first.");
        }
        _report_serializer->Write(std::move(d));
    }

    void WriteToReportLog(std::string&& s) {
        if ( !_report_serializer ) {
            FatalError(TAG, "Attempt to write to report log without opening "
                            "log first.");
        }
        _report_serializer->Write(std::move(s));
    }

    void FlushReportLogQueue() {
        if ( !_report_serializer ) {
            FatalError(TAG, "Attempt to write to report log without opening "
                            "log first.");
        }
        _report_serializer->Flush();
    }
} // namespace ancer::reporting