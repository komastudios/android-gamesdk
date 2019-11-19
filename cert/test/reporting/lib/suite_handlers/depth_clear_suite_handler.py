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

import matplotlib
import matplotlib.pyplot as plt

from lib.chart_components import *

from .common_plot import add_plotters_to_default, \
    plot_default, plot_ignore, plot_time_ms_as_sec


def plot_depth_clear_value(renderer):
    """
    Plots the out-of-memory score as a factor of 0 to 1000.
    """
    plt.ylim(0, 1)
    plot_default(renderer)


def plot_error_incorrect_fragment_rejection(renderer):
    plt.yticks([0], ["Error"])
    xs = renderer.get_x_axis_as_seconds()
    ys = renderer.get_ys()
    label = "Incorrect Fragment Rejection"

    # filter to just error values
    errors = [v for v in zip(xs, ys) if v[1] == 1]
    if errors:
        xs, ys = zip(*errors)
        plt.scatter(xs, ys, s=1, color=[1, 0, 0], marker="|", label=label)
    else:
        plt.legend(title="No " + label)


def plot_error_incorrect_fragment_pass(renderer):
    plt.yticks([0], ["Error"])
    xs = renderer.get_x_axis_as_seconds()
    ys = renderer.get_ys()
    label = "Incorrect Fragment Pass"

    # filter to just error values
    errors = [v for v in zip(xs, ys) if v[1] == 1]
    if errors:
        xs, ys = zip(*errors)
        plt.scatter(xs, ys, s=1, color=[1, 0, 0], marker="|", label=label)
    else:
        plt.legend(title="No " + label)


class DepthClearChartRenderer(ChartRenderer):

    plotters = add_plotters_to_default({
        "depth_clear_value":
            plot_depth_clear_value,
        "error_incorrect_fragment_rejection":
            plot_error_incorrect_fragment_rejection,
        "error_incorrect_fragment_pass":
            plot_error_incorrect_fragment_pass,
    })

    def __init__(self, chart: Chart):
        super().__init__(chart)

    @classmethod
    def can_render_chart(cls, chart: Chart):
        return chart.operation_id == "DepthClearGLES3Operation" and (
            chart.field in cls.plotters)

    def is_event_chart(self):
        return False

    def plot(self, fig, index, count, start_time_seconds, end_time_seconds):
        self.plotters.get(self.chart.field, plot_ignore)(self)


class DepthClearSuiteHandler(SuiteHandler):

    def __init__(self, suite):
        super().__init__(suite)

    @classmethod
    def can_handle_suite(cls, suite: Suite):
        return "32bit Depth Clear" in suite.suite_name

    def assign_renderer(self, chart: Chart):
        chart.set_renderer(DepthClearChartRenderer(chart))
        return True

    def analyze(self, cb: Callable[[Any, Datum, str], None]):
        """Invokes callback to notify erroneous fragment write or rejection"""
        incorrect_fragment_pass_datums = []
        incorrect_fragment_rejection_datums = []
        total_writes = 0
        for topic in self.suite.datums_by_topic:
            op_id, field = topic
            for datum in self.suite.datums_by_topic[topic]:
                value = datum.value == "True"

                if field == "writes_passed_as_expected":
                    total_writes += 1

                if field == "error_incorrect_fragment_pass" and value:
                    incorrect_fragment_pass_datums.append(datum)
                elif field == "error_incorrect_fragment_rejection" and value:
                    incorrect_fragment_rejection_datums.append(datum)

        ratio_incorrect_fragment_passes = len(
            incorrect_fragment_pass_datums) / total_writes
        ratio_incorrect_fragment_rejections = len(
            incorrect_fragment_rejection_datums) / total_writes

        if incorrect_fragment_pass_datums or incorrect_fragment_rejection_datums:
            cb(
                self.suite, None,
                f"Incorrect fragment passes: {100 * ratio_incorrect_fragment_passes:3.2f}%"
                +
                f" incorrect fragment rejections: {100 * ratio_incorrect_fragment_rejections:3.2f}%"
            )
            return True

        return False
