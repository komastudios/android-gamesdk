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

#include <charconv>
#include <string>

#include "util/Time.hpp"


namespace ancer::reporting {
    enum class ReportFlushMode {
        // Writes will be flushed as quickly as possible.
                Immediate,
        // Writes will be periodically flushed.
                Periodic,
        // Writes will only be flushed when FlushReportLogQueue() is called
                Manual
    };

    // Opens the report log using the specified file.
    void OpenReportLog(const std::string& file);
    void OpenReportLog(int file_descriptor);

    // Set the flush mode for the reporter.
    void SetReportLogFlushMode(ReportFlushMode mode);
    ReportFlushMode GetReportLogFlushMode();

    // Set the flush period for when we're in periodic mode.
    void SetPeriodicFlushModePeriod(Duration duration);
    Duration GetPeriodicFlushModePeriod() noexcept;

    // Write a datum to the report log.
    template <typename T>
    void WriteToReportLog(const std::string& suite, const std::string& operation,
                          T&& datum);
    template <typename T>
    void WriteToReportLog(const char* suite, const char* operation, T&& datum);

    // Write a string to the report log.
    void WriteToReportLog(std::string&& s);

    // Immediately flush any pending writes to the report log.
    void FlushReportLogQueue();

    // Performs a "hard" flush of the log queue, waiting on any pending reports
    // and flushing the file as well.
    void HardFlushReportLogQueue();
}


#include "Reporting.inl"
