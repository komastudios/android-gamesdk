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

from typing import Any, Callable

from lib.chart_components \
  import Chart, ChartRenderer, Datum, Suite, SuiteHandler

from .common_plot import add_plotters_to_default, plot_default, plot_ignore


def plot_components_per_vertex(renderer):
    plot_default(renderer, y_label_format="%d comp.")


def plot_vertices_per_second(renderer):
    plot_default(renderer, 1e6, "%.2fM vert/sec")


def plot_vertices_per_renderer(renderer):
    plot_default(renderer, 1e3, "%dK vertices")


class VertexStreamingChartRenderer(ChartRenderer):

    plotters = add_plotters_to_default({
        "vertex_throughput.vertices_per_second": plot_vertices_per_second,
        "vertex_throughput.components_per_vertex": plot_components_per_vertex,
        "vertex_throughput.vertices_per_renderer": plot_vertices_per_renderer,
        "vertex_throughput.renderers": plot_default,
    })

    def __init__(self, chart: Chart):
        super().__init__(chart)

    @classmethod
    def can_render_chart(cls, chart: Chart):
        return chart.operation_id == "VertexStreamGLES3Operation" and (
            chart.field in cls.plotters)

    def is_event_chart(self):
        return False

    def plot(self, fig, index, count, start_time_seconds, end_time_seconds):
        self.plotters.get(self.chart.field, plot_ignore)(self)


class VertexStreamingSuiteHandler(SuiteHandler):

    def __init__(self, suite):
        super().__init__(suite)

    @classmethod
    def can_handle_suite(cls, suite: Suite):
        return "Vertex Streaming" in suite.suite_name

    def assign_renderer(self, chart: Chart):
        chart.set_renderer(VertexStreamingChartRenderer(chart))
        return True

    def analyze(self, cb: Callable[[Any, Datum, str], None]):
        """
        TODO(dagum@google.com): Determine analysis points.
        Hint: fps tells a lot about how adding more renderers affect fps.
        """
        return None