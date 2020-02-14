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
"""Implements HalfFloatPrecisionSuiteHandler to process
reports for the half float precision test
"""
from typing import List
import matplotlib.pyplot as plt

from lib.graphers.suite_handler import SuiteHandler
from lib.report import Suite


class HalfFloatPrecisionSuiteHandler(SuiteHandler):
    """Implements SuiteHandler for processing half float precision
    test results
    """
    def __init__(self, suite):
        super().__init__(suite)
        self.our_data = [
            d for d in self.data
            if d.operation_id == "HalfFloatPrecisionGLES3Operation"
        ]
        self.error_datums = [
            d for d in self.our_data
            if not d.get_custom_field("half_precision.is_correct")
        ]

    @classmethod
    def can_handle_suite(cls, suite: Suite) -> str:
        return "GPU Half Precision" in suite.name

    @classmethod
    def can_render_summarization_plot(cls,
                                      suites: List['SuiteHandler']) -> bool:
        return False

    @classmethod
    def render_summarization_plot(cls, suites: List['SuiteHandler']) -> str:
        return None

    def render_plot(self):
        # gather data for simple error count histogram
        xs = []
        for datum in self.error_datums:
            val = int(datum.get_custom_field_numeric("half_precision.offset"))
            xs.append(val)

        plt.hist(xs)
        plt.xlabel("offset")
        plt.ylabel("error count")

        if self.error_datums:
            return f"Found {len(self.error_datums)} erroneous values in dataset"
        return None
