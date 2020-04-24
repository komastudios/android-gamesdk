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
"""
A grapher for the Choreographer Timestamps operation.
"""

import os
import re
from typing import List, Optional

import matplotlib.pyplot as plt
from matplotlib.patches import Rectangle
import numpy as np

from lib.graphers.suite_handler import SuiteHandler, SuiteSummarizer
from lib.report import Datum, SummaryContext, Suite
import lib.summary_formatters.format_items as fmt
import lib.systrace as systrace

class ChoreographerTimestampsSummarizer(SuiteSummarizer):
    """Suite summarizer for Choreographer Timestamps test."""
    @classmethod
    def default_handler(cls) -> SuiteHandler:
        return ChoreographerTimestampsSuiteHandler

    @classmethod
    def can_handle_datum(cls, datum: Datum):
        return "ChoreographerTimestampsOperation" in datum.operation_id


class ChoreographerTimestampsSuiteHandler(SuiteHandler):
    """
    Implements SuiteHandler for Choreographer Timestamps test results.
    """

    def __init__(self, suite):
        super().__init__(suite)

        self.timestamp_variance = []

        systrace_path = self.get_systrace_path(suite)
        if not os.path.exists(systrace_path):
            print(f"Looking for {systrace_path} but it doesn't exist.")
            return

        offset_ns = self.read_clock_sync_offset_ns(systrace_path)
        if not offset_ns:
            print("We weren't able to get a clock sync!")
            return

        systrace_timestamps = self.get_systrace_timestamps(systrace_path, \
            offset_ns)

        # Calculate variance (how much Choreographer timestamps varied from
        # systrace timestamps)
        for row in self.data:
            timestamp = row.custom.get('timestamp')
            if timestamp:
                index = binary_search_nearest(systrace_timestamps, \
                    timestamp)
                sys_ts = systrace_timestamps[index]
                self.timestamp_variance.append(sys_ts - timestamp)


    def get_systrace_path(self, suite: Suite) -> str:
        """
        Returns the expected path of the systrace file, given the report file
        path from suite.
        """
        report_dir = os.path.dirname(suite.file)
        report_name = os.path.basename(suite.file)
        systrace_name = report_name.replace(".json", "_trace.html")
        systrace_path = os.path.join(report_dir, systrace_name)
        return systrace_path


    def read_clock_sync_offset_ns(self, path: str) -> Optional[int]:
        """
        Looks for the clock sync marker in the file at `path` and returns the
        nanosecond offset that can be added to a systrace timestamp to convert
        it relative to the report file's timestamps. Returns None if the clock
        sync marker was not present in the file.
        """
        with open(path) as file:
            for line in file:
                if systrace.CLOCK_SYNC_MARKER in line:
                    return systrace.get_ancer_clocksync_offset_ns(line)
            return None


    def get_systrace_timestamps(self, path: str, offset_ns: int) -> [int]:
        """
        Returns a list of timestamps for "VSYNC-app" traces in the file at
        `path`, adding `offset_ns` nanoseconds to each timestamp. (This can be
        used to make them relative to the report file's timestamps.)
        """
        timestamps = []
        with open(path) as file:
            vsync_pattern = re.compile(r'\s(\d+\.?\d*):\s.*VSYNC-app')
            for line in file:
                match = vsync_pattern.search(line)
                if match:
                    timestamp = systrace.get_systrace_timestamp_ns(line)
                    if timestamp:
                        timestamps.append(timestamp + offset_ns)
        return timestamps

    def render_report(self, ctx: SummaryContext) -> List[fmt.Item]:
        image = fmt.Image(self.plot(ctx, self.render_plot), self.device())
        return [image]

    def render_plot(self):
        """
        We create a plot showing how much the Choreographer timestamps varied
        from the VSYNC values gather from systrace. The graph has an upper and
        lower bound, such that if a Choreographer timestamp varied from a
        systrace timestamp by enough to pass the upper or lower bound, we
        consider the device as failing the test, and the plot will be marked
        accordingly.
        """

        fig, axes = plt.subplots()
        fig.suptitle(self.title())

        # Convert to milliseconds
        variance_ms = [i / 1_000_000 for i in self.timestamp_variance]

        # Set bounds
        max_y = 2.0
        min_y = -max_y
        max_x = len(variance_ms)
        axes.axis([0, max_x, min_y, max_y])
        axes.set_xlabel("Frame")
        axes.set_ylabel("Timestamp Variance (ms)")

        upper_bound = 1.0
        lower_bound = -upper_bound

        # Line for upper bound
        axes.axhline(y=upper_bound, \
            linestyle='dashed', \
            linewidth=1, \
            color='black')

        # Line for lower bound
        axes.axhline(y=lower_bound, \
            linestyle='dashed', \
            linewidth=1, \
            color='black')

        # Plot points
        axes.plot(variance_ms)

        # Set PASS/FAIL indicators
        lowest_val = min(variance_ms)
        highest_val = max(variance_ms)

        def add_region_marker(label: str, y: int, height: int, color: str):
            color_indicator = Rectangle((0, y - height / 2), \
                max_x, height, \
                facecolor=color, alpha=0.5)
            axes.add_patch(color_indicator)

            axes.text(max_x / 2, y, \
                label, \
                fontsize=24, \
                alpha=0.75, \
                horizontalalignment='center', \
                verticalalignment='center')

        if lowest_val >= lower_bound and highest_val <= upper_bound:
            add_region_marker('PASS', 0.0, upper_bound - lower_bound, 'green')

        if lowest_val < lower_bound:
            add_region_marker('FAIL', (min_y + lower_bound) / 2, lower_bound - min_y, 'red')

        if highest_val > upper_bound:
            add_region_marker('FAIL', (max_y + upper_bound) / 2, max_y - upper_bound, 'red')


def binary_search_nearest(values: List[int], target: int) -> Optional[int]:
    """
    Given a sorted list of integers, `values`, and some `target` value, returns
    the index of `target`, or else the index of the value closest to `target`.
    Returns None if the list is empty.
    """
    if not values:
        return None

    low = 0
    high = len(values) - 1
    mid = 0

    # We modify the traditional binary search so that it closes in around a
    # value, rather than only finding an exact match. Once we narrow the list
    # down to two values, we can directly compare them.
    while high - low > 1:
        mid = int((low + high) / 2)

        # Usually binary search would set `high = mid - 1` or `low = mid + 1`,
        # but we can't be so quick to rule out the `mid` value just because
        # it wasn't an exact match--it may wind up being the closest value.
        if target < values[mid]:
            high = mid
        elif target > values[mid]:
            low = mid
        else:
            return mid

    if abs(target - values[low]) < abs(target - values[high]):
        return low
    return high
