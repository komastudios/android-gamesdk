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
"""Implements AffinityTestSuiteHandler for processing
reports from schedule affinity operation.
"""

from typing import List

import matplotlib.pyplot as plt

from lib.graphers.suite_handler import SuiteHandler, SuiteSummarizer
from lib.report import Datum, SummaryContext
import lib.summary_formatters.format_items as fmt


class AffinityTestSummarizer(SuiteSummarizer):
    """Suite summarizer for schedule affinity operation test."""
    @classmethod
    def default_handler(cls) -> SuiteHandler:
        return AffinityTestSuiteHandler

    @classmethod
    def can_handle_datum(cls, datum: Datum):
        return "Affinity Test" in datum.suite_id


class AffinityTestSuiteHandler(SuiteHandler):
    """SuiteHandler implementation for schedule affinity operation test
    """

    def render_report(self, ctx: SummaryContext) -> List[fmt.Item]:
        start_misses_by_cpu_id = {}
        work_running_misses_by_cpu_id = {}
        finishing_misses_by_cpu_id = {}

        buckets_by_message = {
            "did_set_affinity": start_misses_by_cpu_id,
            "work_running": work_running_misses_by_cpu_id,
            "work_finished": finishing_misses_by_cpu_id,
        }

        for _, data in self.data_by_cpu_id.items():
            # find a cpu mismatch while work is running
            for datum in data:
                if datum.operation_id == "ScheduleAffinityOperation":
                    message = datum.get_custom_field("message")
                    if message in buckets_by_message:
                        bucket = buckets_by_message[message]

                        expected_cpu = int(
                            datum.get_custom_field_numeric("expected_cpu"))
                        actual_cpu = datum.cpu_id
                        if expected_cpu != actual_cpu:
                            count = bucket.get(actual_cpu, 0)
                            bucket[actual_cpu] = count + 1

        cpus = list(self.data_by_cpu_id.keys())
        cpus.sort()

        max_misses = 0
        for cpu in cpus:
            max_misses = max(max_misses, start_misses_by_cpu_id.get(cpu, 0),
                             work_running_misses_by_cpu_id.get(cpu, 0),
                             finishing_misses_by_cpu_id.get(cpu, 0))

        def graph():
            for i, cpu in enumerate(cpus):
                plt.subplot(len(cpus), 1, i + 1)
                plt.ylabel(f"cpu_{cpu}")

                plt.gca().set_ylim([0, max(max_misses, 1)])

                plt.bar([0, 1, 2], [
                    start_misses_by_cpu_id.get(cpu, 0),
                    work_running_misses_by_cpu_id.get(cpu, 0),
                    finishing_misses_by_cpu_id.get(cpu, 0)
                ])

                if i == len(cpus) - 1:
                    plt.xticks([0, 1, 2], ["Startup", "Running", "Finishing"])
                    plt.xlabel("Affinity Mismatches")
                else:
                    plt.xticks([])

        work_running_misses_total = 0
        for count in work_running_misses_by_cpu_id.values():
            work_running_misses_total += count

        # report summary if we have misses in working period
        msg = "PASSED"
        if work_running_misses_total > 0:
            msg = f"Found {work_running_misses_total} CPU affinity mismatches"

        image = fmt.Image(self.plot(ctx, graph), self.device())
        return [image, fmt.Text(msg)]
