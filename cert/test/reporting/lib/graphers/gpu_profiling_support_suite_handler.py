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
"""GPU Profiling Tools Support test suite handler.
"""

from typing import List

from lib.graphers.common_graphers import graph_functional_test_result
from lib.graphers.suite_handler import SuiteHandler, SuiteSummarizer
from lib.report import Datum, SummaryContext
import lib.summary_formatters.format_items as fmt


class GPUProfilingSupportSummarizer(SuiteSummarizer):
    """Suite summarizer for GPU profiling tools support functional test."""
    @classmethod
    def default_handler(cls) -> SuiteHandler:
        return GPUProfilingSupportSuiteHandler


class GPUProfilingSupportSuiteHandler(SuiteHandler):
    """Grapher for GPU profiling tools support functional test (F2.2)."""

    @classmethod
    def can_handle_datum(cls, datum: Datum):
        return "GPU Profiling Tools Support" in datum.suite_id

    def render(self, ctx: SummaryContext) -> List[fmt.Item]:
        result_index = 1  # Inconclusive until proven otherwise
        for datum in self.data:
            if datum.operation_id == 'GPUProfilingSupportOperation':
                supported = datum.get_custom_field('supported')
                if supported is not None:
                    result_index = 2 if supported else 0

        def graph():
            graph_functional_test_result(
                result_index, ['UNSUPPORTED', 'UNDETERMINED', 'SUPPORTED'])

        image_path = self.plot(ctx, graph, '')
        image = fmt.Image(image_path, self.device())
        return [image]
