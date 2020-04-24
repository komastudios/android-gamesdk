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

from typing import List

import matplotlib.pyplot as plt
import matplotlib.lines as lines

from lib.graphers.suite_handler import SuiteHandler, SuiteSummarizer
from lib.report import Datum, SummaryContext, Suite
import lib.summary_formatters.format_items as fmt


class SingleCoreComparisonSummarizer(SuiteSummarizer):
    @classmethod
    def default_handler(cls) -> SuiteHandler:
        return SingleCoreComparisonHandler

    @classmethod
    def can_handle_datum(cls, datum: Datum):
        return "MA:SCC:" in datum.suite_id \
        and datum.operation_id == "MemoryAccessOperation"


class SingleCoreComparisonHandler(SuiteHandler):
    def __init__(self, suite):
        super().__init__(suite)

        self.data_by_id = {}
        for datum in suite:
            self.data_by_id.setdefault(datum.suite_id, []).append(datum)

        self.suites = []
        for suite_id, data in self.data_by_id.items():
            self.suites.append(Suite(suite_id, suite.build, data, suite.file))

        print(self.suites)

#-------------------------------------------------------------------------------
# TODO(tmillican@google.com): Move to general/memory helpers once that exists

    def get_core_size(self, suite: Suite, cpu_id: int) -> str:
        cpu_info = suite.build._d
        if cpu_id in cpu_info['big_cores']['cpus']:
            return 'Big'
        elif cpu_id in cpu_info['middle_cores']['cpus']:
            return 'Middle'
        else:
            return 'Little'

    # TODO(tmillican@google.com): Remove duplication with time/bytes
    def get_time(self, datum: Datum) -> int:
        return int(datum.get_custom_field('time').replace(' nanoseconds', ''))

    def get_bytes(self, datum: Datum) -> int:
        return int(datum.get_custom_field('bytes').replace(' bytes', ''))

    # TODO(tmillican@google.com): Remove duplication with time/bytes
    # TODO(tmillican@google.com): There's probably a more Python-y way to do this
    def get_max_time(self, suites: List[Suite]) -> int:
        v = 0
        for s in suites:
            data = s.data[:]
            data.sort(key=lambda d: self.get_time(d))
            beg = self.get_time(data[0])
            end = self.get_time(data[-1])
            value = end - beg
            v = max(v, value)
        return v

    def get_max_value(self, suites: List[Suite]) -> int:
        v = 0
        for s in suites:
            data = s.data[:]
            data.sort(key=lambda d: self.get_bytes(d))
            beg = self.get_bytes(s.suite_data[0])
            end = self.get_bytes(s.suite_data[-1])
            value = end - beg
            v = max(v, value)
        return v

#-------------------------------------------------------------------------------

    def get_linestyle(self, core_size) -> str:
        if core_size == 'Big':
            return 'solid'
        elif core_size == 'Middle':
            return 'dashed'
        else:
            return 'dotted'

    def draw_datum_set(self, plot, suites: List[Suite]):
        time_div = 1000000 # NS to MS
        value_div = 1024 # Bytes to KB

        # TODO(tmillican@google.com): This seems like something
        #  that should be more straightforward to do, but I
        #  don't know Python that well.
        max_time = self.get_max_time(suites) / time_div
        max_value = self.get_max_value(suites) / value_div

        plt.xlim(0, max_time)
        plt.ylim(0, max_value)

        plt.legend([
                lines.Line2D([], [], ls=self.get_linestyle('Big'), c='k'),
                lines.Line2D([], [], ls=self.get_linestyle('Middle'), c='k'),
                lines.Line2D([], [], ls=self.get_linestyle('Little'), c='k')
            ], ['Big Cores', 'Middle Cores', 'Little Cores'])

        # For big/middle/little
        for suite in suites:
            # Use the last datum for CPU in case it took a while for set
            # affinity to stick
            linestyle = self.get_linestyle(
                self.get_core_size(suite, suite.data[-1].cpu_id))

            times = []
            values = []
            for d in suite.data:
                times.append(self.get_time(d) / time_div)
                values.append(self.get_bytes(d) / value_div)

            plot.plot(times, values, ls=linestyle)

    #---------------

    def render_report(self, ctx: SummaryContext) -> List[fmt.Item]:
        writes = [s for s in self.suites if 'Write' in s.suite_id]
        reads = [s for s in self.suites if 'Read' in s.suite_id]

        def graph():
            plt.subplots_adjust(wspace=0.25)
            plot = plt.subplot(1, 2, 1, label='Write', title='Write',
                            ylabel='KB Written', xlabel='Milliseconds')
            self.draw_datum_set(plot, writes)

            plot = plt.subplot(1, 2, 2, label='Read', title='Read',
                            ylabel='KB Read', xlabel='Milliseconds')
            self.draw_datum_set(plot, reads)

        image = fmt.Image(self.plot(ctx, graph), self.device())
        return [image]
