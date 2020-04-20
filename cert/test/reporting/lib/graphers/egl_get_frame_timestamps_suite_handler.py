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

import lib.graphers as graph_commons
from lib.graphers.suite_handler import SuiteHandler
from lib.report import Suite

class ExtensionAvailability:
    def __init__(self):
        self.has_ext_egl_android_get_frame_timestamps = False
        self.did_enable_surface_egl_timestamps = False
        self.has_fp_get_compositor_timing_supported = False
        self.has_fp_get_compositor_timing = False
        self.has_fp_get_next_frame_id = False
        self.has_fp_get_frame_timestamp_supported = False
        self.has_fp_get_frame_timestamps = False

class EGLGetFrameTimestampsSuiteHandler(SuiteHandler):
    """
    """

    def __init__(self, suite: Suite):
        super().__init__(suite)



        supports_composite_deadline = True
        supports_composite_interval = True
        supports_composite_to_present_latency = True

        supports_requested_present_time = True
        supports_rendering_complete_time = True
        supports_composition_latch_time = True
        supports_first_composition_start_time = True
        supports_last_composition_start_time = True
        supports_first_composition_gpu_finished_time = True
        supports_display_present_time = True
        supports_dequeue_ready_time = True
        supports_reads_done_time = True

        for d in self.data:
            custom = d.custom
            print(custom)

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
        return None
