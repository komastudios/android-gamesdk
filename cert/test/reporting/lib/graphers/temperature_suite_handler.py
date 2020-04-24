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

from lib.graphers.suite_handler import SuiteHandler, SuiteSummarizer
from lib.report import Datum, SummaryContext, Suite
import lib.summary_formatters.format_items as fmt


class TemperatureSuiteSummarizer(SuiteSummarizer):
    """Suite summarizer for TemperatureSuiteHandler."""
    @classmethod
    def default_handler(cls) -> SuiteHandler:
        return TemperatureSuiteHandler

    @classmethod
    def can_handle_datum(cls, datum: Datum):
        return "WaitForPI" in datum.suite_id


class TemperatureSuiteHandler(SuiteHandler):
    """Grapher for test non-functional aspects like memory or FPS."""

    def __init__(self, suite: Suite):
        super().__init__(suite)

        self.x_axis_as_seconds = self.get_x_axis_as_seconds()

        self.x_axis = []
        self.max_temperature = []

    def filter_temperature(self, datum: Datum) -> type(None):
        """Appends datum temperature info to the proper vectors to graph."""
        self.max_temperature.append(
            datum.get_custom_field_numeric(
                'temperature_info.max_cpu_temperature') / 1000)
        # Divided by 1000 so it's in Celsius degrees (comes in millidegrees)


    def render_report(self, ctx: SummaryContext) -> List[fmt.Item]:

        for i, datum in enumerate(self.data):
            if datum.operation_id == 'MonitorOperation':
                self.x_axis.append(self.x_axis_as_seconds[i])
                self.filter_temperature(datum)

        def graph():
            if len(self.max_temperature) > 0:
                plt.plot(np.array(self.x_axis), np.array(self.max_temperature))

        image = fmt.Image(self.plot(ctx, graph), self.device())
        return [image]
