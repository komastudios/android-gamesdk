/*
 * Copyright 2020 The Android Open Source Project
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

#pragma once

#include "Reporting.hpp"

#include <charconv>
#include <cstdio>
#include <thread>
#include <utility>

#include "util/UnownedPtr.hpp"
#include "util/WorkerThread.hpp"


namespace ancer::reporting {
    namespace internal {
        constexpr size_t kPreallocatedReportSize = 512;

        // Note: Offset doubles as the next position to write and string's size.
        // This string is not null terminated.
        struct PreallocatedReport {
            // TODO(tmillican@gmail.com): Could trim memory use a bit by making
            //  'next' be an index and putting the pending flag in an unused bit
            //  in next/offset.
            struct {
                PreallocatedReport* next{nullptr};
                short offset{0};
                bool pending{false};
            } info;
            static constexpr size_t kBufferSize = kPreallocatedReportSize - sizeof(info);
            char buffer[kBufferSize];

            static_assert(std::numeric_limits<decltype(decltype(info)::offset)>::max() >=
                          PreallocatedReport::kBufferSize);
        };
        static_assert(sizeof(PreallocatedReport) == kPreallocatedReportSize);

        inline void Clear(PreallocatedReport& rep) {
            rep.info.next = nullptr;
            rep.info.offset = 0;
            rep.info.pending = false;
        }

        inline bool IsClear(PreallocatedReport& rep) {
            return rep.info.next == nullptr &&
                   rep.info.offset == 0 &&
                   rep.info.pending == false;
        }

//------------------------------------------------------------------------------

        class ReportSerializer;

        class ReportWriter {
        public:
            struct DatumParams {
                // NOTE: We assume that the suite/operation string will outlast
                //  us queuing up this report.
                DatumParams(const char* suite, const char* operation);

                // TODO(tmillican@google.com): Update WorkerThread to support
                //  move semantics so we can make this move-only to avoid any
                //  risk of duplicate issue IDs.
                // DatumParams(const DatumParaums&) = delete;
                // DatumParams(DatumParaums&&) = default;

                const char* suite;
                const char* operation;
                int issue_id;
                long long timestamp;
                std::thread::id thread_id;
                int cpu_id;
            };

            ReportWriter(internal::ReportSerializer&);
            virtual ~ReportWriter();

            void PrintDatumBoilerplate(const DatumParams&);

            template <typename T>
            void ToChars(T val);
            void Print(char c);
            void Print(std::string_view str) { PrintData(str.data(), str.size()); }
            void PrintData(const char* data, size_t);

        private:
            void ExtendReport();

            bool CheckResult(std::to_chars_result result);

            [[nodiscard]] char* Buffer() noexcept;
            [[nodiscard]] auto& Offset() noexcept;
            [[nodiscard]] auto BufSz() noexcept;
            [[nodiscard]] auto BufLeft() noexcept;

            ReportSerializer& _serializer;
            PreallocatedReport& _first_report;
            PreallocatedReport* _current_report;
        };

//------------------------------------------------------------------------------

        class ReportSerializer {
            struct WorkerState {};
        public:
            explicit ReportSerializer(FILE* file, ReportFlushMode mode);
            ~ReportSerializer();

            template <typename Datum>
            void ReportDatum(const char* suite, const char* op, Datum&& datum) {
                ReportWriter::DatumParams params{suite, op};
                _worker.Run([this, params = std::move(params),
                             datum = std::forward<Datum>(datum)](WorkerState* s) {
                    auto writer = ReportWriter{*this};
                    writer.PrintDatumBoilerplate(params); // { ..., "custom":
                        WriteDatum(writer, datum);        //   {...}
                    writer.Print("}\n");                  // }
                });
            }

            void ReportString(std::string&& str) {
                _worker.Run([this, str = std::move(str)](WorkerState* s) {
                    auto writer = ReportWriter{*this};
                    writer.Print(str);
                    writer.Print('\n');
                });
            }

            void SetFlushPeriod(Duration d) { _flush_period = d; }

            [[nodiscard]] auto GetMode() const noexcept { return _mode; }
            [[nodiscard]] auto GetFlushPeriod() const noexcept { return _flush_period; }

            // Flushes any queued up reports.
            // A "hard" flush waits for any pending reports to finish and also
            // flushes the actual file.
            void Flush(bool hard_flush = false);

        private:
            friend class ancer::reporting::internal::ReportWriter;
            PreallocatedReport& GetNextReport(bool append);
            void ReportFinished(PreallocatedReport& base);

        private:
            ancer::unowned_ptr<FILE> _file;
            ReportFlushMode _mode;

            WorkerThread<WorkerState> _worker = {
                "ReportSerializer",
                samples::Affinity::Odd
            };

            std::thread _periodic_thread;
            std::atomic<bool> _alive{true};
            Duration _flush_period;

            // TODO(tmillican@google.com): Double-buffer so we can keep caching
            //  reports during a flush.
            std::mutex _report_mutex;
            std::unique_ptr<PreallocatedReport[]> _reports;
            // TODO(tmillican@google.com): This should probably be in a proper
            //  class to make it safer to use.
            // Circular range for which reports have been 'allocated' but not
            // yet flushed.
            int _allocated_beg{0};
            int _allocated_end{0};
            // Second 'end' iterator marking how many of the allocated reports
            // have been written.
            int _pending_end{0};
        };

        ReportSerializer& GetReportSerializer();
    }

//==============================================================================

    template <typename T>
    void WriteToReportLog(const std::string& suite, const std::string& operation,
                          T&& datum) {
        WriteToReportLog(suite.c_str(), operation.c_str(), std::forward<T>(datum));
    }

    template <typename T>
    void WriteToReportLog(const char* suite, const char* operation,
                          T&& datum) {
        internal::GetReportSerializer().ReportDatum(suite, operation,
                                                    std::forward<T>(datum));
    }

    inline void WriteToReportLog(std::string&& str) {
        internal::GetReportSerializer().ReportString(std::move(str));
    }

//==============================================================================

    namespace internal {
        inline char* ReportWriter::Buffer() noexcept { return _current_report->buffer; }
        inline auto& ReportWriter::Offset() noexcept { return _current_report->info.offset; }
        inline auto ReportWriter::BufSz()   noexcept { return _current_report->kBufferSize; }
        inline auto ReportWriter::BufLeft() noexcept { return BufSz() - Offset(); }

        template <typename T>
        void ReportWriter::ToChars(T val) {
            while (CheckResult(std::to_chars(Buffer() + Offset(),
                                             Buffer() + BufSz(), val))) {
                continue;
            }
        }
    }
}
