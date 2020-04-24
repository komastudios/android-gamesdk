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
"""Nougat Crash functional test suite handler.
"""

from typing import List

import matplotlib.pyplot as plt
import lib.graphers as graph_commons
from lib.graphers.suite_handler import SuiteHandler
from lib.report import Suite

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


    @classmethod
    def can_handle_suite(cls, suite: Suite):
        return 'EGL Get Frame Timestamps' in suite.name

    @classmethod
    def can_render_summarization_plot(cls, suites: List['Suite']) -> bool:
        return False

    @classmethod
    def render_summarization_plot(cls, suites: List['Suite']) -> str:
        return None

    @classmethod
    def handles_entire_report(cls, suites: List['Suite']):
        return False

    @classmethod
    def render_report(cls, raw_suites: List['Suite']):
        return ''

    def render_plot(self) -> str:
        text = "The EGL_ANDROID_get_frame_timestamps extension has over a " \
        "dozen features, some of which may not be supported. The major " \
        "components are given in the \"Summary\" table, while the details of " \
        "\"Compositor Timing Support\" and \"Frame Timestamp Support\" are " \
        "given below.\n\n"

        text += "| Summary | Available | Working |\n"
        text += "| --- | --- | --- |\n"
        text += "| Overall Extension | Y | N |\n"
        text += "| - Compositor Timing Support | N | n/a |\n"
        text += "| - Frame Timestamp Support | Y | N |\n"
        text += "| - Get Next Frame ID | Y | Y |\n"
        text += "| - Surface Timestamp Enabled | Y | Y |\n"
        text += "\n"
        text += "***\n"
        text += "\n"
        text += "| Compositor Timing Support | Available | Working |\n"
        text += "| --- | --- | --- |\n"
        text += "| Composite Deadline | N | n/a |\n"
        text += "| Composite Interval | N | n/a |\n"
        text += "| Composite to Present Latency | N | n/a |\n"
        text += "\n"
        text += "***\n"
        text += "\n"
        text += "| Frame Timestamp Support | Available | Working |\n"
        text += "| --- | --- | --- |\n"
        text += "| Requested Present Time | Y | Y |\n"
        text += "| Requested Present Time | Y | Y |\n"
        text += "| Rendering Complete Time | N | n/a |\n"
        text += "| Composition Latch Time | Y | Y |\n"
        text += "| First Composition Start Time | Y | N |\n"
        text += "| Last Composition Start Time | Y | Y |\n"
        text += "| First Composition GPU Finished Time | Y | Y |\n"
        text += "| Display Present Time | Y | Y |\n"
        text += "| Dequeue Ready Time | Y | Y |\n"
        text += "| Reads Done Time | Y | Y |'\n"

        fig, ax = plt.subplots()

        cols = ['one', 'two']
        rows = ['1', '2']
        cell_text = [['a', 'b'], ['c', 'd']]

        table = ax.table(colLabels=cols, \
            rowLabels=rows, \
            cellText=cell_text)

        return text
