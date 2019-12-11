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

from lib.graphing_components import *

# -----------------------------------------------------------------------------

# import our renderer implementations
from . import HANDLERS

# -----------------------------------------------------------------------------


def create_suite_handler(suite: Suite):
    """Vend a SuiteHandler implementation which is suitable for given suite
    Args:
        suite: A suite of all data from a given test
    Returns:
        SuiteHandler implementatin which can render this data
    """
    for handler in HANDLERS:
        if handler.can_handle_suite(suite):
            return handler(suite)

    return None
