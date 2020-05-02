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
"""
A grapher for the External Framebuffer operation.
"""

import os
import re
from typing import List, Optional

import matplotlib.pyplot as plt
from matplotlib.patches import Rectangle
import numpy as np

from lib.graphers.common_graphers import graph_functional_test_result
from lib.graphers.suite_handler import SuiteHandler
from lib.report import Suite


class ExternalFramebufferSuiteHandler(SuiteHandler):
    """
    Implements SuiteHandler for External Framebuffer test results.
    """

    def __init__(self, suite):
        super().__init__(suite)

        self.result_index = 0 # fail, unavailable, success
        self.error_msg = None
        for row in self.data:
            if 'measured_red' in row.custom:
                success = row.custom['success']
                self.result_index = 2
                if not success:
                    self.result_index = 0
                    self.error_msg = 'Value read back from framebuffer did '\
                        'not match the expected value.'
                break
            if 'indicates_failure' in row.custom \
                and 'error_message' in row.custom:
                self.error_msg = row.custom['error_message']
                # failed or unavailable
                self.result_index = 0 if row.custom['indicates_failure'] else 1
                break


    @classmethod
    def can_handle_suite(cls, suite: Suite):
        return 'External Framebuffer' in suite.name

    @classmethod
    def can_render_summarization_plot(cls, suites: List['Suite']) -> bool:
        return False

    @classmethod
    def render_summarization_plot(cls, suites: List['Suite']) -> str:
        return None

    def render_plot(self) -> str:
        graph_functional_test_result(self.result_index,
            ['Failed', 'Unavailable', 'Success'])
        return self.error_msg
