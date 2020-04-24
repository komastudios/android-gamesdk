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
"""Graph handler for the Fill Rate test."""

from typing import List

from matplotlib.axes import SubplotBase
import matplotlib.pyplot as plt
from matplotlib.ticker import FormatStrFormatter
import numpy as np

from lib.common import nanoseconds_to_seconds
from lib.graphers.suite_handler import SuiteHandler, SuiteSummarizer
from lib.report import Datum, SummaryContext
import lib.summary_formatters.format_items as fmt


class FillRateSummarizer(SuiteSummarizer):
    @classmethod
    def default_handler(cls) -> SuiteHandler:
        return FillRateSuiteHandler

    @classmethod
    def can_handle_datum(cls, datum: Datum):
        return 'GPU fill rate' in datum.suite_id


class FillRateSuiteHandler(SuiteHandler):
    """Graph handler for the Fill Rate test."""

    def __init__(self, suite):
        super().__init__(suite)

        self.__first_timestamp = self.data[0].timestamp
        self.__last_timestamp = self.data[-1].timestamp

        self.__xs_quad = []
        self.__ys_quads = []
        self.__ys_ppq = []
        self.__ys_pps = []

        self.__xs_frame = []
        self.__ys_fps = []

    def render_report(self, ctx: SummaryContext) -> List[fmt.Item]:
        for datum in self.data:
            if datum.operation_id == 'FillRateGLES3Operation':
                self.__quad_data_points(datum)
            elif datum.operation_id == 'MonitorOperation':
                self.__frame_data_points(datum)

        self.__wrapup_data_points()

        image = fmt.Image(self.plot(ctx, self.__add_charts), self.device())
        return [image]

    def __quad_data_points(self, quad_datum: Datum) -> type(None):
        """Segregates fill rate data points and appends these to collections.
        """
        self.__xs_quad.append(quad_datum.timestamp - self.__first_timestamp)

        self.__ys_quads.append(quad_datum.get_custom_field_numeric( \
            'fill_rate.num_quads') / 1000)
        self.__ys_ppq.append(quad_datum.get_custom_field_numeric( \
            'fill_rate.pixels_per_quad') / 1000)
        self.__ys_pps.append(quad_datum.get_custom_field_numeric( \
            'fill_rate.pixels_per_second') / 1000)

    def __frame_data_points(self, frame_datum: Datum) -> type(None):
        """Segregates fps data points and appends these to collections.
        """
        self.__xs_frame.append( \
            frame_datum.timestamp - self.__first_timestamp)

        self.__ys_fps.append(frame_datum.get_custom_field_numeric( \
            'perf_info.fps'))

    def __wrapup_data_points(self) -> type(None):
        """Massages all collected data to ready it for matplotlib charts."""
        self.__xs_quad = nanoseconds_to_seconds(np.array(self.__xs_quad))
        self.__ys_quads = np.array(self.__ys_quads)
        self.__ys_ppq = np.array(self.__ys_ppq)
        self.__ys_pps = np.array(self.__ys_pps)

        self.__xs_frame = nanoseconds_to_seconds(np.array(self.__xs_frame))
        self.__ys_fps = np.array(self.__ys_fps)

    def __add_charts(self) -> type(None):
        """Creates the fill rate charts that helps analize device
        behavior during this test."""
        self.__add_quad_chart(plt.subplot(2, 2, 1))
        self.__add_ppq_chart(plt.subplot(2, 2, 3))
        self.__add_pps_chart(plt.subplot(2, 2, 2))
        self.__add_fps_chart(plt.subplot(2, 2, 4))

    def __add_quad_chart(self, figure: SubplotBase) -> type(None):
        """Creates the total quads chart."""
        self.__add_quad_related_chart(figure, self.__ys_quads, "quads")

    def __add_ppq_chart(self, figure: SubplotBase) -> type(None):
        """Creates the pixels per quad chart."""
        self.__add_quad_related_chart(figure, self.__ys_ppq, "pixels/quad")

    def __add_pps_chart(self, figure: SubplotBase) -> type(None):
        """Creates the pixels per second chart."""
        self.__add_quad_related_chart(figure, self.__ys_pps, "pixels/second")

    def __add_quad_related_chart(self, figure: SubplotBase, ys,
                                 title: str) -> type(None):
        """Creates a fill quad-related chart given a series of y-values and a
        title."""
        figure.plot(self.__xs_quad, ys)
        figure.get_xaxis().set_major_formatter(FormatStrFormatter('%ds'))
        figure.get_yaxis().set_major_formatter(FormatStrFormatter('%dK'))
        figure.set_ylabel(title)

    def __add_fps_chart(self, figure: SubplotBase) -> type(None):
        """Creates the frames per second chart."""
        figure.plot(self.__xs_frame, self.__ys_fps)
        figure.get_xaxis().set_major_formatter(FormatStrFormatter('%ds'))
        figure.set_ylabel('frames/second')
