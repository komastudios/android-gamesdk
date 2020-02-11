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
"""Provides CpusetSuiteHandler which gets graphics for the cpuset functional
test.
"""

from typing import List

import matplotlib.pyplot as plt
import numpy as np

from lib.graphers.suite_handler import SuiteHandler
from lib.report import Suite


class CpusetSuiteHandler(SuiteHandler):
    """Grapher for the cpuset functional test (F2.1)."""

    def __init__(self, suite: Suite):
        super().__init__(suite)

        self.enabled = False
        self.sane = False

        for datum in self.data:
            stage = datum.get_custom_field("stage")
            if stage == "Test summary":
                self.enabled = datum.get_custom_field("cpuset_enabled")
                self.sane = datum.get_custom_field("bigger_cores_first")
                break

    @classmethod
    def can_handle_suite(cls, suite: Suite):
        return "Sane CPUSET" in suite.name

    @classmethod
    def can_render_summarization_plot(cls,
                                      suites: List['SuiteHandler']) -> bool:
        return False

    @classmethod
    def render_summarization_plot(cls, suites: List['SuiteHandler']) -> str:
        return None

    def render_plot(self) -> str:
        plt.gcf().set_figheight(1.5)

        if self.enabled:
            if self.sane:
                msg = "CPUSET is enabled and CPU core numbers are assigned " \
                "from big cores to LITTLE ones."
                score_color = (0, 1, 0)
            else:
                msg = "CPUSET is enabled on this device but CPU core numbers" \
                " aren't assigned from big cores to LITTLE ones."
                score_color = (1, 1, 0)
        else:
            msg = "CPUSET isn't enabled on this device."
            score_color = (1, 0, 0)

        plt.xticks(np.arange(0))
        plt.yticks(np.arange(0))
        plt.bar(0, 1, color=score_color)

        return msg

    @classmethod
    def handles_entire_report(cls, suites: List['SuiteHandler']):
        return False

    @classmethod
    def render_report(cls, raw_suites: List['SuiteHandler']):
        return ""
