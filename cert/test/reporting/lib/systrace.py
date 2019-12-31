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
"""Functions for parsing systrace HTML files"""

import os
import re
import subprocess
import time

from pathlib import Path
from typing import Dict, List, Tuple

from bs4 import BeautifulSoup

from lib.build import APP_ID
from lib.common import Error

# ------------------------------------------------------------------------------
SYSTRACE_LINE_CPU_NUMBER_REGEXP = re.compile(r'(\[\d+\])')
SYSTRACE_LINE_TIMESTAMP_REGEXP = re.compile(r'(\d+\.?\d*:)')
CLOCK_SYNC_MARKER = "ancer::clock_sync"
CLOCK_SYNC_ISSUE_ID_REGEXP = re.compile(r'\(([0-9]+)\)')

# ------------------------------------------------------------------------------


class SystraceParseError(Error):
    """Exception raised when having trouble parsing a systrace

    Attributes:
        message -- explanation of the error
    """

    def __init__(self, message):
        super().__init__()
        self.message = message


# ------------------------------------------------------------------------------

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


def get_systrace_line_timestamp_ns(line: str) -> float:
    #pylint: disable=line-too-long
    """Get the systrace timestamp in nanoseconds from a systrace line,
    or 0 if not able
    So given the line:
    <...>-19979 (-----) [000] ...1 23495.016046: tracing_mark_write: C|19865|ancer::clock_sync(417)|23495016024390
    this would return int("23495.016046") * 1e9
    """
    match = SYSTRACE_LINE_TIMESTAMP_REGEXP.search(line)
    if not match.group():
        print(f"Unable to find systrace timestamp in line \"{line}\"")
        return 0

    return float(match.group()[:-1]) * 1e9


def get_ancer_clocksync_offset_ns(line: str) -> int:
    #pylint: disable=line-too-long
    """Given an ancer::clock_sync line, return the clock skew
    So given the line:
    <...>-19979 (-----) [000] ...1 23495.016046: tracing_mark_write: C|19865|ancer::clock_sync(417)|23495016024390
    This would return -21610 (nanoseconds)
    Returns:
        Nanoseconds of clock skew between the systrace clock and the
        clock used by ancer
    """

    # get the systrace timestamp for our clock_sync line
    st_timestamp_ns = get_systrace_line_timestamp_ns(line)

    # get the reference timestamp value from this line - it's the value after |
    last_pipe_pos = line.rfind('|')
    ref_timestamp = line[last_pipe_pos + 1:]
    ref_timestamp_ns = int(ref_timestamp)

    skew = ref_timestamp_ns - st_timestamp_ns
    return skew


def find_timestamp_offset(lines: List[str]) -> Tuple[int, int]:
    """Given a systrace data section split to a line list,
    find the first ancer clock sync marker and determine the clock offset
    in nanoeseconds
    """
    for line in lines:
        if CLOCK_SYNC_MARKER in line:
            return get_ancer_clocksync_offset_ns(line)

    print(f"Couldn't find {CLOCK_SYNC_MARKER} entry in systrace")
    return None


# ------------------------------------------------------------------------------


def filter_systrace_to_interested_lines(systrace_file: Path,
                                        keywords: List[str],
                                        pattern: str) -> Tuple[int, List[str]]:
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

    with open(systrace_file, 'rb') as file:
        file_data = file.read()
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

    # get our clock sync data
    timestamp_offset_ns = find_timestamp_offset(trace_tag_lines)

    if keywords:
        # filter to lines which interest the caller
        def _f(line: str) -> bool:
            for keyword in keywords:
                if keyword in line:
                    return True
            return False

        return timestamp_offset_ns, \
                [line for line in trace_tag_lines if _f(line)]

    if pattern:
        regexp = re.compile(pattern)
        return timestamp_offset_ns, \
            [line for line in trace_tag_lines if regexp.search(line)]

    return timestamp_offset_ns, []


def convert_systrace_line_to_datum(systrace_line: str,
                                   timestamp_offset_ns: int) -> Dict:
    #pylint: disable=line-too-long
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
    timestamp_match = SYSTRACE_LINE_TIMESTAMP_REGEXP.search(systrace_line)
    if not timestamp_match:
        raise SystraceParseError("Can't find timestamp in systrace line")

    message = systrace_line[timestamp_match.end():].strip()
    message_break = message.find(":")

    first_comma = systrace_line.find("(")
    pid_break = systrace_line.rfind("-", 0, first_comma)
    pid = int(systrace_line[pid_break + 1:first_comma - 1])

    task_name = systrace_line[:pid_break].strip()

    cpu_number_match = SYSTRACE_LINE_CPU_NUMBER_REGEXP.search(systrace_line)
    if not cpu_number_match:
        raise SystraceParseError("Can't find cpu_id in systrace line")

    cpu_id = int(cpu_number_match.group()[1:-1])
    suite_id = "systrace"

    systrace_timestamp = float(timestamp_match.group()[:-1]) * 1e9
    timestamp = systrace_timestamp - timestamp_offset_ns
    suite_id = "systrace"
    operation_id = task_name
    thread_id = pid  # TODO: PID != TID, so where does this belong
    cpu_id = int(cpu_number_match.group()[1:-1])
    token = message[:message_break].strip()
    value = message[message_break + 1:].strip()

    return {
        "issue_id": -1,  # systrace markers don't have and issue_id
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


# ------------------------------------------------------------------------------


class LocalSystrace(object):
    """Helper to start a local (over USB) systrace and blocks until ready
    """

    def __init__(self, device_id: str, dst_file: Path, categories: List[str]):
        """Start systrace on a device, blocks until systrace is ready"""
        self._trace_file = dst_file

        # run_command(f'adb shell "setprop debug.atrace.app_number 1"')
        # run_command(f'adb shell "setprop debug.atrace.app_0 {APP_ID}"')

        cmd = [
            "python2",
            os.path.expandvars(
                "$ANDROID_HOME/platform-tools/systrace/systrace.py"),
            "-e",
            device_id,
            "-a",
            APP_ID,
            "-o",
            str(dst_file),
        ] + categories

        print(f"LocalSystrace::__init__ - Starting systrace for {APP_ID}")
        self.process = subprocess.Popen(cmd,
                                        stdout=subprocess.PIPE,
                                        stdin=subprocess.PIPE,
                                        shell=False)

        # TODO(shamyl@gmail.com): Find a way to capture "Starting
        # tracing (stop with enter)"
        # attempting to run the loop below blocks on the first call to
        # readline()
        # while True:
        #     line = self.process.stdout.readline().decode("utf-8")
        #     if "stop with enter" in line:
        #         break

        time.sleep(10)

    def finish(self):
        """Stop the running systrace, blocking until its complete"""
        # systrace ends when it receives an "enter" input
        print(f"LocalSystrace::finish - Stopping systrace for {APP_ID}...")
        self.process.stdin.write(b"\n")
        self.process.stdin.close()
        self.process.wait()
        print("LocalSystrace::finish - done")
        return self._trace_file
