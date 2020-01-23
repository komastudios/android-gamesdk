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

#include "Reporting.hpp"

#include <atomic>
#include <cstdio>
#include <mutex>
#include <sstream>
#include <thread>
#include <variant>

#include "util/Error.hpp"
#include "util/Json.hpp"
#include "util/Log.hpp"
#include "util/WorkerThread.hpp"
#include "Reporting.inl"

using namespace ancer;
using namespace ancer::reporting;

using PreallocatedReport = internal::PreallocatedReport;
using ReportWriter = internal::ReportWriter;
using ReportSerializer = internal::ReportSerializer;


//==============================================================================

namespace {
    constexpr Log::Tag TAG{"ancer::reporting"};

    std::unique_ptr<internal::ReportSerializer> _report_serializer;
    // So serializer can just handle changes at create/destroy and/or we can
    // set these before actually opening the file..
    FILE* _file = nullptr;
    ReportFlushMode _flush_mode = ReportFlushMode::Periodic;
}

//------------------------------------------------------------------------------

ReportSerializer& reporting::internal::GetReportSerializer() {
    assert(_report_serializer);
    return *_report_serializer;
}

void reporting::OpenReportLog(const std::string& file) {
    if (_report_serializer) {
        Log::I(TAG, "OpenReportLog - File was already open, refrained from "
                    "opening \'%s\'.", file.c_str());
    } else {
        _file = fopen(file.c_str(), "w");
        _report_serializer = std::make_unique<ReportSerializer>(_file, _flush_mode);
        Log::I(TAG, "OpenReportLog - Opened report log file \"%s\"",
               file.c_str());
    }
}

void reporting::OpenReportLog(int file_descriptor) {
    if (_report_serializer) {
        Log::I(TAG, "OpenReportLog - File was already open, refrained from "
                    "opening \'%d\'.", file_descriptor);
    } else {
        _file = fdopen(file_descriptor, "w");
        _report_serializer = std::make_unique<ReportSerializer>(_file, _flush_mode);
        Log::I(TAG, "OpenReportLog - Opened report log file \"%d\"",
               file_descriptor);
    }
}

void reporting::SetReportLogFlushMode(ReportFlushMode mode) {
    if ( mode != _flush_mode ) {
        _flush_mode = mode;
        _report_serializer = std::make_unique<ReportSerializer>(_file, _flush_mode);;
    }
}

ReportFlushMode reporting::GetReportLogFlushMode() {
    return _flush_mode;
}

void reporting::SetPeriodicFlushModePeriod(Duration duration) {
    assert(_report_serializer);
    _report_serializer->SetFlushPeriod(duration);
}

Duration reporting::GetPeriodicFlushModePeriod() noexcept {
    assert(_report_serializer);
    return _report_serializer->GetFlushPeriod();
}

void reporting::FlushReportLogQueue() {
    if ( !_report_serializer ) {
        FatalError(TAG, "Attempt to flush report queue without an open log.");
    } else {
        _report_serializer->Flush();
    }
}

void reporting::HardFlushReportLogQueue() {
    if ( !_report_serializer ) {
        FatalError(TAG, "Attempt to flush report queue without an open log.");
    } else {
        _report_serializer->Flush(true);
    }
}

//==============================================================================

namespace {
    constexpr Milliseconds kDefaultFlushPeriod = 500ms;
    constexpr int kDefaultSmallReportQueueSize = 32;
    constexpr int kDefaultLargeReportQueueSize = 5120;
}

//------------------------------------------------------------------------------

namespace {
    std::atomic<int> _issue_id{0};
}

ReportWriter::DatumParams::DatumParams(const char* st, const char* op)
: suite(st)
, operation(op)
, issue_id(_issue_id++)
, timestamp(SteadyClock::now().time_since_epoch().count())
, thread_id(std::this_thread::get_id())
, cpu_id(sched_getcpu()) {
}

