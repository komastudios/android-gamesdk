#
# Copyright 2019 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

import re

from pathlib import Path
from typing import List, Tuple

from bs4 import BeautifulSoup

from lib.csv import CSVEntry


def trim_to_trace(file_data):
    """Return the trace data portion of the systrace file.
    Args:
        file_data: The contents of a systrace html file
    Returns:
        substring of file_data
    """
    start_marker = b'<!-- BEGIN TRACE -->'
    end_marker = b'<!-- END TRACE -->'
    start_pos = file_data.index(start_marker) + len(start_marker)
    end_pos = file_data.index(end_marker)
    return file_data[start_pos:end_pos]


def filter_systrace_to_interested_lines(systrace_file: Path,
                                        keywords: List[str] = [],
                                        pattern: str = None
                                       ) -> Tuple[int, List[str]]:
    """Trims contents of a systrace file to subset in which we are interested

    Given the systrace HTML file `systrace_file`, extract the systrace entries 
    which contain the specified keywords or match the specified regex pattern
    Returns tuple of time offset in nanoseconds for systrace log timestamps to 
    the clock used in our json report logs, and the list of
    lines which match the query

    Args:
        systrace_file: Path to a systrace html file
        keywords: (optional) List of keywords to look for in the file
        pattern: (optional) Regex for finding lines we are interested in

    Returns: 
        A tuple of a time offset for clock alignment, and list of lines from the
        systrace html data which matched the search

    Note:
        If both keywords are pattern are empty, this will return an empty
        list of results

    """

    with open(systrace_file, 'rb') as fp:
        file_data = fp.read()
    trace_data = trim_to_trace(file_data)

    soup = BeautifulSoup(trace_data, 'lxml')
    script_tags = soup.find_all('script', attrs={'class': 'trace-data'})

    # The number of trace-data script tags apparently depends on device model.
    # If there's more than one, we look at the longest one because probably
    # (hopefully) that's where the actual trace is.
    trace_tag = max([(len(tag.string), tag) for tag in script_tags])[1]
    data = trace_tag.string.split('\n')
    data = [line.strip() for line in data if line.strip() and line[0] != '#']

    # first get our clock sync data
    clock_sync_marker_line = None
    for line in data:
        if "trace_event_clock_sync: parent_ts=" in line:
            clock_sync_marker_line = line
            break

    if clock_sync_marker_line:
        # parse the marker to get the offset
        # marker looks something like:
        # <...>-26895 (-----) [001] ...1 518315.218945: tracing_mark_write: trace_event_clock_sync: parent_ts=516807.750000
        # first extract the parent_ts value and convert from
        # seconds to nanoseconds
        parent_ts = clock_sync_marker_line[clock_sync_marker_line.
                                           rfind("parent_ts"):].strip()
        parent_ts_ns = float(parent_ts.split("=")[1]) * 1e9

        # get the systrace timestamp and convert from seconds to nanos
        st_timestamp_regexp = re.compile(r'(\d+\.?\d*:)')
        st_timestamp_match = st_timestamp_regexp.search(clock_sync_marker_line)
        if not st_timestamp_match.group():
            print(
                "Unable to find systrace timestamp in trace_event_clock_sync line"
            )
            exit(1)

        st_timestamp_ns = float(st_timestamp_match.group()[:-1]) * 1e9

        # get the offset
        timestamp_offset_ns = int(parent_ts_ns) - int(st_timestamp_ns)
    else:
        print(
            f"Unable to find \"trace_event_clock_sync: parent_ts\" clock"
            f"sync marker in trace({systrace_file}); timestamps may be misaligned"
        )
        timestamp_offset_ns = 0

    if keywords:
        # now filter to lines which interest the caller
        def _f(line: str) -> bool:
            for kw in keywords:
                if kw in line:
                    return True
            return False

        return timestamp_offset_ns, list(filter(_f, data))
    elif pattern:
        regexp = re.compile(pattern)
        return timestamp_offset_ns, list(
            filter(lambda l: regexp.search(l) is not None, data))

    return timestamp_offset_ns, []


def convert_systrace_line_to_csv(systrace_line: str,
                                 timestamp_offset_ns: int) -> CSVEntry:
    """Convert a systrace line to a CSVEntry

    Parses a systrace line into a CSVEntry, passing the domain-specific 
    report as the csventry value field

    Args:
        systrace_line: line from a systrace data segment
        timestamp_offset_ns: time offset to apply for clock sync

    Returns:
        CSVEntry suitable for merging into our CSV report data

    """
    # lines look something like:
    # <...>-26895 (-----) [001] .... 518315.221509: sched_process_exit: comm=atrace pid=26895 prio=120
    # surfaceflinger-664   (  664) [003] ...1 518317.537935: tracing_mark_write: B|664|handleMessageInvalidate
    cpu_number_regexp = re.compile(r'(\[\d+\])')
    timestamp_regexp = re.compile(r'(\d+\.?\d*:)')
    timestamp_match = timestamp_regexp.search(systrace_line)
    if not timestamp_match:
        raise "Can't find timestamp in systrace line"

    message = systrace_line[timestamp_match.end():].strip()
    message_break = message.find(":")

    task_name_break = systrace_line.find("-")
    task_name = systrace_line[:task_name_break].strip()

    pid_break = systrace_line.find(" ", task_name_break + 1)
    pid = int(systrace_line[task_name_break + 1:pid_break])

    cpu_number_match = cpu_number_regexp.search(systrace_line)
    if not cpu_number_match:
        raise "Can't find cpu_id in systrace line"

    cpu_id = int(cpu_number_match.group()[1:-1])
    suite_id = "systrace"

    timestamp = float(timestamp_match.group()[:-1]) * 1e9 + timestamp_offset_ns
    suite_id = "systrace"
    operation_id = task_name
    thread_id = pid  # TODO: PID != TID, so where does this belong
    cpu_id = int(cpu_number_match.group()[1:-1])
    token = message[:message_break].strip()
    value = message[message_break + 1:].strip()

    return CSVEntry(timestamp=timestamp,
                    suite_id=suite_id,
                    operation_id=operation_id,
                    thread_id=thread_id,
                    cpu_id=cpu_id,
                    token=token,
                    value=value)
