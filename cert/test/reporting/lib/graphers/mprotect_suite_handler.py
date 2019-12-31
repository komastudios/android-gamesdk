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

import matplotlib.pyplot as plt
import numpy as np

from lib.graphers.suite_handler import SuiteHandler
from lib.report import Suite

class MProtectSuiteHandler(SuiteHandler):
    """Implementation of SuiteHandler to process report data
    from the Vulkan Mprotect test
    """

    def __init__(self, suite):
        super().__init__(suite)

        for datum in self.data:
            if datum.operation_id == "VulkanMprotectCheckOperation":
                self.mprotect_score = datum.get_custom_field("mprotect.rank")

    @classmethod
    def can_handle_suite(cls, suite: Suite):
        return "Vulkan memory write protection" in suite.name

    @classmethod
    def can_render_summarization_plot(cls,
                                      suites: List['SuiteHandler']) -> bool:
        return False

    @classmethod
    def render_summarization_plot(cls, suites: List['SuiteHandler']) -> str:
        return None

    def render_plot(self) -> str:
        if self.mprotect_score is not None:
            score_color = (0, 1, 0) if self.mprotect_score == 0 else (1, 0, 0)
            plt.xticks(np.arange(0))
            plt.yticks(np.arange(0))
            plt.bar(0, 1, color=score_color)

        msg = ""

        if self.mprotect_score is None:
            msg = "No mprotect results found."
        elif self.mprotect_score == 0:
            msg = "PASSED."
        elif self.mprotect_score == 1:
            msg = "R/W protect fail."
        elif self.mprotect_score == 2:
            msg = "Unexpected violation."
        elif self.mprotect_score == 3:
            msg = "Read protect fail."
        elif self.mprotect_score == 4:
            msg = "Missing violation."
        elif self.mprotect_score == 5:
            msg = "Mem mapping fail."
        elif self.mprotect_score == 6:
            msg = "Mem alloc fail."
        elif self.mprotect_score == 7:
            msg = "No mappable memory."
        else:
            msg = f"Unexpected result: ({self.mprotect_score})"

        return msg
