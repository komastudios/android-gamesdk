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
"""SuiteHandler implementation for "Memory allocation" test
"""

from typing import List

from matplotlib.collections import EventCollection
import matplotlib.pyplot as plt
from matplotlib.ticker import FormatStrFormatter
import numpy as np

from lib.common import bytes_to_megabytes, nanoseconds_to_seconds
from lib.graphers.suite_handler import SuiteHandler, SuiteSummarizer
from lib.report import Datum, SummaryContext
import lib.summary_formatters.format_items as fmt


class MemoryAllocationSuiteSummarizer(SuiteSummarizer):
    """Suite summarizer for Memory Allocation tests."""
    @classmethod
    def default_handler(cls) -> SuiteHandler:
        return MemoryAllocationSuiteHandler

    @classmethod
    def can_handle_datum(cls, datum: Datum):
        return "Memory allocation" in datum.suite_id


class MemoryAllocationSuiteHandler(SuiteHandler):
    """SuiteHandler for Memory Allocation tests
    """

    def __init__(self, suite):
        super().__init__(suite)

        self.__first_timestamp = self.data[0].timestamp
        self.__last_timestamp = self.data[-1].timestamp

        self.__max_total_mem = 0

        self.__xs = []
        self.__ys_test_alloc = []
        self.__ys_sys_free = []

        self.__xs_free = []

    def render_report(self, ctx: SummaryContext) -> List[fmt.Item]:
        for datum in self.data:
            if datum.operation_id == 'MemoryAllocOperation':
                self.__segregate_datum_info(datum)
        self.__post_process_data_points()

        def graph():
            self.__arrange_mem_figure(plt.subplot(1, 1, 1))

        image = fmt.Image(self.plot(ctx, graph), self.device())
        return [image]

    def __segregate_datum_info(self, datum: Datum) -> type(None):
        """Receives a datum and uses its info to fill the various data arrays.
        """
        if datum.get_custom_field("is_on_trim") or \
            datum.get_custom_field("is_restart"):
            return

        x = datum.timestamp - self.__first_timestamp

        if datum.get_custom_field("is_free"):
            self.__xs_free.append(x)
            return

        test_alloc = datum.get_custom_field_numeric("total_allocation_bytes")
        sys_free = \
            datum.get_custom_field_numeric("sys_mem_info.available_memory")

        total_mem = test_alloc + sys_free
        if self.__max_total_mem < total_mem:
            self.__max_total_mem = total_mem

        self.__xs.append(x)
        self.__ys_test_alloc.append(test_alloc)
        self.__ys_sys_free.append(sys_free)

    def __post_process_data_points(self) -> type(None):
        """Massages data points for rendering with MatPlotLib"""
        self.__xs = nanoseconds_to_seconds(np.array(self.__xs))
        self.__ys_test_alloc = \
            bytes_to_megabytes(np.array(self.__ys_test_alloc))
        self.__ys_sys_free = bytes_to_megabytes(np.array(self.__ys_sys_free))

        self.__xs_free = nanoseconds_to_seconds(np.array(self.__xs_free))

        self.__max_total_mem = bytes_to_megabytes(self.__max_total_mem)

    def __arrange_mem_figure(self, fig) -> None:
        """Disposes all graphic components to represent memory usage."""
        fig.stackplot(self.__xs,
                      self.__ys_test_alloc,
                      self.__ys_sys_free,
                      labels=["Allocated", "Available"])
        fig.legend(loc='upper left')

        fig.set_xlim(
            0,
            nanoseconds_to_seconds(self.__last_timestamp -
                                   self.__first_timestamp))
        fig.set_ylim(0, self.__max_total_mem)

        free_events = EventCollection(self.__xs_free,
                                      color='black',
                                      linelength=self.__max_total_mem * 2,
                                      linewidth=0.5,
                                      linestyle='dashed')
        fig.add_collection(free_events)

        fig.get_xaxis().set_major_formatter(FormatStrFormatter('%ds'))
        fig.get_yaxis().set_major_formatter(FormatStrFormatter('%dMb'))
