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

from typing import List

from lib.graphers.common_graphers import graph_functional_test_result
from lib.graphers.suite_handler import SuiteHandler, SuiteSummarizer
from lib.report import Datum, SummaryContext
import lib.items as items


class ExternalFramebufferSummarizer(SuiteSummarizer):
    """Suite summarizer for the External Framebuffer test."""

    @classmethod
    def default_handler(cls) -> SuiteHandler:
        return ExternalFramebufferSuiteHandler


class ExternalFramebufferSuiteHandler(SuiteHandler):
    """
    Implements SuiteHandler for External Framebuffer test results.
    """

    def __init__(self, suite):
        super().__init__(suite)

        self.result_index = 0  # fail, unavailable, success
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
    def can_handle_datum(cls, datum: Datum):
        return 'External Framebuffer' in datum.suite_id

    def render(self, ctx: SummaryContext) -> List[items.Item]:

        def graph():
            graph_functional_test_result(self.result_index,
                                         ['Failed', 'Unavailable', 'Success'])

        image_path = self.plot(ctx, graph, '')
        image = items.Image(image_path, self.device())

        if self.error_msg:
            return [image, items.Text(self.error_msg)]
        return [image]
