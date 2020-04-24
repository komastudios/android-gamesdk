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
"""Provides MProtectSuiteHandler which processes report data
from the Vulkan Mprotect test
"""

from typing import List

from lib.graphers.common_graphers import graph_functional_test_result
from lib.graphers.suite_handler import SuiteHandler, SuiteSummarizer
from lib.report import Datum, SummaryContext
import lib.summary_formatters.format_items as fmt


class MProtectSummarizer(SuiteSummarizer):
    """Suite summarizer for the Vulkan mprotect test."""
    @classmethod
    def default_handler(cls) -> SuiteHandler:
        return MProtectSuiteHandler


class MProtectSuiteHandler(SuiteHandler):
    """Implementation of SuiteHandler to process report data
    from the Vulkan Mprotect test
    """

    def __init__(self, suite):
        super().__init__(suite)

        for datum in self.data:
            if datum.operation_id == "VulkanMprotectCheckOperation":
                self.mprotect_score = datum.get_custom_field("mprotect.score")

    @classmethod
    def can_handle_datum(cls, datum: Datum):
        return "Vulkan memory write protection" in datum.suite_id

    def render(self, ctx: SummaryContext) -> List[fmt.Item]:
        result_index = 0
        msg = None

        if self.mprotect_score is None:
            result_index = 1
            msg = "No mprotect results found."
        elif self.mprotect_score == 0:
            result_index = 2
            msg = "PASSED."
        elif self.mprotect_score == 1:
            msg = "R/W protect fail."
        elif self.mprotect_score == 2:
            msg = "Unexpected write violation."
        elif self.mprotect_score == 3:
            msg = "Read-only protect fail."
        elif self.mprotect_score == 4:
            msg = "Missing write violation."
        elif self.mprotect_score == 5:
            msg = "Mem mapping fail."
        elif self.mprotect_score == 6:
            msg = "Mem alloc fail."
        elif self.mprotect_score == 7:
            msg = "No mappable memory."
        elif self.mprotect_score == 8:
            msg = "Vulkan isn't supported on this device."
        else:
            result_index = 1
            msg = f"Unexpected result: ({self.mprotect_score})"

        def graph():
            graph_functional_test_result(
                result_index, ['UNAVAILABLE', 'UNDETERMINED', 'PASSED'])

        image = fmt.Image(self.plot(ctx, graph), self.device())
        return [image, fmt.Text(msg)]
