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

#pragma once

#include <string>

#include "util/Time.hpp"
#include "util/Json.hpp"

namespace ancer::reporting {
    struct Datum {
        std::string suite_id;
        std::string operation_id;
        Timestamp timestamp;
        std::string thread_id;
        int cpu_id = 0;
        Json custom;
    };

    JSON_WRITER(Datum) {
        JSON_REQVAR(suite_id);
        JSON_REQVAR(operation_id);
        JSON_REQVAR(timestamp);
        JSON_REQVAR(thread_id);
        JSON_REQVAR(cpu_id);
        JSON_REQVAR(custom);
    }

    enum class ReportFlushMode {
        /**
         * Writes to the report log will be immediately written and flushed
         */
                Immediate,
        /**
         * Writes to the report log will be periodically flushed. See SetPeriodicFlushModePeriod()
         */
                Periodic,
        /**
         * Writes will only be flushed when FlushReportLogQueue() is called
         */
                Manual
    };

    /**
     * Opens the specified file for writing Report data
     * @param file the file to which we want to append report data
     */
    void OpenReportLog(const std::string& file);

    /**
     * Close the currently open report file. Any calls to WriteReport()
     * without re-opening will terminate.
     */
    void CloseReportLog();

    /**
     * Set the flushing mode for the report writer thread
     * @param mode mode to use
     */
    void SetReportLogFlushMode(ReportFlushMode mode);
    ReportFlushMode GetReportLogFlushMode();

    /**
     * Set the flush period for when flush mode is ReportFlushMode::PERIODIC
     * @param duration the interval in seconds between flushing the report queue
     */
    void SetPeriodicFlushModePeriod(Duration duration);
    Duration GetPeriodicFlushModePeriod() noexcept;

    /**
     * Write a datum to the report log
     * @param d the datum to write
     */
    void WriteToReportLog(const Datum& d);

    /**
     * Write a string to the report log
     * @param s the string to write
     */
    void WriteToReportLog(const std::string& s);

    /**
     * Immediately flush any pending writes to the report log
     */
    void FlushReportLogQueue();
} // namespace ancer::reporting
