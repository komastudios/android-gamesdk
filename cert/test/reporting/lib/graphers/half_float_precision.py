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

from lib.graphing_components import *


class HalfFloatPrecisionSuiteHandler(SuiteHandler):

    def __init__(self, suite):
        super().__init__(suite)
        self.our_data = [
            d for d in self.data
            if d.operation_id == "HalfFloatPrecisionGLES3Operation"
        ]
        self.error_datums = [
            d for d in self.our_data
            if d.get_custom_field("half_precision.is_correct") == False
        ]

    @classmethod
    def can_handle_suite(cls, suite: Suite) -> str:
        return "GPU Half Precision" in suite.name

    def render_plot(self):
        # gather data for simple error count histogram
        xs = []
        for d in self.error_datums:
            v = int(d.get_custom_field_numeric("half_precision.offset"))
            xs.append(v)

        plt.hist(xs)
        plt.xlabel("offset")
        plt.ylabel("error count")

        if len(self.error_datums) > 0:
            return f"Found {len(self.error_datums)} erroneous values in dataset"
        return None
