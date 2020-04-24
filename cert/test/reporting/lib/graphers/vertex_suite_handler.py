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
"""Graph handler for the Vertex Throughput test."""

from typing import List

from matplotlib.axes import SubplotBase
import matplotlib.pyplot as plt
from matplotlib.ticker import FormatStrFormatter
import numpy as np

from lib.common import nanoseconds_to_seconds
from lib.graphers.suite_handler import SuiteHandler
from lib.report import Datum, Suite


class VertexRateSuiteHandler(SuiteHandler):
    """Graph handler for the Vertex Throughput test."""

    def __init__(self, suite):
        super().__init__(suite)

        self.__first_timestamp = self.data[0].timestamp
        self.__last_timestamp = self.data[-1].timestamp

        self.__xs_vertex = []
        self.__ys_vps = []
        self.__ys_renderers = []

        self.__xs_frame = []
        self.__ys_fps = []

    @classmethod
    def can_handle_suite(cls, suite: Suite):
        return 'Vertex Streaming' in suite.name

    @classmethod
    def can_render_summarization_plot(cls, suites: List['Suite']) -> bool:
        return False

    @classmethod
    def render_summarization_plot(cls, suites: List['Suite']) -> str:
        return None

    def render_plot(self) -> str:
        for datum in self.data:
            if datum.operation_id == 'VertexStreamGLES3Operation':
                self.__fill_vertex_data_points(datum)
            elif datum.operation_id == 'MonitorOperation':
                self.__fill_frame_data_points(datum)

        self.__wrapup_data_points()

        self.__add_charts()

        return None

    def __fill_vertex_data_points(self, vertex_datum: Datum) -> type(None):
        """Segregates vertex rate data points and appends these to collections.
        """
        self.__xs_vertex.append( \
            vertex_datum.timestamp - self.__first_timestamp)

        self.__ys_vps.append(vertex_datum.get_custom_field_numeric( \
            'vertex_throughput.vertices_per_second') / 1000)
        self.__ys_renderers.append(vertex_datum.get_custom_field_numeric( \
            'vertex_throughput.renderers'))

    def __fill_frame_data_points(self, frame_datum: Datum) -> type(None):
        """Segregates fps data points and appends these to collections.
        """
        self.__xs_frame.append( \
            frame_datum.timestamp - self.__first_timestamp)

        self.__ys_fps.append(frame_datum.get_custom_field_numeric( \
            'perf_info.fps'))

    def __wrapup_data_points(self) -> type(None):
        """Massages all collected data to ready it for matplotlib charts."""
        self.__xs_vertex = nanoseconds_to_seconds(np.array(self.__xs_vertex))
        self.__ys_vps = np.array(self.__ys_vps)
        self.__ys_renderers = np.array(self.__ys_renderers)

        self.__xs_frame = nanoseconds_to_seconds(np.array(self.__xs_frame))
        self.__ys_fps = np.array(self.__ys_fps)

    def __add_charts(self) -> type(None):
        """Creates the vertex throughput charts that helps analize device
        behavior during this test."""
        self.__add_vps_chart(plt.subplot(3, 1, 2))
        self.__add_renderers_chart(plt.subplot(3, 1, 1))
        self.__add_fps_chart(plt.subplot(3, 1, 3))

    def __add_vps_chart(self, figure: SubplotBase) -> type(None):
        """Creates the vertices per second chart."""
        figure.plot(self.__xs_vertex, self.__ys_vps)
        figure.get_xaxis().set_major_formatter(FormatStrFormatter('%ds'))
        figure.get_yaxis().set_major_formatter(FormatStrFormatter('%dK'))
        figure.set_ylabel('vertices/second')

    def __add_renderers_chart(self, figure: SubplotBase) -> type(None):
        """Creates the renderers chart."""
        figure.plot(self.__xs_vertex, self.__ys_renderers)
        figure.get_xaxis().set_major_formatter(FormatStrFormatter('%ds'))
        figure.set_ylabel('vertex renderers')

    def __add_fps_chart(self, figure: SubplotBase) -> type(None):
        """Creates the frames per second chart."""
        figure.plot(self.__xs_frame, self.__ys_fps)
        figure.get_xaxis().set_major_formatter(FormatStrFormatter('%ds'))
        figure.set_ylabel('frames/second')
