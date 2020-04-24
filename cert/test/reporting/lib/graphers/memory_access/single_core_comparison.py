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
from lib.report import Datum, ReportContext
import lib.summary_formatters.format_items as fmt


class SingleCoreComparisonSummarizer(SuiteSummarizer):
    @classmethod
    def default_handler(cls) -> SuiteHandler:
        return SingleCoreComparisonHandler

    @classmethod
    def can_handle_datum(cls, datum: Datum):
        return "MA:SCC:" in datum.suite_id


class SingleCoreComparisonHandler(SuiteHandler):
    def __init__(self, suite):
        super().__init__(suite)
        # self.suite_data = [
        #     d for d in self.data
        #     if "MA:SCC:" in d.suite_id
        #     and d.operation_id == "MemoryAccessOperation"
        # ]

#-------------------------------------------------------------------------------
# TODO(tmillican@google.com): Move to general/memory helpers once that exists

    def get_core_size(self, suite, cpu_id):
        cpu_info = suite.suite.build._d
        if cpu_id in cpu_info['big_cores']['cpus']:
            return 'Big'
        elif cpu_id in cpu_info['middle_cores']['cpus']:
            return 'Middle'
        else:
            return 'Little'

    # TODO(tmillican@google.com): Remove duplication with time/bytes
    def get_time(self, datum):
        return int(datum.get_custom_field('time').replace(' nanoseconds', ''))

    def get_bytes(self, datum):
        return int(datum.get_custom_field('bytes').replace(' bytes', ''))

    # TODO(tmillican@google.com): Remove duplication with time/bytes
    # TODO(tmillican@google.com): There's probably a more Python-y way to do this
    def get_max_time(self, suites: List['SingleCoreComparisonHandler']):
        v = 0
        for s in suites:
            if s:
                data = s.suite_data
                data.sort(key=lambda d: self.get_time(d))
                beg = self.get_time(data[0])
                end = self.get_time(data[-1])
                value = end - beg
                v = max(v, value)
        return v

    def get_max_value(self, suites: List['SingleCoreComparisonHandler']):
        v = 0
        for s in suites:
            if s:
                data = s.suite_data
                data.sort(key=lambda d: self.get_bytes(d))
                beg = self.get_bytes(s.suite_data[0])
                end = self.get_bytes(s.suite_data[-1])
                value = end - beg
                v = max(v, value)
        return v

#-------------------------------------------------------------------------------

    def get_linestyle(self, core_size):
        if core_size == 'Big':
            return 'solid'
        elif core_size == 'Middle':
            return 'dashed'
        else:
            return 'dotted'

    def draw_datum_set(self, plot, suites: List['SingleCoreComparisonHandler']):
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
            if not suite:
                continue

            data = suite.suite_data
            # Use the last datum for CPU in case it took a while for set
            # affinity to stick
            linestyle = self.get_linestyle(
                self.get_core_size(suite, data[-1].cpu_id))

            times = []
            values = []
            for d in data:
                times.append(self.get_time(d) / time_div)
                values.append(self.get_bytes(d) / value_div)

            plot.plot(times, values, ls=linestyle)

    #---------------

    # @classmethod
    # def handles_entire_report(self, suites: List[SuiteHandler]):
    #     for suite in suites:
    #         if "MA:SCC:" in suite.name:
    #             return True
    #     return False

    # def render_report_legacy(self, raw_suites: List[SuiteHandler]):

    def render_report(self, ctx: ReportContext) -> List[fmt.Item]:
        # suites = [
        #     s.handler for s in raw_suites
        #     if isinstance(s.handler, SingleCoreComparisonHandler)]

        def graph():
            plt.subplots_adjust(wspace=0.25)

            writes = [
                s for s in suites
                if s.suite_data and 'Write' in s.suite_data[0].suite_id]
            plot = plt.subplot(1, 2, 1, label='Write', title='Write',
                            ylabel='KB Written', xlabel='Milliseconds')
            self.draw_datum_set(plot, writes)

            reads = [
                s for s in suites
                if s.suite_data and 'Read' in s.suite_data[0].suite_id]
            plot = plt.subplot(1, 2, 2, label='Read', title='Read',
                            ylabel='KB Read', xlabel='Milliseconds')
            self.draw_datum_set(plot, reads)

        device = self.suite.identifier()
        image = fmt.Image(self.plot(ctx, graph), device)

        return [image]
