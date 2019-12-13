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

from lib.graphing_components import *


class AffinityTestSuiteHandler(SuiteHandler):

    def __init__(self, suite):
        super().__init__(suite)

    @classmethod
    def can_handle_suite(cls, suite: Suite):
        return "Affinity Test" in suite.name

    def render_plot(self) -> str:
        x_axis_seconds = self.get_x_axis_as_seconds()
        start_time_seconds = self.get_xs()[0] / NS_PER_S
        end_time_seconds = self.get_xs()[-1] / NS_PER_S

        happy_color = [0.2, 1, 0.2]
        bad_color = [1, 0.2, 0.2]

        # render one subplot row per CPU
        for i, cpu_id in enumerate(self.cpu_ids):
            fig = plt.subplot(len(self.cpu_ids), 1, i + 1)
            fig.set_xlim(0, end_time_seconds - start_time_seconds)
            plt.ylabel(f'cpu_{i}')

            for d in self.data_by_cpu_id[cpu_id]:
                # we only care about ScheduleAffinityOperation
                if d.operation_id != "ScheduleAffinityOperation":
                    continue

                s = (d.timestamp / NS_PER_S) - start_time_seconds
                y = fig.get_ylim()[1]

                message = d.get_custom_field("message")
                expected_cpu = int(d.get_custom_field_numeric("expected_cpu"))
                actual_cpu = int(d.cpu_id)

                if message == "work_running":
                    if expected_cpu != actual_cpu:
                        label = f"exp: {expected_cpu} act: {actual_cpu}"
                        fig.axvline(s, color=bad_color, label=label)

                elif message == "work_started":
                    label = f"work_started"
                    fig.axvline(s, color=happy_color, label=label)

                elif message == "work_finished":
                    label = f"work_finished"
                    fig.axvline(s, color=happy_color, label=label)

        return self._generate_summary()
    
    def _generate_summary(self):
        mismatches = 0
        for cpu_id, data in self.data_by_cpu_id.items():
            # find a cpu mismatch while work is running
            for d in data:
                if d.operation_id == "ScheduleAffinityOperation" and d.get_custom_field(
                        "message") == "work_running":
                    expected_cpu = int(d.get_custom_field_numeric("expected_cpu"))
                    actual_cpu = d.cpu_id
                    if expected_cpu != actual_cpu:
                        mismatches += 1

        if mismatches > 0:
            return f"Found {mismatches} CPU affinity mismatches"

        # looks like we have no mismatches to highlight
        return None

