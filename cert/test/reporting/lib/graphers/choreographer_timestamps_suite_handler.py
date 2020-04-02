#
# Copyright 2020 The Android Open Source Project
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
"""Provides VulkanVaryingsHandler which processes report data
from the Vulkan Varyings test
"""

import os
import re
from typing import List, Optional

import matplotlib.pyplot as plt
import numpy as np

from lib.graphers.suite_handler import SuiteHandler
from lib.report import Suite

import lib.systrace as systrace

# TODO(baxtermichael@google.com): remove
def readable_time(t: int) -> str:
    s = str(t)
    s = s[::-1] # reverse
    ns = ''
    i = 0
    for c in s:
        ns += c
        i += 1
        if i > 2:
            ns += ','
            i = 0
    return ns[::-1]


def binsearch_nearest(a: List[int], x: int) -> Optional[int]:
    """
    Given a sorted list of integers `a` and some target value `x`, returns the
    index of `x`, or else the index of the value closest to `x`. Returns None if
    the list is empty.
    """
    if not a:
        return None
    if len(a) == 1:
        return 0

    low = 0
    high = len(a) - 1
    mid = 0

    while low <= high:
        if high - low == 1:
            break # only two values left, break to check for closest

        # The ordinary approach to binary search would have us set
        # `high = mid - 1` or `low = mid + 1`,
        # but by throwing away the +/- 1, we wind up with either an exact match
        # or the two closest values.
        mid = int((low + high) / 2)
        if x < a[mid]:
            high = mid
        elif x > a[mid]:
            low = mid
        else:
            return mid

    if abs(x - a[low]) < abs(x - a[high]):
        return low
    return high


class ChoreographerTimestampsSuiteHandler(SuiteHandler):
    """
    """
    def __init__(self, suite):
        super().__init__(suite)

        # Get systrace file so we can parse it
        self.systrace_path = self.get_systrace_path(suite)
        self.trace_offset_ns = self.read_clock_sync()

        if not self.trace_offset_ns:
            print("We weren't able to get a clock sync!")
            return

        self.trace_lines = []
        self.timestamps_raw = []
        self.timestamps_conv = []
        self.read_systrace_file()

        # Print matched times for visual inspection.
        print(f"Choreographer                : delta    :      VSYNC-app (converted)        :        VSYNC-app (original)")
        for d in self.data:
            ts = d.custom.get('timestamp')
            if ts:
                sys_ts_idx = binsearch_nearest(self.timestamps_conv, ts)
                sys_ts = self.timestamps_conv[sys_ts_idx]
                sys_ts_og = self.timestamps_raw[sys_ts_idx]
                # TODO(baxtermichael@google.com): Check for None type.
                print(f"{readable_time(ts)}        : {readable_time(sys_ts - ts)} :        {readable_time(sys_ts)}        :        {readable_time(sys_ts_og)}")


    def get_systrace_path(self, suite: Suite) -> Optional[str]:
        report_dir = os.path.dirname(suite.file)
        report_name = os.path.basename(suite.file)
        systrace_name = report_name.replace(".json", "_trace.html")
        systrace_path = os.path.join(report_dir, systrace_name)

        if not os.path.exists(systrace_path):
            print(f"Looking for {self.systrace_path} but it doesn't exist.")
            return None

        return systrace_path


    def read_clock_sync(self):
        with open(self.systrace_path) as f:
            for line in f:
                if systrace.CLOCK_SYNC_MARKER in line:
                    return systrace.get_ancer_clocksync_offset_ns(line)
            return None

    def read_systrace_file(self):
        vsync_pattern = re.compile(r'\s(\d+\.?\d*):\s.*VSYNC-app')
        with open(self.systrace_path) as f:
            for line in f:
                match = vsync_pattern.search(line)
                if match:
                    self.trace_lines.append(line)
                    ts = systrace.get_systrace_timestamp_ns(line)
                    self.timestamps_raw.append(ts)
                    self.timestamps_conv.append(ts + self.trace_offset_ns)


    @classmethod
    def can_handle_suite(cls, suite: Suite):
        return 'Choreographer Timestamps' in suite.name

    @classmethod
    def can_render_summarization_plot(cls, suites: List['Suite']) -> bool:
        return False

    @classmethod
    def render_summarization_plot(cls, suites: List['Suite']) -> str:
        return None

    @classmethod
    def handles_entire_report(cls, suites: List['Suite']):
        return False

    @classmethod
    def render_report(cls, raw_suites: List['SuiteHandler']):
        return ''

    def render_plot(self) -> str:
        return ''
