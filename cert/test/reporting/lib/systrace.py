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

import json
import re

from pathlib import Path
from typing import Dict, List, Tuple

from bs4 import BeautifulSoup


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


SYSTRACE_LINE_TIMESTAMP_REGEXP = re.compile(r'(\d+\.?\d*:)')

def get_systrace_line_timestamp_ns(line:str)->float:
    match = SYSTRACE_LINE_TIMESTAMP_REGEXP.search(line)
    if not match.group():
        print(f"Unable to find systrace timestamp in line \"{line}\"")
        return 0

    return float(match.group()[:-1]) * 1e9


def find_timestamp_offset(lines: List[str], trace_events: Dict) -> int:
    # first get our clock sync data
    clock_sync_marker_line = None
    for line in lines:
        if "trace_event_clock_sync:" in line:
            clock_sync_marker_line = line
            break

    if not clock_sync_marker_line:
        print("Couldn't find a trace_event_clock_sync: entry in systrace")
        return 0

    # get the systrace timestamp for the FIRST systrace line
    first_st_timestamp_ns = get_systrace_line_timestamp_ns(lines[0])
    if first_st_timestamp_ns == 0:
        return 0

    # get the systrace timestamp for the clock sync marker line
    st_timestamp_ns = get_systrace_line_timestamp_ns(clock_sync_marker_line)
    if st_timestamp_ns == 0:
        return 0

    if "parent_ts=" in clock_sync_marker_line:
        # parse the marker to get the offset
        # marker looks something like:
        # <...>-26895 (-----) [001] ...1 518315.218945: tracing_mark_write: trace_event_clock_sync: parent_ts=516807.750000
        # first extract the parent_ts value and convert from
        # seconds to nanoseconds
        parent_ts = clock_sync_marker_line[clock_sync_marker_line.
                                           rfind("parent_ts"):].strip()
        parent_ts_ns = float(parent_ts.split("=")[1]) * 1e9

        # get the offset
        timestamp_offset_ns = int(parent_ts_ns) - int(st_timestamp_ns)
        return timestamp_offset_ns

    elif "name=" in clock_sync_marker_line:
        # some systraces have a json block with the timestamp in it (traceEvents)
        # attempt to find the issue_ts with matching sync id
        # TODO(shamyl@google.com): Work out how the skew is computed; see
        # https://github.com/JetBrains/android/blob/master/profilers/src/com/android/tools/profilers/cpu/atrace/AtraceParser.java#L514
        # https://source.chromium.org/chromium/chromium/src/+/master:third_party/catapult/common/py_trace_event/py_trace_event/trace_event.py;l=113?q=issue_ts%20f:%5C.py&ss=chromium&originalUrl=https:%2F%2Fcs.chromium.org%2F

        delta = first_st_timestamp_ns - st_timestamp_ns

        clock_sync_name = clock_sync_marker_line[clock_sync_marker_line.
                                                 rfind("name"):].strip()
        clock_sync_token = clock_sync_name.split("=")[1]

        ts = None
        for event in trace_events["traceEvents"]:
            if "name" in event and event["name"] == "clock_sync":
                if "args" in event:
                    if event["args"]["sync_id"] == clock_sync_token:
                        ts = event["args"]["issue_ts"]
                if not ts and "ts" in event:
                    ts = event["ts"]

        if ts:
            ts *= 1e6 # from milli to nanos
            print(f"ts: {ts} delta: {delta}")
        return 0


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
    trace_tag_lines = trace_tag.string.split('\n')
    trace_tag_lines = [
        line.strip()
        for line in trace_tag_lines
        if line.strip() and line[0] != '#'
    ]

    # also find the traceEvents tag
    trace_events = None
    for tag in script_tags:
        if "\"traceEvents\"" in tag.string:
            try:
                trace_events = json.loads(tag.string)
                break
            except:
                pass

    # get our clock sync data
    timestamp_offset_ns = find_timestamp_offset(trace_tag_lines, trace_events)

    if keywords:
        # filter to lines which interest the caller
        def _f(line: str) -> bool:
            for kw in keywords:
                if kw in line:
                    return True
            return False

        return timestamp_offset_ns, list(filter(_f, trace_tag_lines))
    elif pattern:
        regexp = re.compile(pattern)
        return timestamp_offset_ns, list(
            filter(lambda l: regexp.search(l) is not None, trace_tag_lines))

    return timestamp_offset_ns, []


def convert_systrace_line_to_datum(systrace_line: str,
                                   timestamp_offset_ns: int) -> Dict:
    """Convert a systrace line to a report datum, suitable
    for merging into datum stream

    Args:
        systrace_line: line from a systrace data segment
        timestamp_offset_ns: time offset to apply for clock sync

    Returns:
        Dict in format of report datum

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

    first_comma = systrace_line.find("(")
    pid_break = systrace_line.rfind("-", 0, first_comma)
    pid = int(systrace_line[pid_break + 1:first_comma - 1])

    task_name = systrace_line[:pid_break].strip()

    cpu_number_match = cpu_number_regexp.search(systrace_line)
    if not cpu_number_match:
        raise "Can't find cpu_id in systrace line"

    cpu_id = int(cpu_number_match.group()[1:-1])
    suite_id = "systrace"

    systrace_timestamp = float(timestamp_match.group()[:-1]) * 1e9
    timestamp = systrace_timestamp + timestamp_offset_ns
    suite_id = "systrace"
    operation_id = task_name
    thread_id = pid  # TODO: PID != TID, so where does this belong
    cpu_id = int(cpu_number_match.group()[1:-1])
    token = message[:message_break].strip()
    value = message[message_break + 1:].strip()

    return {
        "timestamp": int(timestamp),
        "suite_id": suite_id,
        "operation_id": operation_id,
        "thread_id": thread_id,
        "cpu_id": cpu_id,
        "custom": {
            token: value,
            "unadjusted_systrace_timestamp": systrace_timestamp
        }
    }
