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
"""EGL Get Frame Timestamps functional test suite handler.
"""

from typing import List

import matplotlib.pyplot as plt
import lib.graphers as graph_commons
from lib.graphers.suite_handler import SuiteHandler, SuiteSummarizer
from lib.report import Datum, ReportContext, Suite
import lib.summary_formatters.format_items as fmt


class EGLGetFrameTimestampsSummarizer(SuiteSummarizer):
    @classmethod
    def default_handler(cls) -> SuiteHandler:
        return EGLGetFrameTimestampsSuiteHandler

    @classmethod
    def can_handle_datum(cls, datum: Datum):
        return datum.suite_id == 'EGL Get Frame Timestamps' or \
            datum.operation_id == 'EGLGetFrameTimestampsOperation'

    # def render_summary(self, ctx):
    #     sections = super().render_summary(ctx)

    #     def image_plotter():
    #         print('pretending to plot')

    #     image = fmt.Image(self.plot(ctx, image_plotter, 'My Summary Image'))
    #     sections.insert(0, fmt.Section('The Summary', [image]))

    #     return sections


class EGLGetFrameTimestampsSuiteHandler(SuiteHandler):
    """
    """

    def __init__(self, suite: Suite):
        super().__init__(suite)

        self.availability_data = None
        self.composite_data = None
        self.present_time_data = None

        self.next_frame_ids = []
        self.measurements = []
        self.events = []

        for d in self.data:
            custom = d.custom
            if 'has_ext_egl_android_get_frame_timestamps' in custom:
                self.availability_data = custom
            elif 'supports_composite_deadline' in custom:
                self.composite_data = custom
            elif 'supports_requested_present_time' in custom:
                self.present_time_data = custom
            elif 'next_frame_id' in custom:
                self.next_frame_ids.append(custom.get('next_frame_id'))
            elif 'measurement_name' in custom:
                self.measurements.append(custom.get('measurement_name'))
            elif 'event_name' in custom:
                self.events.append(custom.get('event_name'))

        # Check for required fields
        if not self.availability_data \
            or not self.composite_data \
            or not self.present_time_data:
            print("Error: missing data")

        print(len(self.next_frame_ids))
        print(len(self.measurements))
        print(len(self.events))

        # TODO(baxtermichael@google.com): Check for duplicate next_frame_id's.

    def render_report(self, ctx: ReportContext) -> List[fmt.Item]:
        # text = "The EGL_ANDROID_get_frame_timestamps extension has over a " \
        # "dozen features, some of which may not be supported. The major " \
        # "components are given in the \"Summary\" table, while the details of " \
        # "\"Compositor Timing Support\" and \"Frame Timestamp Support\" are " \
        # "given below.\n\n"

        # text += "| Summary | Available | Working |\n"
        # text += "| --- | --- | --- |\n"
        # text += "| Overall Extension | Y | N |\n"
        # text += "| - Compositor Timing Support | N | n/a |\n"
        # text += "| - Frame Timestamp Support | Y | N |\n"
        # text += "| - Get Next Frame ID | Y | Y |\n"
        # text += "| - Surface Timestamp Enabled | Y | Y |\n"
        # text += "\n"
        # text += "***\n"
        # text += "\n"
        # text += "| Compositor Timing Support | Available | Working |\n"
        # text += "| --- | --- | --- |\n"
        # text += "| Composite Deadline | N | n/a |\n"
        # text += "| Composite Interval | N | n/a |\n"
        # text += "| Composite to Present Latency | N | n/a |\n"
        # text += "\n"
        # text += "***\n"
        # text += "\n"
        # text += "| Frame Timestamp Support | Available | Working |\n"
        # text += "| --- | --- | --- |\n"
        # text += "| Requested Present Time | Y | Y |\n"
        # text += "| Requested Present Time | Y | Y |\n"
        # text += "| Rendering Complete Time | N | n/a |\n"
        # text += "| Composition Latch Time | Y | Y |\n"
        # text += "| First Composition Start Time | Y | N |\n"
        # text += "| Last Composition Start Time | Y | Y |\n"
        # text += "| First Composition GPU Finished Time | Y | Y |\n"
        # text += "| Display Present Time | Y | Y |\n"
        # text += "| Dequeue Ready Time | Y | Y |\n"
        # text += "| Reads Done Time | Y | Y |\n"
        # text += "\n"
        # text += "***\n"
        # text += "\n"
        # text += " Frame Timestamp Support (Available, Working)\n"
        # text += "\n"
        # text += "- (Y, Y) Requested Present Time\n"
        # text += "- (Y, Y) Requested Present Time\n"
        # text += "- (N, n/a) Rendering Complete Time\n"
        # text += "- (Y, Y) Composition Latch Time\n"
        # text += "- (Y, N) First Composition Start Time\n"
        # text += "- (Y, Y) Last Composition Start Time\n"
        # text += "- (Y, Y) First Composition GPU Finished Time\n"
        # text += "- (Y, Y) Display Present Time\n"
        # text += "- (Y, Y) Dequeue Ready Time\n"
        # text += "- (Y, Y) Reads Done Time\n"

        device = self.suite.identifier()

        para = fmt.Paragraph(f'Here is a paragraph.')
        image = fmt.Image(self.plot(ctx, self.handle_plot, self.title()), device)
        header = fmt.Heading("Ye ol' header", 4)
        image2 = fmt.Image(self.plot(ctx, self.handle_plot2, 'Plot 2'), 'Plot 2')
        table = fmt.Table(['One', 'Two'], [['a', 'b'], ['c', 'd']])

        return [para, image, header, image2, table]

    def handle_plot(self):
        fig, ax = plt.subplots()

        cols = ['one', 'two']
        rows = ['4', '5']
        cell_text = [['a', 'b'], ['c', 'd']]

        table = ax.table(colLabels=cols, \
            rowLabels=rows, \
            cellText=cell_text)

    def handle_plot2(self):
        plt.plot([1, 3, 5, 7, 9, 11, 13, 17, 19])
