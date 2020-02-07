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
"""Implements MarchingCubesSuiteHandler for processing
reports from marching cubes operation.
"""

from typing import List, Union

import matplotlib.pyplot as plt

from lib.common import nanoseconds_to_milliseconds
from lib.report import Datum, Suite
from lib.graphers.suite_handler import SuiteHandler

SLEEP_DUR = \
    "marching_cubes_permutation_results.configuration.sleep_per_iteration"

X_LABELS = ["sleep(ms) (floating)", "sleep(ms) (pinned)"]

THREAD_SETUPS = [
    "OneBigCore", "AllBigCores", "OneLittleCore", "AllLittleCores", "AllCores"
]


def y_label(datum: Datum, thread_setup):
    """Generate a y-label desciption for plotting based on the thread setup"""
    desc = None
    if datum:
        n_threads = int(
            datum.get_custom_field_numeric(
                "marching_cubes_permutation_results.num_threads_used"))
        if thread_setup == "OneBigCore":
            desc = "1 Big Core"
        elif thread_setup == "AllBigCores":
            desc = f"{n_threads} Big Cores" if n_threads > 1 else "1 Big Core"
        elif thread_setup == "OneLittleCore":
            desc = "1 Little Core"
        elif thread_setup == "AllLittleCores":
            desc = f"{n_threads} Little Cores" if n_threads > 1 else "1 Little Core"
        elif thread_setup == "AllCores":
            desc = f"All {n_threads} Cores" if n_threads > 1 else "1 Core"
        return desc

    if thread_setup == "OneBigCore":
        desc = "One Big Core"
    elif thread_setup == "AllBigCores":
        desc = "All Big Cores"
    elif thread_setup == "OneLittleCore":
        desc = "1 Little Core"
    elif thread_setup == "AllLittleCores":
        desc = "All Little Cores"
    elif thread_setup == "AllCores":
        desc = "All Cores"

    return desc


def apply_bar_value_labels(axis, rects):
    """Attach a text label above each bar in *rects*, displaying its height.
    Args:
        axis: pyplot axes
        rects: The bar rects returned by plt.bar()
    """
    for rect in rects:
        height = rect.get_height()
        axis.annotate(
            '{}'.format(int(height)),
            xy=(rect.get_x() + rect.get_width() / 2, height),
            xytext=(0, 0),  # 3 points vertical offset
            textcoords="offset points",
            ha='center',
            va='bottom')


def get_datum_value(datum: Datum, key: str) -> Union[int, float]:
    '''Helper to look up values in MarchingCubesGLES3Operation results'''
    return datum.get_custom_field_numeric(
        f"marching_cubes_permutation_results.{key}")


class DataConsistencyError(Exception):
    """Error raised to represent some kind of data inconstency."""


class MarchingCubesSuiteHandler(SuiteHandler):
    """SuiteHandler implementation for processing data from
    MarchingCubesGLES3Operation"""

    def __init__(self, suite):
        super().__init__(suite)

        self.data_by_thread_setup_floating = {}
        self.data_by_thread_setup_pinned = {}

        for thread_setup in THREAD_SETUPS:
            self.data_by_thread_setup_floating[thread_setup] = []
            self.data_by_thread_setup_pinned[thread_setup] = []

        for datum in suite.data_by_operation_id["MarchingCubesGLES3Operation"]:
            thread_setup = datum.get_custom_field(
                "marching_cubes_permutation_results.exec_configuration.thread_affinity")

            pin_state = datum.get_custom_field(
                "marching_cubes_permutation_results.exec_configuration.thread_pinned")

            if pin_state:
                self.data_by_thread_setup_pinned[thread_setup].append(datum)
            else:
                self.data_by_thread_setup_floating[thread_setup].append(datum)

    @classmethod
    def can_handle_suite(cls, suite: Suite):
        return "MarchingCubesGLES3Operation" in suite.data_by_operation_id

    @classmethod
    def can_render_summarization_plot(cls,
                                      suites: List['SuiteHandler']) -> bool:
        return False

    @classmethod
    def render_summarization_plot(cls, suites: List['SuiteHandler']) -> str:
        return None

    def render_plot(self) -> str:
        thread_setups = list(self.data_by_thread_setup_floating.keys())

        # THREAD_SETUPS is already in the desired order, so filter it
        # to those in our data, while retaining THREAD_SETUPS's order
        thread_setups = [ts for ts in THREAD_SETUPS if ts in thread_setups]
        num_rows = len(thread_setups)

        for col, data_by_thread_setup in enumerate([
                self.data_by_thread_setup_floating,
                self.data_by_thread_setup_pinned
        ]):
            for row, thread_setup in enumerate(thread_setups):
                idx = 1 + row * 2 + col
                axes = plt.subplot(num_rows, 2, idx)

                data = data_by_thread_setup[thread_setup]
                if data:
                    y_values_by_sleep_dur = {}
                    for datum in data:
                        vps = get_datum_value(datum, "median_vps")
                        sleep_dur = nanoseconds_to_milliseconds(
                            datum.get_custom_field_numeric(SLEEP_DUR))
                        y_values_by_sleep_dur[sleep_dur] = vps

                    durs = list(y_values_by_sleep_dur.keys())
                    durs.sort()

                    y_values = [y_values_by_sleep_dur[dur] for dur in durs]
                    bar_width = 1 / len(y_values)

                    if len(y_values) > 1:
                        min_y = min(y_values)
                        max_y = max(y_values)
                        y_range = max(
                            max_y - min_y, 10
                        )  # a little padding in case we have one data point
                        min_y -= y_range * 0.25
                        max_y += y_range * 0.25
                        plt.gca().set_ylim([min_y, max_y])

                    else:
                        # some reports have just one value, and that
                        # wreaks layout havok with the min_y, max_y range
                        plt.yticks(y_values)

                    plt.xticks(durs, ["{:.2f}".format(d) for d in durs])

                    bar_colors = [(0.25, 0.75, 0.25) if y >= max(y_values) else
                                  (0.5, 0.5, 0.5) for y in y_values]

                    rendered_bars = plt.bar(durs,
                                            y_values,
                                            color=bar_colors,
                                            width=bar_width)

                    apply_bar_value_labels(axes, rendered_bars)

                else:
                    plt.yticks([])
                    plt.xticks([])

                if col == 0:
                    # only show y labels on col 0
                    datum = data[0] if data else None
                    plt.ylabel(y_label(datum, thread_setup))

                if row == len(thread_setups) - 1:
                    # only show x labels on last row
                    plt.xlabel(X_LABELS[col])

        plt.subplots_adjust(hspace=0.5)

        # no interesting summary
        return None
