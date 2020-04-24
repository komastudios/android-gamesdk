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
"""Provides VulkanVaryingsHandler which processes report data
from the Vulkan Varyings test
"""

from typing import List

import matplotlib.pyplot as plt
import numpy as np

from lib.graphers.suite_handler import SuiteHandler, SuiteSummarizer
from lib.report import Datum, SummaryContext
import lib.summary_formatters.format_items as fmt


class VulkanVaryingsSummarizer(SuiteSummarizer):
    """Suite summarizer for the Vulkan Varyings test."""
    @classmethod
    def default_handler(cls) -> SuiteHandler:
        return VulkanVaryingsHandler

    @classmethod
    def can_handle_datum(cls, datum: Datum):
        return "Vulkan Varyings Blocks" in datum.suite_id


class VulkanVaryingsHandler(SuiteHandler):
    """Implementation of SuiteHandler to process report data
    from the Vulkan Varyings test
    """

    def __init__(self, suite):
        super().__init__(suite)

        self.found_data = False

        for datum in self.data:
            if datum.operation_id == "VulkanVaryingsTestOperation":
                self.found_data = True
                self.initialization_success = datum.get_custom_field("initialization_success")
                self.max_block_components = datum.get_custom_field("max_block_components")
                self.max_single_components = datum.get_custom_field("max_single_components")
                self.reported_max_components = datum.get_custom_field("reported_max_components")

    def render_report(self, ctx: SummaryContext) -> List[fmt.Item]:

        def graph():
            #plt.gcf().set_figheight(1.5)
            if self.found_data:
                block_color = (0, 1, 0) if self.max_block_components >= \
                    self.reported_max_components else (1, 0, 0)
                single_color = (0, 1, 0) if self.max_single_components >= \
                    self.reported_max_components else (1, 0, 0)
                yticks = list(set([0, self.max_block_components,
                                self.max_single_components,
                                self.reported_max_components]))
                plt.xticks([0, 1], ["block", "single"])
                plt.yticks(yticks)
                plt.bar(0, self.max_block_components, color=block_color)
                plt.bar(1, self.max_single_components, color=single_color)

        image = fmt.Image(self.plot(ctx, graph), self.device())
        return [image]
