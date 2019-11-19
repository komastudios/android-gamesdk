# import matplotlib
# import matplotlib.pyplot as plt
# import numpy as np

# from typing import List

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
