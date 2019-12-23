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

from abc import ABC, abstractclassmethod, abstractmethod
from pathlib import Path
from typing import List, TypeVar

from lib.graphing_components import BuildInfo, Datum, Suite


class SummaryFormatter(ABC):

    @abstractclassmethod
    def can_publish(cls: TypeVar("ReportFormatter")) -> bool:
        raise NotImplementedError(
            "ReportFormatter subclass must implement can_publish() function")

    def generate_summary(self,
                         report_files: List[Path], report_summary_file: Path,
                         figure_dpi: int) -> type(None):
        """Render a report (markdown, or HTML) of the report JSON data
        contained in report_files
        Args:
            report_files: List of report JSON files
            report_summary_file: Name of file to write summary to
            figure_dpi: DPI for saving figures
        """

        raise NotImplementedError(
            "ReportFormatter subclass must implement generate_summary() function"
        )
