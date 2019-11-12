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

from lib.chart_components import *

from .common_plot import add_plotters_to_default, \
    plot_default, plot_ignore, plot_time_ms_as_sec


class CpuIntensiveChartRenderer(ChartRenderer):

    plotters = add_plotters_to_default({
        "json_manipulation.duration": plot_time_ms_as_sec,
        "json_manipulation.iterations": plot_default,
    })

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


class CpuIntensiveSuiteHandler(SuiteHandler):

    def __init__(self, suite):
        super().__init__(suite)

    @classmethod
    def can_handle_suite(cls, suite: Suite):
        return "Cpu intensive json (Java)" in suite.suite_name

    def assign_renderer(self, chart: Chart):
        chart.set_renderer(CpuIntensiveChartRenderer(chart))
        return True

    def analyze(self, cb: Callable[[Any, Datum, str], None]):
        """TODO(dagum@google.com): No analysis as yet on cpu intensive data"""
        return None
