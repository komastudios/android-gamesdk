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

import matplotlib.pyplot as plt
import numpy as np

from lib.graphing_components import Suite, SuiteHandler


class MProtectSuiteHandler(SuiteHandler):

    def __init__(self, suite):
        super().__init__(suite)

        for d in self.data:
            if d.operation_id == "VulkanMprotectCheckOperation":
                self.mprotect_score = d.get_custom_field("mprotect.rank")

    @classmethod
    def can_handle_suite(cls, suite: Suite):
        return "Vulkan memory write protection" in suite.name

    def render_plot(self) -> str:
        if self.mprotect_score is not None:
            score_color = (0, 1, 0) if self.mprotect_score == 0 else (1, 0, 0)
            plt.xticks(np.arange(0))
            plt.yticks(np.arange(0))
            plt.bar(0, 1, color=score_color)

        return self._generate_summary()

    def _generate_summary(self):
        if self.mprotect_score is None:
            return "No mprotect results found."
        elif self.mprotect_score == 0:
            return "PASSED."
        elif self.mprotect_score == 1:
            return "R/W protect fail."
        elif self.mprotect_score == 2:
            return "Unexpected violation."
        elif self.mprotect_score == 3:
            return "Read protect fail."
        elif self.mprotect_score == 4:
            return "Missing violation."
        elif self.mprotect_score == 5:
            return "Mem mapping fail."
        elif self.mprotect_score == 6:
            return "Mem alloc fail."
        elif self.mprotect_score == 7:
            return "No mappable memory."
        else:
            return f"Unexpected result: ({self.mprotect_score})"

