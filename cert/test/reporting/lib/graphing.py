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

from lib.common import Error
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


def render_report_document(report_files: List[Path], report_summary_file: Path,
                           doc_fmt: str, dpi: int):
    """Render a report (markdown, or HTML) of the report JSON data contained
    in report_files
    Args:
        report_files: List of report JSON files
        report_summary_file: Name of file to write summary Markdown or HTML to
        dpi: DPI for saving figures
    """
    markdown_str = ""
    suites_by_name = {}
    all_suites = []
    folder = report_summary_file.parent

    for report_file in report_files:
        suites = load_suites(report_file)
        all_suites.extend(suites)
        for suite in suites:
            suites_by_name.setdefault(suite.name, []).append(suite)

    for i, suite_name in enumerate(suites_by_name):

        # trim suites list to just those with a handler
        suites = [s for s in suites_by_name[suite_name] if s.handler]

        if suites:
            markdown_str += f"\n# {suite_name}\n"

            for suite in suites:
                base_name = suite.file.stem
                title = suite.identifier()
                plot_file = folder.joinpath("images").joinpath(f"{base_name}.png")
                summary = suite.handler.plot(plot_file, dpi)

                markdown_str += f"\n### {title}\n"
                markdown_str += f"\n![{title}]({str(plot_file.relative_to(folder))})\n"
                if summary:
                    markdown_str += "\n" + summary + "\n"

        # check that all suites handler's are same class
        if len(set([c.__class__ for c in suites])) == 1:
            handler_class = suites[0].handler.__class__
            if handler_class.can_render_summarization_plot(suites):

                filename = f"summary_{i}.png"
                plot_file = folder.joinpath("images").joinpath(filename)

                summary = handler_class.summarization_plot(
                    suites, plot_file, dpi)

                markdown_str += f"\n\n## Summary for:\n{suite_name}\n"
                markdown_str += f"\n![{title}]({str(plot_file.relative_to(folder))})\n"
                if summary:
                    markdown_str += "\n" + summary + "\n"
                markdown_str += "\n---\n"

            else:
                print("Couldn't find a common class to render summary"\
                    f" for suites of name {suite_name}")

    # now attempt to generate cross-suite summarizations by finding
    # all the suites which use the same SuiteHandler
    suites_by_handler_class = {}
    for s in [s for s in all_suites if s.handler]:
        suites_by_handler_class.setdefault(s.handler.__class__, []).append(s)

    for i, handler_class in enumerate(suites_by_handler_class):
        suites = suites_by_handler_class[handler_class]
        if handler_class.can_render_summarization_plot(suites):
            filename = f"meta_summary_{i}.png"
            plot_file = folder.joinpath("images").joinpath(filename)

            summary = handler_class.summarization_plot(suites, plot_file, dpi)

            markdown_str += f"\n\n## Meta Summary\n"
            markdown_str += f"\n![{title}]({str(plot_file.relative_to(folder))})\n"
            if summary:
                markdown_str += "\n" + summary + "\n"
            markdown_str += "\n---\n"

    # ensure the correct document extension
    if report_summary_file.suffix != DocumentFormat.extension(doc_fmt):
        report_summary_file = report_summary_file.parent.joinpath(
            report_summary_file.stem + DocumentFormat.extension(doc_fmt))

    # save the markdown string
    with open(report_summary_file, "w") as fp:
        if doc_fmt == DocumentFormat.HTML:
            fp.write(markdown2.markdown(markdown_str))
        elif doc_fmt == DocumentFormat.MARKDOWN:
            fp.write(markdown_str)
