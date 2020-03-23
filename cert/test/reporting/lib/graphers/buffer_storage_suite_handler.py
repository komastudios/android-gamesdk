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
"""Implements a suite handler to process reports for OpenGL ES buffer storage
compliance tests.
"""

from typing import List
import matplotlib.pyplot as plt
import numpy as np

from lib.graphers.suite_handler import SuiteHandler
from lib.report import Suite


class BufferStorageSuiteHandler(SuiteHandler):
    """Implements SuiteHandler for OpenGL ES buffer storage compliance test
    results.
    """

    def __init__(self, suite):
        super().__init__(suite)

        for datum in self.data:
            if datum.operation_id == "BufferStorageGLES3Operation":
                self.test_result_status = datum.get_custom_field(
                    "buffer_storage.status")

    @classmethod
    def can_handle_suite(cls, suite: Suite):
        return "GLES3 Buffer Storage" in suite.name

    @classmethod
    def can_render_summarization_plot(cls,
                                      suites: List['SuiteHandler']) -> bool:
        return False

    @classmethod
    def render_summarization_plot(cls, suites: List['SuiteHandler']) -> str:
        return None

    def render_plot(self) -> str:
        plt.gcf().set_figheight(3)
        if self.test_result_status is not None:
            score_color = (0, 1, 0) \
            if self.test_result_status == 0 else (1, 0, 0)
            plt.xticks(np.arange(0))
            plt.yticks(np.arange(0))
            plt.bar(0, 1, color=score_color)

        msg = None
        if self.test_result_status is None:
            msg = "Test result status not found."
        elif self.test_result_status == 0:
            msg = "Passed."
        elif self.test_result_status == 1:
            msg = "Feature not found as OpenGL ES extension."
        elif self.test_result_status == 2:
            msg = "Feature not found in OpenGL ES driver library."
        elif self.test_result_status == 3:
            msg = "Issues allocating a mutable bufffer store."
        elif self.test_result_status == 4:
            msg = "Issues deallocating a mutable bufffer store."
        elif self.test_result_status == 5:
            msg = "Issues allocating an immutable bufffer store."
        elif self.test_result_status == 6:
            msg = "Unexpected success deallocating an immutable buffer store."
        else:
            msg = f"Unexpected result: ({self.test_result_status})"

        return msg

    @classmethod
    def handles_entire_report(cls, suites: List['Suite']):
        return False

    @classmethod
    def render_report(cls, raw_suites: List['SuiteHandler']):
        return ''
