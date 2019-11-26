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
    plot_default, plot_ignore, plot_time_ns_as_ms


def plot_pi_error(renderer):
    """
    Adapts pi calculation errors to a more relevant scale
    """
    return plot_default(renderer, 1e-9, "%d x10e-9")


def plot_pi_iterations(renderer):
    """
    Adapts pi iterations to a more relevant scale
    """
    return plot_default(renderer, 1e6, "%d M")


class CalculatePiChartRenderer(ChartRenderer):

    plotters = add_plotters_to_default({
        "pi.duration": plot_time_ns_as_ms,
        "pi.iterations": plot_pi_iterations,
        "pi.value": plot_default,
        "pi.error": plot_pi_error,
    })

    def __init__(self, chart: Chart):
        super().__init__(chart)

    @classmethod
    def can_render_chart(cls, chart: Chart):
        return chart.operation_id == "CalculatePIOperation" and (
            chart.field in cls.plotters)

    def is_event_chart(self):
        return False

    def plot(self, fig, index, count, start_time_seconds, end_time_seconds):
        self.plotters.get(self.chart.field, plot_ignore)(self)


class CalculatePiSuiteHandler(SuiteHandler):

    def __init__(self, suite):
        super().__init__(suite)

    @classmethod
    def can_handle_suite(cls, suite: Suite):
        return "Calculate Pi (C++)" in suite.suite_name

    def assign_renderer(self, chart: Chart):
        chart.set_renderer(CalculatePiChartRenderer(chart))
        return True

    def analyze(self, cb: Callable[[Any, Datum, str], None]):
        """TODO(dagum@google.com): No analysis as yet on calculate pi data"""
        return None
