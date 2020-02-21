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
"""Provides a suite handler that graphs temperature, memory and other stress
test runtime aspects.
"""

from typing import List

import matplotlib.pyplot as plt
import numpy as np

from lib.common import nanoseconds_to_seconds
from lib.graphers.suite_handler import SuiteHandler
from lib.report import Datum, Suite


class MonitorSuiteHandler(SuiteHandler):
    """Grapher for test non-functional aspects like memory or FPS."""

    def __init__(self, suite: Suite):
        super().__init__(suite)

        x_axis_as_seconds = self.get_x_axis_as_seconds()
        self.start_time_seconds = x_axis_as_seconds[0]
        self.end_time_seconds = x_axis_as_seconds[-1]
        self.x_axis = []

        self.memory_allocated = []
        self.memory_available = []

    @classmethod
    def can_handle_suite(cls, suite: Suite):
        return True  # Just handles all

    @classmethod
    def can_render_summarization_plot(cls, suites: List['Suite']) -> bool:
        return False

    @classmethod
    def render_summarization_plot(cls, suites: List['Suite']) -> str:
        return None

    def filter_timestamp(self, datum: Datum) -> type(None):
        """"""
        self.x_axis.append(
            nanoseconds_to_seconds(datum.timestamp) - self.start_time_seconds)

    def filter_memory_data(self, datum: Datum) -> type(None):
        """Appends datum memory info to the proper vectors to graph."""
        self.memory_allocated.append(
            datum.get_custom_field_numeric('memory_state.native_allocated'))
        self.memory_available.append(
            datum.get_custom_field_numeric('memory_state.available_memory'))

    def render_plot(self) -> str:
        for datum in self.data:
            if datum.operation_id == 'MonitorOperation':
                self.filter_timestamp(datum)
                self.filter_memory_data(datum)

        if len(self.memory_available) > 0:
            wd = 0.35
            alloc_bar = plt.bar(self.x_axis, self.memory_allocated, width=wd)
            avail_bar = plt.bar(self.x_axis, self.memory_available, width=wd)

            plt.legend((alloc_bar[0], avail_bar[0]), \
                ('Allocated', 'Available'))

        return ""

    @classmethod
    def handles_entire_report(cls, suites: List['Suite']):
        return False

    @classmethod
    def render_report(cls, raw_suites: List['Suite']):
        return ''
