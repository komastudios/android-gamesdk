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


class TemperatureSuiteHandler(SuiteHandler):
    """Grapher for test non-functional aspects like memory or FPS."""

    def __init__(self, suite: Suite):
        super().__init__(suite)

        self.x_axis_as_seconds = self.get_x_axis_as_seconds()

        self.x_axis = []
        self.max_temperature = []

    @classmethod
    def can_handle_suite(cls, suite: Suite):
        return suite.name == "WaitForPI"

    @classmethod
    def can_render_summarization_plot(cls, suites: List['Suite']) -> bool:
        return False

    @classmethod
    def render_summarization_plot(cls, suites: List['Suite']) -> str:
        return None

    def filter_temperature(self, datum: Datum) -> type(None):
        """Appends datum temperature info to the proper vectors to graph."""
        self.max_temperature.append(
            datum.get_custom_field_numeric(
                'temperature_info.max_cpu_temperature') / 1000)
        # Divided by 1000 so it's in Celsius degrees (comes in millidegrees)

    def render_plot(self) -> str:
        for i, datum in enumerate(self.data):
            if datum.operation_id == 'MonitorOperation':
                self.x_axis.append(self.x_axis_as_seconds[i])
                self.filter_temperature(datum)

        if len(self.max_temperature) > 0:
            plt.plot(np.array(self.x_axis), np.array(self.max_temperature))

        return ""

    @classmethod
    def handles_entire_report(cls, suites: List['Suite']):
        return False

    @classmethod
    def render_report(cls, raw_suites: List['Suite']):
        return ''
