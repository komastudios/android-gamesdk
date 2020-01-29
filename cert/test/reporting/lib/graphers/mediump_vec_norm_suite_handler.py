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
"""Implements MediumPVecNormSuiteHandler for processing
reports from mediump vec normalization test
"""

import json
from typing import List
import matplotlib.pyplot as plt

from lib.graphers.suite_handler import SuiteHandler
from lib.report import Suite


class MediumPVecNormSuiteHandler(SuiteHandler):
    """SuiteHandler implementation for schedule affinity operation test
    """

    @classmethod
    def can_handle_suite(cls, suite: Suite):
        return "MediumPVecNormalizationGLES3Operation" in suite.data_by_operation_id

    @classmethod
    def can_render_summarization_plot(cls,
                                      suites: List['SuiteHandler']) -> bool:
        return False

    @classmethod
    def render_summarization_plot(cls, suites: List['SuiteHandler']) -> str:
        return None

    def render_plot(self) -> str:
        my_data = self.suite.data_by_operation_id[
            "MediumPVecNormalizationGLES3Operation"]

        device = self.suite.build["DEVICE"]
        for datum in my_data:
            is_failure = datum.get_custom_field(
                "mediump_vec_normalization_result.failure")
            is_expected_failure = datum.get_custom_field(
                "mediump_vec_normalization_result.expected_failure")

            if is_failure and not is_expected_failure:
                expected = json.dumps(
                    datum.custom["mediump_vec_normalization_result"]
                    ["expected"])
                actual = json.dumps(
                    datum.custom["mediump_vec_normalization_result"]["actual"])
                print(
                    f"Got an unexpected failure from device:{device} expected: {expected} actual:{actual} file:{str(self.suite.file)}"
                )

        return None
