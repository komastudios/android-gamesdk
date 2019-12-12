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


def render_report_document(report_files: List[Path], report_summary_file: Path,
                           dpi):
    """Render a report (markdown, or HTML) of the report JSON data contained
    in report_files
    Args:
        report_files: List of report JSON files
        report_summary_file: Name of file to write summary Markdown or HTML to
        dpi: DPI for saving figures
    """
    markdown_str = ""
    suites_by_name = {}
    folder = report_summary_file.parent

    if not report_summary_file.suffix in [".md", ".html"]:
        raise UnsupportedReportFormatError(
            f"Unsupported format {report_summary_file.suffix} for saving report document"
        )

    save_as_html = report_summary_file.suffix == ".html"

    for report_file in report_files:
        for suite in load_suites(report_file):
            suites_by_name.setdefault(suite.name, []).append(suite)

    for suite_name in suites_by_name:
        markdown_str += f"""
# {suite_name}
{get_readable_utc()}
"""

        suites = suites_by_name[suite_name]

        for i, suite in enumerate(suites):
            if suite.handler:
                base_name = suite.file.stem
                title = suite.identifier()
                summary = suite.handler.summary()
                plot_file = folder.joinpath("images").joinpath(
                    f"{base_name}.png")
                suite.handler.plot(False, plot_file_name=plot_file, dpi=dpi)

                markdown_str += f"""
### {title}

<img src="{str(plot_file.relative_to(folder))}" width="90%" alt="{title}" />

{summary if summary else ""}

---
"""

    with open(report_summary_file, "w") as fp:
        if save_as_html:
            html_str = markdown2.markdown(markdown_str)
            fp.write(html_str)
        else:
            fp.write(markdown_str)
