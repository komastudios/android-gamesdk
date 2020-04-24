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
"""Provides CpusetSuiteHandler which gets graphics for the cpuset functional
test.
"""

from typing import List

import matplotlib.pyplot as plt
import numpy as np

from lib.graphers.suite_handler import SuiteHandler, SuiteSummarizer
from lib.report import Datum, SummaryContext, Suite
import lib.summary_formatters.format_items as fmt


class CpusetSuiteSummarizer(SuiteSummarizer):
    """Suite summarizer for the cpuset functional test."""
    @classmethod
    def default_handler(cls) -> SuiteHandler:
        return CpusetSuiteHandler


class CpusetSuiteHandler(SuiteHandler):
    """Grapher for the cpuset functional test (F2.1)."""

    def __init__(self, suite: Suite):
        super().__init__(suite)

        self.xs_ok: List[float] = []
        self.ys_ok: List[float] = []
        self.xs_err: List[float] = []
        self.ys_err: List[float] = []

        self.bigger_cores_first = None
        self.cpuset_setup_total = 0
        self.cpuset_setup_error = 0

    @classmethod
    def can_handle_datum(cls, datum: Datum):
        return "Sane CPUSET" in datum.suite_id

    def analyze_setup(self, datum: Datum):
        """Checks success/failure state after setting core affinity to a
        thread. Keeps counters of setup total vs. setup failures"""
        if not datum.get_custom_field("cpuset_enabled"):
            self.cpuset_setup_error += 1
        self.cpuset_setup_total += 1

    def analyze_in_progress_event(self, datum: Datum,
                                  timestamp_in_seconds: float):
        """Inspects contents of an in-progress datum, filling the proper
        coordinates (OK, error)."""
        if datum.get_custom_field('is_core_among_affine'):
            self.xs_ok.append(timestamp_in_seconds)
            self.ys_ok.append(0)
        else:
            self.xs_err.append(timestamp_in_seconds)
            self.ys_err.append(0)

    def analyze_test_results(self, datum: Datum):
        """Captures the 'sane' core numbering."""
        self.bigger_cores_first = \
            datum.get_custom_field('bigger_cores_first')

    def format_figure_and_axes(self, x_axis_as_seconds: List[float]):
        """Sets aspects like figure height, visibility and text of legends,
        among other graphical aspects."""
        fig, axes = plt.subplots()
        fig.set_figheight(2)

        first_timestamp = x_axis_as_seconds[0]
        last_timestamp = x_axis_as_seconds[-1]
        axes.set_xlim(0, last_timestamp - first_timestamp)
        axes.get_yaxis().set_visible(False)

        # A green line represents all situations were affinity was kept.
        axes.plot(np.array(self.xs_ok),
                  np.array(self.ys_ok),
                  '-',
                  color='tab:green')
        # Red crosses signal points were affinity wasn't kept.
        axes.plot(np.array(self.xs_err),
                  np.array(self.ys_err),
                  'x',
                  color='tab:red')
        axes.legend(['Affinity OK', 'Affinity fail'])

    def compose_summary(self) -> str:
        """Based on the collected data, composes a string that details whether
        cores are numbered from big to LITTLE, whether setting affinity
        succeeded via CPUSET succeeded or not, whether affinity was preserved
        during the test run."""
        if self.bigger_cores_first is None:
            self.bigger_cores_first = self.suite.build.check_core_sizes()
        sanity = f'Cores are{" not" if not self.bigger_cores_first else ""} ' \
            'numbered from big to LITTLE.'

        # Initial value, to be changed if next if succeeds
        cpuset_enabled = 'No data collected about CPUSET affinity setup.'
        if self.cpuset_setup_total > 0:
            if self.cpuset_setup_error == 0:
                cpuset_enabled = 'CPUSET is enabled'
                if len(self.xs_err) > 0:
                    cpuset_enabled += " but, at times, affinity wasn't kept."
                else:
                    cpuset_enabled += ", and affinity was kept at all times."
            elif self.cpuset_setup_error == self.cpuset_setup_total:
                cpuset_enabled = "CPUSET isn't enabled, as all affinity " \
                    'schedules failed.'
            else:
                cpuset_enabled = 'CPUSET seems enabled, but some affinity ' \
                    'schedules failed.'

        return f'{sanity} {cpuset_enabled}'

    def render(self, ctx: SummaryContext) -> List[fmt.Item]:

        x_axis_as_seconds = self.get_x_axis_as_seconds()
        for i, datum in enumerate(self.data):
            if datum.operation_id == 'SaneCpusetOperation':
                stage = datum.get_custom_field('stage')
                if stage == 'Thread setup':
                    self.analyze_setup(datum)
                elif stage == 'Thread in progress':
                    self.analyze_in_progress_event(datum, x_axis_as_seconds[i])
                elif stage == 'Test summary':
                    self.analyze_test_results(datum)

        def graph():
            self.format_figure_and_axes(x_axis_as_seconds)

        image_path = self.plot(ctx, graph)
        image = fmt.Image(image_path, self.device())
        text = fmt.Text(self.compose_summary())
        return [image, text]
