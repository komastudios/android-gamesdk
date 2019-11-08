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
#include <condition_variable>
#include <fstream>
#include <mutex>
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
        constexpr Milliseconds DEFAULT_FLUSH_PERIOD_MILLIS = 1000ms;

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

            virtual void Write(const Datum& d) = 0;

            virtual void Write(const std::string& s) = 0;

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

            void Write(const Datum& d) override {
                _worker.Run(
                        [this, d](state* s) {
                            auto j = Json(d);
                            GetWriter().Write(j.dump()).Write("\n").Flush();
                        });
            }

            void Write(const std::string& msg) override {
                _worker.Run(
                        [this, msg](state* s) {
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

            void Write(const Datum& d) override {
                _worker.Run(
                        [this, d](state* s) {
                            auto j = Json(d);
                            GetWriter().Write(j.dump()).Write("\n");
                        });
            }

            void Write(const std::string& msg) override {
                _worker.Run(
                        [this, msg](state* s) {
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
                Log::I(
                        TAG,
                        "ReportSerializer_Periodic::dtor - _numWrites: %d - "
                        "stopping write thread...",
                        _num_writes);
                _alive.store(false, std::memory_order_relaxed);
                _write_thread.join();
                Log::I(TAG, "ReportSerializer_Periodic::dtor - DONE");
            }

            void SetFlushPeriod(Duration duration) {
                _flush_period = duration;
            }

            [[nodiscard]] auto
            GetFlushPeriod() const noexcept { return _flush_period; }

            void Write(const Datum& d) override {
                std::lock_guard<std::mutex> lock(_write_lock);
                _num_writes++;
                _log_items.emplace_back(d);
            }

            void Write(const std::string& msg) override {
                std::lock_guard<std::mutex> lock(_write_lock);
                _num_writes++;
                _log_items.emplace_back(msg);
            }

            void Flush() override {
                std::lock_guard<std::mutex> lock(_write_lock);

                if ( !_log_items.empty()) {

                    Log::I( TAG,
                            "ReportSerializer_Periodic::Flush - writing %d items",
                            _log_items.size());

                    auto& out = GetWriter();
                    for ( const auto& i : _log_items ) {
                        std::visit(
                                overloaded{
                                        [&out](Datum d) {
                                            auto j = Json(d);
                                            out.Write(j.dump()).Write("\n");
                                        },
                                        [&out](std::string s) {
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


    void OpenReportLog(const std::string& file) {
<<<<<<< HEAD   (dbad79 Revised crash when report file is open.)
        if ( _ofs.is_open()) {
          // It's fine if it happens that the report file is already open.
          // Opening happens upon Android MainActivity creation, that at times is recycled and
          // re-created as Android takes allocated resources back.
          Log::I(TAG, "OpenReportLog - File \"" + file + "\" was already open.");
        } else {
          _ofs = std::ofstream(file.c_str(), std::ofstream::out);
          Log::I(TAG, "OpenReportLog - Opened report log file \"" + file + "\"");
=======
        if (_writer) {
            FatalError(
                    TAG,
                    "Called OpenReportLog on an already open report; please "
                    "call CloseReportLog() first");
>>>>>>> BRANCH (1853aa Merge "Add BUILD_TYPE parameter to the samples CMake file. T)
        }
<<<<<<< HEAD   (dbad79 Revised crash when report file is open.)
        if (_report_serializer == nullptr) {
          _report_serializer = MakeReportSerializer(_report_flush_mode, _ofs);
        }
=======

        _writer = std::make_unique<Writer>(file);
        _report_serializer = MakeReportSerializer(
                _report_flush_mode, _writer.get());

        Log::I(TAG, "OpenReportLog - Opened report log file \"" + file + "\"");
>>>>>>> BRANCH (1853aa Merge "Add BUILD_TYPE parameter to the samples CMake file. T)
    }

    void OpenReportLog(int file_descriptor) {
        if (_writer) {
            FatalError(
                    TAG,
                    "Called OpenReportLog on an already open report; please "
                    "call CloseReportLog() first");
        }

        _writer = std::make_unique<Writer>(file_descriptor);
        _report_serializer = MakeReportSerializer(
                _report_flush_mode, _writer.get());

        Log::I(TAG, "OpenReportLog - Opened report log using file descriptor %d",
                file_descriptor);
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


    void WriteToReportLog(const reporting::Datum& d) {
        if ( !_report_serializer ) {
            FatalError(TAG, "Attempt to write to report log without opening "
                            "log first.");
        }
        _report_serializer->Write(d);
    }

    void WriteToReportLog(const std::string& s) {
        if ( !_report_serializer ) {
            FatalError(TAG, "Attempt to write to report log without opening "
                            "log first.");
        }
        _report_serializer->Write(s);
    }

    void FlushReportLogQueue() {
        if ( !_report_serializer ) {
            FatalError(TAG, "Attempt to write to report log without opening "
                            "log first.");
        }
        _report_serializer->Flush();
    }
} // namespace ancer::reporting