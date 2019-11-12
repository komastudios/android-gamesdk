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

import numpy as np

from lib.csv import CSVEntry, time_units_per_ns
import re as regex
from typing import Any, List, Callable

NS_PER_S = 1e9
MS_PER_S = 1e3
NS_PER_MS = 1e6

NANOSEC_EXPRESSION = "^(\d+) (nanoseconds|ns|nsec)$"


def to_float(v: Any) -> float:
    try:
        return float(v)
    except ValueError:
        if v == "True":
            return 1
        elif v == "False":
            return 0
        else:
            match = regex.search(NANOSEC_EXPRESSION, v)
            if match is not None and \
                len(match.groups()) > 1:
                return float(match.group(1))

    return None


def to_ns_duration(v: str):
    for k in time_units_per_ns:
        pos = v.find(k)
        if pos >= 0:
            head = v[:pos].strip()
            head_numeric = to_float(head)
            if head_numeric is not None:
                return head_numeric / time_units_per_ns[k]

    return None


class Datum:

    def __init__(self, csv_entry: CSVEntry, timestamp: int, operation: str,
                 field: str, value: str):
        self.csv_entry = csv_entry
        self.timestamp = timestamp
        self.operation = operation
        self.field = field
        self.value = value

        # now attempt to parse a numeric and duration values
        self.numeric_value = to_float(self.value)
        self.duration_ns_value = to_ns_duration(self.value)


class Chart:

    def __init__(self, suite_id: str, operation_id: str, field: str,
                 data: List[Datum]):
        self.suite_id = suite_id
        self.operation_id = operation_id
        self.field = field
        self.data = data

    def set_renderer(self, r):
        self.renderer = r

    def is_event_chart(self):
        return self.renderer.is_event_chart()

    def plot(self, fig, index, count, start_time_seconds, end_time_seconds):
        self.renderer.plot(fig, index, count, start_time_seconds,
                           end_time_seconds)


class Suite:

    def __init__(self, suite_name: str, entries: List[CSVEntry]):
        self.suite_name = suite_name

    def topics(self):
        """Returns the topics repsented by this suite
        Returns:
            List of tuple of (operation id, field name)
        """
        pass

    def analyze(self, cb: Callable[[Any, Datum, str], None]):
        """Performs an analysis of the contents of the suite

        For each "warning" or outlier bit of data invokes the callback
        passing the suite, the datum, and a string description of the event

        This is kind of a strawman implementation, currently being used
        to find outliers in suite data.

        TODO(shamyl@google.com): Think hard about the purpose here and possibly
        refactor into passing some kind of warning/outlier/other token in the
        callback

        Returns:
            the count of warnings emitted during analysis
        """
        if self.handler:

            def _cb(s, d, m):
                print(f"Suite[{s.suite_name}]::analyze msg: {m}")
                cb(s, d, m)

            return self.handler.warn(_cb)
        return None

    def plot(self, title: str, fields: List[str] = [], skip: List[str] = []):
        """Plot the data in the suite
        Args:
            title: The title to display
            fields: If not empty, only those charts whos field names are in
                the fields array will be rendered
            skip: Any charts whos field names are in this list will be skipped
        """
        pass


class ChartRenderer(object):
    # All = "ALL"

    def __init__(self, chart: Chart):
        self.chart = chart
        self.data = chart.data

    # TODO(shamyl@google.com): Implement field merging in SuiteImpl
    # def fields_to_merge(self) -> List[List[str]]:
    #     """Return a list of fields which should be drawn atop the same figure
    #     Return None to draw each field separately in a dedicated row in the figure.
    #     For example, returning the following would cause those the two charts
    #     with these fields to be drawn in a single row:
    #         [
    #             ["sys_mem_info.available_memory", "sys_mem_info_oom_score"],
    #         ]
    #     Return self.All to merge all fields into one row
    #     """
    #     return None

    def is_event_chart(self):
        return False

    def plot(self, fig, index, count, start_time_seconds, end_time_seconds):
        """Render this chart's data into the currently active matplotlib figure

        Args:
            fig: the active matplotlib figure
            index: a suite may have several charts; index is the index of this renderer in the list
            count: the number of charts in the suite
            start_time_seconds: time in seconds of the first datum in the suite
            end_time_seconds: time in seconds of the last datum in the suite
        """
        pass

    def get_xs(self) -> List[int]:
        return np.array(list(map(lambda d: d.timestamp, self.data)))

    def get_x_axis_as_seconds(self):
        xs = self.get_xs()
        return (xs - xs[0]) / NS_PER_S

    def get_ys(self) -> List[float]:
        return np.array(list(map(lambda d: d.numeric_value, self.data)))


class SuiteHandler(object):

    def __init__(self, suite: Suite):
        self.suite = suite

    @classmethod
    def can_handle_suite(cls, suite: Suite):
        """Check if this SuiteHandler class can be used to handle this
        suite's data
        Args:
            suite: The suite in question
        Returns:
            True if this SuiteHandler class should handle
                the contents of the provided Suite instance
        """
        return False

    def assign_renderer(self, chart: Chart):
        """Assign an appropriate ChartRenderer implementation to `chart`
        Return true if a renderer was found, otherwise False
        """
        return False

    def analyze(self, cb: Callable[[Any, Datum, str], None]):
        """Analyze the suite's data and report any warnings
        Args:
            cb: callback invoked for each "warning" data point, passing
                the suite, the datum, and a str describing the finding
        """
        pass
