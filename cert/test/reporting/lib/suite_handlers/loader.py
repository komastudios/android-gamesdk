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

from typing import List

from lib.chart_components import *

# -----------------------------------------------------------------------------

# import our renderer implementations
# TODO(shamyl@google.com): Determine a way to perform this import automatically

from lib.suite_handlers.affinity_test_suite_handler import AffinityTestSuiteHandler
from lib.suite_handlers.memory_allocation_suite_handler import MemoryAllocationSuiteHandler

# -----------------------------------------------------------------------------


def create_suite_handler(suite: Suite):
    """Vend a ChartRenderer implementation which is suitable for a chart's data
    Args:
        chart: A chart of homogenous data type
    Returns:
        ChartRenderer implementatin which can render this data
    """
    handler_classes = [
        AffinityTestSuiteHandler,
        MemoryAllocationSuiteHandler,
    ]

    for hc in handler_classes:
        if hc.can_handle_suite(suite):
            return hc(suite)

    return None
