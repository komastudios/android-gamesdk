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

from math import ceil
from typing import List

import matplotlib.pyplot as plt
import numpy as np

from lib.graphers.suite_handler import SuiteHandler
from lib.report import Datum, Suite


class CalculateWaitPiSuiteHandler(SuiteHandler):
    """Grapher for the calculate pi & wait operation."""

    def __init__(self, suite: Suite):
        super().__init__(suite)

        self.__data_by_thread = {}
        self.__temperature_data = []
        self.__max_iterations = 0
        self.__max_x = 0

        is_first_datum = True
        first_timestamp = 0
        for datum in self.data:
            if is_first_datum:
                self.__runtime_params = datum.custom
                is_first_datum = False
                first_timestamp = datum.timestamp
            else:
                datum.timestamp -= first_timestamp
                self.__max_x = max(self.__max_x, datum.timestamp)
                if datum.operation_id == 'CalculateWaitPIOperation':
                    thread_id = datum.thread_id
                    thread_data = self.__data_by_thread.get(thread_id)
                    if thread_data is None:
                        thread_data = []
                        self.__data_by_thread[thread_id] = thread_data
                    datum.custom['t0'] -= first_timestamp
                    datum.custom['t1'] -= first_timestamp
                    thread_data.append(datum)
                    self.__max_iterations = max(self.__max_iterations,
                                                datum.get_custom_field('iterations'))
                elif datum.operation_id == 'MonitorOperation':
                    self.__temperature_data.\
                        append(datum.custom['temperature_info'])

    @classmethod
    def can_handle_suite(cls, suite: Suite):
        return 'WaitForPI' in suite.name

    @classmethod
    def can_render_summarization_plot(cls, suites: List['Suite']) -> bool:
        return False

    @classmethod
    def render_summarization_plot(cls, suites: List['Suite']) -> str:
        return None

    def render_plot(self) -> str:
        # figsize = (6, 6)
        rows, cols = ceil((len(self.cpu_ids) + 1) / 2), 2
        fig, axes = plt.subplots(rows, cols, sharex=True, sharey=True)
        thread_id = 0
        fig.suptitle("I never said they'd be pretty")
        fig.subplots_adjust(hspace=0.5)
        for axe in axes.flatten():
            axe.set_title(f"Thread #{thread_id}")
            axe.set_ylim(0, self.__max_iterations)
            axe.set_xlim(0, self.__max_x)
            thread_id += 1
        return ''

    @classmethod
    def handles_entire_report(cls, suites: List['Suite']):
        return False

    @classmethod
    def render_report(cls, raw_suites: List['SuiteHandler']):
        return ''
