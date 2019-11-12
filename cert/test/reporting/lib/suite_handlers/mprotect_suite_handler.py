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

import matplotlib.pyplot as plt
import numpy as np

from lib.chart_components import *

from .common_plot import plot_ignore


MPROTECT_RANKING = [
    "mprotect available", "R/W protect fail", "Unexpected violation",
    "Read protect fail", "Missing violation", "Mem mapping fail",
    "Mem alloc fail", "No mappable mem"
]


def plot_rank(renderer):
    """
    Plots the mprotect ranking on a green box if OK, red otherwise.
    """
    ranking_length = len(MPROTECT_RANKING)
    ranking_value = int(renderer.data[0].numeric_value)
    ranking_color = (0, 1, 0) if ranking_value == 0 else (1, 0, 0)

    plt.xticks(np.arange(ranking_length))
    plt.yticks([])
    plt.barh(0, ranking_length, color=ranking_color)
    plt.text((ranking_length - 1) / 2,
             0,
             MPROTECT_RANKING[ranking_value],
             ha='center')


class MprotectChartRenderer(ChartRenderer):

    plotters = {
        "mprotect.rank": plot_rank,
    }

    def __init__(self, chart: Chart):
        super().__init__(chart)

    @classmethod
    def can_render_chart(cls, chart: Chart):
        return chart.operation_id == "JsonManipulatorOperation" and (
            chart.field in cls.plotters)

    def is_event_chart(self):
        return False

    def plot(self, fig, index, count, start_time_seconds, end_time_seconds):
        self.plotters.get(self.chart.field, plot_ignore)(self)


class MprotectSuiteHandler(SuiteHandler):

    def __init__(self, suite):
        super().__init__(suite)

    @classmethod
    def can_handle_suite(cls, suite: Suite):
        return "Vulkan memory write protection" in suite.suite_name

    def assign_renderer(self, chart: Chart):
        chart.set_renderer(MprotectChartRenderer(chart))
        return True

    def analyze(self, cb: Callable[[Any, Datum, str], None]):
        return None
