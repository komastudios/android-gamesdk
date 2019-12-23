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

import sys
import json
import markdown2

from pathlib import Path
from typing import Any, Dict, List, Tuple

from lib.common import Error, get_readable_utc, remove_files_with_extensions
from lib.graphing_components import BuildInfo, Datum, Suite
from lib.graphers.loader import create_suite_handler


class UnsupportedReportFormatError(Error):
    """Exception raised when a requesting an unsupported format for
    saving the report

    Attributes:
        message -- explanation of the error
    """

    def __init__(self, message):
        self.message = message


# -----------------------------------------------------------------------------


def cleanup_report_dir(report_dir: Path) -> type(None):
    """
    Removes any existing report-related asset (e.g., .json, .png) that could
    be treated as "brand new" by a brand new report to create.

    Args:
        report_dir: path.
    """
    remove_files_with_extensions(report_dir, [".json", ".csv", ".md", ".html"])
    remove_files_with_extensions(report_dir.joinpath("images"), [".png"])


def load_suites(report_file) -> List[Suite]:
    """Load and configure suites from a given report JSON file
    Args:
        report_file: Report JSON file
    Returns:
        list of Suite instances for which a suitable SuiteHandler was found
    """
    build: BuildInfo = None
    suite_data: Dict[str, List[Datum]] = {}
    suites: List[Suite] = []

    with open(report_file) as f:
        for i, line in enumerate(f):
            jd = json.loads(line)
            # first line is the build info, every other is a report datum
            if i == 0:
                build = BuildInfo.from_json(jd)
            else:
                datum = Datum.from_json(jd)
                suite_data.setdefault(datum.suite_id, []).append(datum)

    for suite_name in suite_data:
        data = suite_data[suite_name]
        suite = Suite(suite_name, build, data, report_file)

        suite.handler = create_suite_handler(suite)
        if not suite.handler:
            print(f"Found no handler for suite_id {suite.name}")

        suites.append(suite)

    return suites


class DocumentFormat:
    MARKDOWN = ".md"
    HTML = ".html"

    @classmethod
    def supported_formats(cls) -> List[str]:
        return [cls.MARKDOWN, cls.HTML]

    @classmethod
    def from_extension(cls, ext) -> str:
        for fmt in cls.supported_formats():
            if ext == fmt or ("." + ext == fmt):
                return fmt

        raise UnsupportedReportFormatError(
            f'Unrecognized document format extension "{ext}"')

    @classmethod
    def extension(cls, fmt) -> str:
        if fmt in cls.supported_formats():
            return fmt
        raise UnsupportedReportFormatError(
            f'Unrecognized document format "{fmt}"')
