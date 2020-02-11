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

from lib.graphers.suite_handler import SuiteHandler
from lib.report import Datum, Suite


class CpusetSuiteHandler(SuiteHandler):
    """Grapher for the cpuset functional test (F2.1)."""

    def __init__(self, suite: Suite):
        super().__init__(suite)

        self.xs_ok: List[float] = []
        self.ys_ok: List[float] = []
        self.xs_err: List[float] = []
        self.ys_err: List[float] = []

        self.summary_message: str = ''

    @classmethod
    def can_handle_suite(cls, suite: Suite):
        return 'Sane CPUSET' in suite.name

    @classmethod
    def can_render_summarization_plot(cls,
                                      suites: List['SuiteHandler']) -> bool:
        return False

    @classmethod
    def render_summarization_plot(cls, suites: List['SuiteHandler']) -> str:
        return None

    def analize_in_progress_event(self, datum: List[Datum],
                                  timestamp_in_seconds: float):
        """Inspects contents of an in-progress datum, filling the proper
        coordinates (OK, error)."""
        if datum.get_custom_field('is_core_among_affine'):
            self.xs_ok.append(timestamp_in_seconds)
            self.ys_ok.append(0)
        else:
            self.xs_err.append(timestamp_in_seconds)
            self.ys_err.append(0)

    def analize_test_results(self, datum: List[Datum]):
        """Inspects contents of a test summary datum, composing the summary
        message."""
        if datum.get_custom_field('cpuset_enabled'):
            bottom_line = 'CPUSET is enabled on this device'
            detail = ''
            if len(self.xs_err) > 0:
                detail = ', although affinity failed in some cases'

            sanity = 'Cores are numbered from big to LITTLE'
            if not datum.get_custom_field('bigger_cores_first'):
                sanity = "Cores aren't numbered from big to LITTLE"

            self.summary_message = f'{bottom_line}{detail}. {sanity}.'
        else:
            self.summary_message = "CPUSET isn't enabled on this device."

    def render_plot(self) -> str:
        fig, axes = plt.subplots()
        fig.set_figheight(2)

        x_axis_as_seconds = self.get_x_axis_as_seconds()
        for i, datum in enumerate(self.data):
            if datum.operation_id == 'SaneCpusetOperation':
                stage = datum.get_custom_field('stage')
                if stage == 'Thread in progress':
                    self.analize_in_progress_event(datum, x_axis_as_seconds[i])
                elif stage == 'Test summary':
                    self.analize_test_results(datum)

        axes.set_xlim(0, x_axis_as_seconds[-1] - x_axis_as_seconds[0])
        axes.get_yaxis().set_visible(False)
        # A green line represents all situations were affinity was kept.
        axes.plot(np.array(self.xs_ok), np.array(self.ys_ok),
                  '-', color='tab:green')
        # Red crosses signal points were affinity wasn't kept.
        axes.plot(np.array(self.xs_err), np.array(self.ys_err),
                  'x', color='tab:red')

        return self.summary_message

    @classmethod
    def handles_entire_report(cls, suites: List['Suite']):
        return False

    @classmethod
    def render_report(cls, raw_suites: List['SuiteHandler']):
        return ''
