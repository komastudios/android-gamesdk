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
"""A grapher for the calculate pi & wait operation.
"""

from typing import List

import matplotlib.pyplot as plt
import numpy as np

from lib.graphers.suite_handler import SuiteHandler
from lib.report import Datum, Suite


class CalculateWaitPiSuiteHandler(SuiteHandler):
    """Grapher for the calculate pi & wait operation."""

    def __init__(self, suite: Suite):
        super().__init__(suite)

        self.__wait_method = None
        self.gather_wait_method()

    @classmethod
    def can_handle_suite(cls, suite: Suite):
        return 'WaitForPI' in suite.name

    @classmethod
    def can_render_summarization_plot(cls, suites: List['Suite']) -> bool:
        return False

    @classmethod
    def render_summarization_plot(cls, suites: List['Suite']) -> str:
        return None

    def gather_wait_method(self):
        """Checks data per CPU looking for the chosen wait method."""
        index = 0
        while self.__wait_method is None:
            no_cpu_data_visited = True
            for cpu_id in self.cpu_ids:
                cpu_data = self.data_by_cpu_id[cpu_id]
                if len(cpu_data) > index:
                    no_cpu_data_visited = False
                    datum = cpu_data[index]
                    if datum.operation_id == 'CalculateWaitPIOperation' and \
                        isinstance(datum.custom, str):
                        self.__wait_method = datum.custom
                        break
            if no_cpu_data_visited:
                print("Warning: a wait method wasn't found in reported data.")
                break
            else:
                index += 1

    def render_plot(self) -> str:
        figsize = (5, 4)
        fig1, axs = plt.subplots(len(self.cpu_ids) / 2,
                                 2,
                                 figsize=figsize,
                                 constrained_layout=True)
        return ''

    @classmethod
    def handles_entire_report(cls, suites: List['Suite']):
        return False

    @classmethod
    def render_report(cls, raw_suites: List['SuiteHandler']):
        return ''
