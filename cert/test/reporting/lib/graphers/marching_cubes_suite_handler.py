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

import math
import sys

from lib.common import Error
from lib.graphing_components import *

X_LABELS = ["sleep(ms) (floating)", "sleep(ms) (pinned)"]
THREAD_SETUPS = ["One", "OnePerBigCore", "OnePerLittleCore", "OnePerCore"]


def y_label(datum: Datum, thread_setup):
    if datum:
        n_threads = int(
            datum.get_custom_field_numeric(
                "marching_cubes_permutation_results.num_threads_used"))
        if thread_setup == "One":
            return "One Core"
        elif thread_setup == "OnePerBigCore":
            return f"{n_threads} Big Cores"
        elif thread_setup == "OnePerLittleCore":
            return f"{n_threads} Little Cores"
        elif thread_setup == "OnePerCore":
            return f"All {n_threads} Cores"
    else:
        if thread_setup == "One":
            return "One Core"
        elif thread_setup == "OnePerBigCore":
            return f"Big Cores"
        elif thread_setup == "OnePerLittleCore":
            return f"Little Cores"
        elif thread_setup == "OnePerCore":
            return f"All Cores"


def voxels_per_second(datum: Datum):
    n_vox = datum.get_custom_field_numeric(
        "marching_cubes_permutation_results.num_voxels_marched_per_iteration")
    avg_dur_s = datum.get_custom_field_numeric(
        "marching_cubes_permutation_results.average_calc_duration") / NS_PER_S
    return n_vox / avg_dur_s


def min_max_voxels_per_second(data: List[Datum]):
    min_vps = float('inf')
    max_vps = 0
    for d in data:
        vps = voxels_per_second(d)
        min_vps = min(min_vps, vps)
        max_vps = max(max_vps, vps)
    min_vps = 0 if math.isinf(min_vps) else min_vps
    return round(min_vps), round(max_vps)


class DataConsistencyError(Error):

    def __init__(self, message):
        self.message = message


class MarchingCubesSuiteHandler(SuiteHandler):

    def __init__(self, suite):
        super().__init__(suite)

        my_data = [
            d for d in self.data
            if d.operation_id == "MarchingCubesGLES3Operation"
        ]

        self.data_by_thread_setup_floating = {}
        self.data_by_thread_setup_pinned = {}
        for thread_setup in THREAD_SETUPS:
            self.data_by_thread_setup_floating[thread_setup] = []
            self.data_by_thread_setup_pinned[thread_setup] = []

        for d in my_data:
            thread_setup = d.get_custom_field(
                "marching_cubes_permutation_results.configuration.thread_setup")

            pin_state = d.get_custom_field(
                "marching_cubes_permutation_results.configuration.pinned")

            if pin_state:
                self.data_by_thread_setup_pinned[thread_setup].append(d)
            else:
                self.data_by_thread_setup_floating[thread_setup].append(d)

    @classmethod
    def can_handle_suite(cls, suite: Suite):
        return "Marching Cubes Permutations Test" in suite.name

    def summary(self):
        # looks like we have no mismatches to highlight
        return None

    def render_plot(self):
        row_names = list(self.data_by_thread_setup_floating.keys())
        row_names.sort()
        rows = len(row_names)

        for col, data_by_thread_setup in enumerate([
                self.data_by_thread_setup_floating,
                self.data_by_thread_setup_pinned
        ]):
            for row, thread_setup in enumerate(row_names):
                idx = 1 + row * 2 + col
                plt.subplot(rows, 2, idx)

                data = data_by_thread_setup[thread_setup]
                if data:
                    ys_by_sleep_dur = {}
                    for d in data:
                        vps = voxels_per_second(d)
                        sleep_dur = d.get_custom_field_numeric(
                            "marching_cubes_permutation_results.configuration.sleep_per_iteration"
                        )
                        ys_by_sleep_dur[sleep_dur] = vps

                    durs = list(ys_by_sleep_dur.keys())
                    durs.sort()

                    ys = list(map(lambda dur: ys_by_sleep_dur[dur], durs))
                    min_y = min(ys)
                    max_y = max(ys)
                    y_range = max(
                        max_y - min_y,
                        10)  # a little padding in case we have one data point
                    min_y -= y_range * 0.25
                    max_y += y_range * 0.25

                    plt.gca().set_ylim([min_y, max_y])
                    plt.bar(durs, ys)

                    plt.xticks(durs, [str(int(d)) for d in durs])
                else:
                    plt.yticks([])
                    plt.xticks([])

                if col == 0:
                    # only show y labels on col 0
                    datum = data[0] if data else None
                    plt.ylabel(y_label(datum, thread_setup))

                if row == len(row_names) - 1:
                    # only show x labels on last row
                    plt.xlabel(X_LABELS[col])
