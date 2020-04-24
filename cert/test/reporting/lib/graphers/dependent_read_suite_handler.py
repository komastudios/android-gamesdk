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
"""Grapher for OpenGL ES test Dependent Texture Read (aka Indirect Texture
Read)"""

from typing import List

import numpy as np
import matplotlib.pyplot as plt

from lib.common import nanoseconds_to_milliseconds
from lib.graphers.suite_handler import SuiteHandler
from lib.report import Datum, Suite


class DependentReadSuiteHandler(SuiteHandler):
    """Handler for Dependent Texture Read performance test."""

    def __init__(self, suite: Suite):
        super().__init__(suite)

        self.__xs = []
        self.__ys_fps = []
        self.__ys_indirections = []

        self.__xs_frame_time = []
        self.__ys_max_frame_time = []

    @classmethod
    def can_handle_suite(cls, suite: Suite):
        return "Indirect Texture Read" in suite.name

    @classmethod
    def can_render_summarization_plot(cls, suites: List['Suite']) -> bool:
        return False

    @classmethod
    def render_summarization_plot(cls, suites: List['Suite']) -> str:
        return None

    def render_plot(self) -> str:
        self.__compile_data()
        self.__adapt_data_points_for_graphics()

        fig, axs = plt.subplots(3, 1)
        fig.suptitle(self.title())

        self.__plot_fps(axs[0])
        self.__plot_frame_times(axs[1])
        self.__plot_indirectios(axs[2])

        return None

    def __compile_data(self) -> type(None):
        """Iterates through all test data, segregating data points into their
        proper buckets."""
        for datum in self.data:
            if datum.operation_id == 'DependentTextureReadGLES3Operation':
                self.__xs.append(datum.timestamp)
                self.__ys_fps.append(datum.get_custom_field_numeric('fps'))
                self.__ys_indirections.append(
                    datum.get_custom_field_numeric('indirections'))
                self.__compile_frame_times(datum)

    def __compile_frame_times(self, datum: Datum) -> type(None):
        """Segregates frame times data points."""
        max_frame_time = datum.get_custom_field_numeric('maximum_frame_time')

        if max_frame_time > 0:
            self.__xs_frame_time.append(datum.timestamp)
            self.__ys_max_frame_time.append(max_frame_time)

    def __adapt_data_points_for_graphics(self) -> type(None):
        """Converts data points to Numpy for MatPlotLib."""
        self.__xs = np.array(self.__xs)
        self.__ys_fps = np.array(self.__ys_fps)
        self.__ys_indirections = np.array(self.__ys_indirections)

        self.__xs_frame_time = np.array(self.__xs_frame_time)
        self.__ys_max_frame_time = nanoseconds_to_milliseconds(
            np.array(self.__ys_max_frame_time))

    def __plot_fps(self, axes) -> type(None):
        """Renders the frame per second curve."""
        axes.plot(self.__xs, self.__ys_fps)
        axes.set_xlim([self.__xs[0], self.__xs[-1]])
        axes.get_xaxis().set_visible(False)
        axes.set_ylim([0, self.__ys_fps.max() + 5])
        axes.set_ylabel('Frames per second')

    def __plot_frame_times(self, axes) -> type(None):
        """Renders min and max frame time curves."""
        axes.plot(self.__xs_frame_time,
                  self.__ys_max_frame_time)
        axes.set_xlim([self.__xs[0], self.__xs[-1]])
        axes.get_xaxis().set_visible(False)
        axes.set_ylim([
            self.__ys_max_frame_time.min() - 3,
            self.__ys_max_frame_time.max() + 3
        ])
        axes.set_ylabel('Max frame time (ms)')

    def __plot_indirectios(self, axes) -> type(None):
        """Renders texture coordinate reading indirections."""
        axes.stackplot(self.__xs, self.__ys_indirections)
        axes.set_xlim([self.__xs[0], self.__xs[-1]])
        axes.get_xaxis().set_visible(False)
        axes.set_ylim([0, self.__ys_indirections[-1] + 1])
        axes.set_ylabel('Indirect reads')
