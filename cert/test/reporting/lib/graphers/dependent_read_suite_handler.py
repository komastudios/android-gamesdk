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

from lib.graphers.suite_handler import SuiteHandler
from lib.report import Suite


class DependentReadSuiteHandler(SuiteHandler):
    """Handler for Dependent Texture Read performance test."""

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
        xs = []
        ys_fps = []
        ys_indirections = []

        for datum in self.data:
            if datum.operation_id == 'DependentTextureReadGLES3Operation':
                xs.append(datum.timestamp)
                ys_fps.append(datum.get_custom_field_numeric('fps'))
                ys_indirections.append(
                    datum.get_custom_field_numeric('indirections'))

        xs = np.array(xs)
        ys_fps = np.array(ys_fps)
        ys_indirections = np.array(ys_indirections)

        fig, axs = plt.subplots(2, 1)

        fig.suptitle(self.title())

        axs[0].plot(xs, ys_fps)
        axs[0].set_xlim([xs[0], xs[-1]])
        axs[0].get_xaxis().set_visible(False)
        axs[0].set_ylim([0, ys_fps.max() + 5])
        axs[0].set_ylabel('Frames per second')

        axs[1].stackplot(xs, ys_indirections)
        axs[1].set_xlim([xs[0], xs[-1]])
        axs[1].get_xaxis().set_visible(False)
        axs[1].set_ylim([0, ys_indirections[-1] + 1])
        axs[1].set_ylabel('Indirect reads')

        return None

    @classmethod
    def handles_entire_report(cls, suites: List['Suite']):
        return False

    @classmethod
    def render_report(cls, raw_suites: List['Suite']):
        return ''