//------------------------------------------------------------------------------

ReportWriter::ReportWriter(ReportSerializer& s)
: _serializer(s)
, _first_report(_serializer.GetNextReport(false))
, _current_report(&_first_report) {
    assert(IsClear(*_current_report));
}

ReportWriter::~ReportWriter() {
    _serializer.ReportFinished(_first_report);
}

//------------------------------------------------------------------------------

void ReportWriter::PrintDatumBoilerplate(const DatumParams& params) {
    // TODO(tmillican@google.com): Verify this doesn't allocate. It might fall
    //  under small-string optimization, but I don't know if that's guaranteed
    //  for stringstream or not.
    std::stringstream ts;
    ts << params.thread_id;

    static constexpr const char* fmt =
            R"({"issue_id":%d, "timestamp":%lld, "thread_id":"%s", "cpu_id":%d,)"
            R"("suite_id":"%s", "operation_id":"%s",custom:)";
    int result = snprintf(Buffer() + Offset(), BufLeft(),
                          fmt, params.issue_id, params.timestamp,
                          ts.str().c_str(), params.cpu_id, params.suite,
                          params.operation);
    if (result < 0) {
        FatalError(TAG, "Error '%d' encountered when trying to parse a string.",
                   result);
    } else if (result > BufLeft()) {
        // This should be the first thing we write, so we'd better have enough
        // space.
        FatalError(TAG, "Tried to write %d characters into a string that's only "
                        "has %z free characters left.",
                        result, (size_t)BufLeft());
    } else {
        Offset() += result;
    }
}

//------------------------------------------------------------------------------

void ReportWriter::Print(const char c) {
    if (BufLeft() == 0) {
        ExtendReport();
    }
    Buffer()[Offset()++] = c;
}

void ReportWriter::PrintData(const char* data, size_t data_left) {
    size_t data_off = 0;
    while (data_left != 0) {
        const auto to_copy = std::min(data_left, BufLeft());
        memcpy(Buffer() + Offset(), data + data_off, to_copy);
        data_off  += to_copy;
        data_left -= to_copy;
        Offset() += to_copy;
        if (data_left != 0) {
            ExtendReport();
        }
    }
}

//------------------------------------------------------------------------------

void ReportWriter::ExtendReport() {
    assert(_current_report->info.next == nullptr);
    if (_current_report->info.offset == 0) {
        // Probably an infinite loop, not good.
        FatalError(TAG, "Trying to extend an empty report???");
    }
    Log::D(TAG, "Extending report");
    _current_report = _current_report->info.next = &_serializer.GetNextReport(true);
    assert(IsClear(*_current_report));
}

//------------------------------------------------------------------------------

// Returns if we should try again.
bool ReportWriter::CheckResult(const std::to_chars_result result) {
    if (result.ec != std::errc{}) {
        Log::E(TAG, "Error '%d' encountered when calling ToChars.", (int)result.ec);
        return false;
    } else if (const auto newoff = (size_t)(result.ptr - Buffer()) ;
               newoff >= BufSz()) {
        ExtendReport();
        return true;
    } else {
        Offset() = newoff;
        return false;
    }
}

//==============================================================================

namespace {
    [[nodiscard]] constexpr auto NumReports(ReportFlushMode mode) noexcept {
        switch (mode) {
            case ReportFlushMode::Immediate:
                return kDefaultSmallReportQueueSize;
            case ReportFlushMode::Periodic:
            case ReportFlushMode::Manual:
                return kDefaultLargeReportQueueSize;
        }
    }

    // A bit weird API-wise, but passing in mode to avoid accidental typos.
    // This should really be a proper iterator type at some point...
    [[nodiscard]] constexpr int CircNext(int i, ReportFlushMode mode) noexcept {
        assert(0 <= i && i < NumReports(mode));
        if (++i == NumReports(mode))
            i = 0;
        return i;
    }

