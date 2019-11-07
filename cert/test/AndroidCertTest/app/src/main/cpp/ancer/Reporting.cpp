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


//==================================================================================================

namespace ancer::reporting {

    namespace {
        constexpr Log::Tag TAG{"ancer::reporting"};
        constexpr Milliseconds DEFAULT_FLUSH_PERIOD_MILLIS = 1000ms;

        // -----------------------------------------------------------------------------------------


        class ReportSerializerBase {
        public:
            explicit ReportSerializerBase(std::ofstream& ofs) : _ofs(ofs) {}

            virtual ~ReportSerializerBase() = default;

            virtual void Write(const Datum& d) = 0;

            virtual void Write(const std::string& s) = 0;

            virtual void Flush() { _ofs.flush(); }

            std::ofstream& GetOutputStream() { return _ofs; }

        private:

            std::ofstream& _ofs;
        };

        /*
         * ReportSerializer_Immediate - writes logs immediately
         * to the specified ofstream, flushing after every write
         */
        class ReportSerializer_Immediate : public ReportSerializerBase {
        public:
            explicit ReportSerializer_Immediate(std::ofstream& ofs) :
                    ReportSerializerBase(ofs) {}

            void Write(const Datum& d) override {
                _worker.Run(
                        [this, d](state* s) {
                            auto j = Json(d);
                            GetOutputStream() << j.dump() << "\n";
                            GetOutputStream().flush();
                        });
            }

            void Write(const std::string& msg) override {
                _worker.Run(
                        [this, msg](state* s) {
                            GetOutputStream() << msg << "\n";
                            GetOutputStream().flush();
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

        /*
         * ReportSerializer_Manual - sends report data to ofstream
         * (which may flush when OS wants to), but explicitly flushed
         * when flush() is called.
         */
        class ReportSerializer_Manual : public ReportSerializerBase {
        public:
            explicit ReportSerializer_Manual(std::ofstream& ofs) :
                    ReportSerializerBase(ofs) {}

            void Write(const Datum& d) override {
                _worker.Run(
                        [this, d](state* s) {
                            auto j = Json(d);
                            GetOutputStream() << j.dump() << "\n";
                        });
            }

            void Write(const std::string& msg) override {
                _worker.Run(
                        [this, msg](state* s) {
                            GetOutputStream() << msg << "\n";
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

        /*
         * ReportSerializer_Periodic - stores writes in memory, serializing and
         * flushing periodically. See SetFlushPeriod()
         */
        class ReportSerializer_Periodic : public ReportSerializerBase {
        public:
            explicit ReportSerializer_Periodic(std::ofstream& ofs) :
                    ReportSerializerBase(ofs) {

                _write_thread = std::thread(
                        [this]() {
                            while ( _alive.load(std::memory_order_relaxed)) {
                                auto start_time =
                                        std::chrono::steady_clock::now();

                                Flush();

                                auto end_time =
                                        std::chrono::steady_clock::now();
                                auto elapsed = end_time - start_time;
                                auto sleep_time = _flush_period - elapsed;
                                if ( sleep_time > Duration::zero()) {
                                    std::this_thread::sleep_for(sleep_time);
                                }
                            }
                        });
            }

            ~ReportSerializer_Periodic() override {
                _alive.store(false, std::memory_order_relaxed);
                _write_thread.join();
                Log::I(TAG, "ReportSerializer_Periodic::dtor - DONE");
            }

            void SetFlushPeriod(Duration duration) {
                _flush_period = duration;
            }

            [[nodiscard]] auto GetFlushPeriod() const noexcept {
                return _flush_period;
            }

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
                    auto& out = GetOutputStream();
                    for ( const auto& i : _log_items ) {
                        std::visit(
                                overloaded{
                                        [&out](Datum d) {
                                            auto j = Json(d);
                                            out << j.dump() << "\n";
                                        },
                                        [&out](std::string s) {
                                            out << s << "\n";
                                        }
                                }, i);
                    }

                    _log_items.clear();
                    out.flush();
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


        // -----------------------------------------------------------------------------------------


        std::unique_ptr<ReportSerializerBase>
        MakeReportSerializer(ReportFlushMode mode, std::ofstream& out) {
            switch ( mode ) {
            case ReportFlushMode::Immediate:
                return std::move(
                        std::make_unique<ReportSerializer_Immediate>(out));
            case ReportFlushMode::Manual:
                return std::move(
                        std::make_unique<ReportSerializer_Manual>(out));
            case ReportFlushMode::Periodic:
                auto p = std::make_unique<ReportSerializer_Periodic>(
                        out);
                p->SetFlushPeriod(GetPeriodicFlushModePeriod());
                return std::move(p);
            }
        }

        // -----------------------------------------------------------------------------------------

        std::ofstream _ofs;
        ReportFlushMode _report_flush_mode = ReportFlushMode::Periodic;
        std::unique_ptr<ReportSerializerBase> _report_serializer;
        Duration _periodic_report_serializer_flush_period =
                DEFAULT_FLUSH_PERIOD_MILLIS;
    }


    void OpenReportLog(const std::string& file) {
        if ( _ofs.is_open()) {
          // It's fine if it happens that the report file is already open.
          // Opening happens upon Android MainActivity creation, that at times is recycled and
          // re-created as Android takes allocated resources back.
          Log::I(TAG, "OpenReportLog - File \"" + file + "\" was already open.");
        } else {
          _ofs = std::ofstream(file.c_str(), std::ofstream::out);
          Log::I(TAG, "OpenReportLog - Opened report log file \"" + file + "\"");
        }
        if (_report_serializer == nullptr) {
          _report_serializer = MakeReportSerializer(_report_flush_mode, _ofs);
        }
    }

    void CloseReportLog() {
        if ( _report_serializer ) {
            _report_serializer->Flush();
            _report_serializer = nullptr;
        }
        if ( _ofs.is_open()) {
            _ofs.close();
        }
    }

    void SetReportLogFlushMode(ReportFlushMode mode) {
        if ( mode != _report_flush_mode ) {
            _report_flush_mode = mode;
            if ( _ofs.is_open()) {
                _report_serializer = MakeReportSerializer(mode, _ofs);
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