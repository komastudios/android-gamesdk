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

from abc import ABCMeta, abstractclassmethod
from typing import TypeVar


class SummaryFormatter(ABCMeta):

    @abstractclassmethod
    def can_publish(cls: TypeVar("ReportFormatter")) -> bool:
        raise NotImplementedError(
            "ReportFormatter subclass must implement can_publish() function")