    constexpr void CircInc(int& i, ReportFlushMode mode) noexcept {
        i = CircNext(i, mode);
    }
}

//------------------------------------------------------------------------------

ReportSerializer::ReportSerializer(FILE* file, ReportFlushMode mode)
: _file(file)
, _mode(mode)
, _flush_period(kDefaultFlushPeriod)
, _reports(new PreallocatedReport[NumReports(_mode)]) {
    if (mode == ReportFlushMode::Periodic) {
        _periodic_thread = std::thread([this]() {
            while (_alive.load(std::memory_order_relaxed)) {
                const auto start_time = SteadyClock::now();
                Flush();
                const auto end_time = SteadyClock::now();
                const auto elapsed = end_time - start_time;
                const auto sleep_time = _flush_period - elapsed;
                if (sleep_time > Duration::zero()) {
                    std::this_thread::sleep_for(sleep_time);
                }
            }
        });
    }
}

//------------------------------------------------------------------------------

ReportSerializer::~ReportSerializer() {
    if (_periodic_thread.joinable()) {
        Log::V(TAG, "Stopping write thread...");
        _alive.store(false, std::memory_order_relaxed);
        _periodic_thread.join();
        Log::V(TAG, "...and done!");
    }

    Flush(true);
}

//------------------------------------------------------------------------------

void ReportSerializer::Flush(bool hard_flush) {
    // TODO(tmillican@google.com): Block new reports from starting during a hard
    //  flush?
    // TODO(tmillican@google.com): Swap buffers to avoid blocking further
    //  reports during a flush?

    do {
        std::scoped_lock lock{_report_mutex};
        while (_allocated_beg != _pending_end) {
            auto& report = _reports.get()[_allocated_beg];
            if (report.info.pending) {
                for (auto* rep = &report ; rep != nullptr ; ) {
                    fwrite(rep->buffer, sizeof(rep->buffer[0]), rep->info.offset,
                           _file);
                    auto& prev = *rep;
                    rep = rep->info.next;
                    Clear(prev);
                }
            }
            CircInc(_allocated_beg, _mode);
        }
    } while (hard_flush && _allocated_beg != _allocated_end);

    if (hard_flush) {
        fflush(_file);
    }
}

//------------------------------------------------------------------------------

PreallocatedReport& ReportSerializer::GetNextReport(bool append) {
    // TODO(tmillican@google.com): Slightly different logic for append w.r.t.
    //  errors, locks, etc.?

    PreallocatedReport* report;
    {
        std::scoped_lock lock{_report_mutex};

        // If the buffer is entirely used, wait for a slot to free up.
        // TODO(tmillican@google.com): Handle this more gracefully
        while (CircNext(_allocated_end, _mode) == _allocated_beg) {
            _report_mutex.unlock();
            Log::W(TAG, "Ran out of preallocated report slots! Consider "
                        "increasing the report rate or reporting less "
                        "frequently.");
            Flush();
            _report_mutex.lock();
        }

        report = &_reports.get()[_allocated_end];
        assert(IsClear(*report));
        CircInc(_allocated_end, _mode);
    }
    return *report;
}

//------------------------------------------------------------------------------

void ReportSerializer::ReportFinished(PreallocatedReport& base) {
    // If this report is the next claimed-but-unwritten one, we can advance the
    // pending iterator.
    bool advance_pending = false;
    {
        std::scoped_lock lock{_report_mutex};
        advance_pending = (_pending_end == (int)(&base - _reports.get()));

        for (auto* report = &base ; report != nullptr ; report = report->info.next) {
            report->info.pending = true;
        }
    }

    if (advance_pending) {
        while (_pending_end != _allocated_end) {
            auto& report = _reports.get()[_pending_end];
            if (!report.info.pending)
                break;
            CircInc(_pending_end, _mode);
        }

        if (_mode == ReportFlushMode::Immediate) {
            Flush();
        }
    }
}
