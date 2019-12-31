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
"""Helper functions to load report JSON documents and generate
reports.
"""

import json
from pathlib import Path
import platform
from typing import Dict, List

import markdown2
import matplotlib
import matplotlib.pyplot as plt

from lib.common import Error
from lib.report import BuildInfo, Datum, Suite
from lib.graphers.loader import create_suite_handler

#-------------------------------------------------------------------------------

if platform.system() == "Linux":
    matplotlib.use('gtk3cairo')

SMALL_SIZE = 5
MEDIUM_SIZE = 7
BIGGER_SIZE = 10

plt.rc('font', size=SMALL_SIZE)  # controls default text sizes
plt.rc('axes', titlesize=SMALL_SIZE)  # fontsize of the axes title
plt.rc('axes', labelsize=MEDIUM_SIZE)  # fontsize of the x and y labels
plt.rc('xtick', labelsize=SMALL_SIZE)  # fontsize of the tick labels
plt.rc('ytick', labelsize=SMALL_SIZE)  # fontsize of the tick labels
plt.rc('legend', fontsize=SMALL_SIZE)  # legend fontsize
plt.rc('figure', titlesize=BIGGER_SIZE)  # fontsize of the figure title

# ------------------------------------------------------------------------------


class UnsupportedReportFormatError(Error):
    """Exception raised when a requesting an unsupported format for
    saving the report

    Attributes:
        message -- explanation of the error
    """

    def __init__(self, message):
        super().__init__()
        self.message = message


#-------------------------------------------------------------------------------


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

    with open(report_file) as file:
        for i, line in enumerate(file):
            line_dict = json.loads(line)
            # first line is the build info, every other is a report datum
            if i == 0:
                build = BuildInfo.from_json(line_dict)
            else:
                datum = Datum.from_json(line_dict)
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
    """DocumentFormat
    Helper for representing supported document formats for
    saving reports.
    Example:
        render_report_document(in_files, out_file, DocumentFormat.HTML)
    """
    MARKDOWN = ".md"
    HTML = ".html"

    @classmethod
    def supported_formats(cls) -> List[str]:
        """Supported document format types
        Returns:
            list of supported document formats
        """
        return [cls.MARKDOWN, cls.HTML]

    @classmethod
    def from_extension(cls, ext) -> str:
        """Parse a file extension to its matching document format
        Args:
            ext: a file extension, e.g., ".md"
        Returns:
            The corresponding document format
        Raises:
            UnsupportedReportFormatError if the document extension
            doesn't represent a supported document format
        """
        for fmt in cls.supported_formats():
            if fmt in (ext, "." + ext):
                return fmt

        raise UnsupportedReportFormatError(
            f'Unrecognized document format extension "{ext}"')

    @classmethod
    def extension(cls, fmt) -> str:
        """Get the file extension for a given document format
        Returns:
            the corresponding file extension for a given DocumentFormat
        Raises:
            UnsupportedReportFormatError for an unrecognized document format
        """
        if fmt in cls.supported_formats():
            return fmt
        raise UnsupportedReportFormatError(
            f'Unrecognized document format "{fmt}"')


def __render_suite_reports(suites_by_name, folder, dpi, next_image_idx) -> str:
    markdown_str = ""
    for suite_name in suites_by_name:

        # trim suites list to just those with a handler
        suites = [s for s in suites_by_name[suite_name] if s.handler]

        if suites:
            markdown_str += f"\n# {suite_name}\n"

            for suite in suites:
                base_name = suite.file.stem
                title = suite.identifier()
                plot_file = folder.joinpath("images").joinpath(
                    f"{base_name}_{next_image_idx()}.png")
                summary = suite.handler.plot(plot_file, dpi)

                markdown_str += f"\n### {title}\n"
                markdown_str += \
                    f"\n![{title}]({str(plot_file.relative_to(folder))})\n"

                if summary:
                    markdown_str += "\n" + summary + "\n"

        # check that all suites handler's are same class
        if len({c.__class__ for c in suites}) == 1:
            handler_class = suites[0].handler.__class__
            if handler_class.can_render_summarization_plot(suites):

                filename = f"summary_{next_image_idx()}.png"
                plot_file = folder.joinpath("images").joinpath(filename)

                summary = handler_class.summarization_plot(
                    suites, plot_file, dpi)

                markdown_str += f"\n\n## Summary for:\n{suite_name}\n"
                markdown_str += \
                    f"\n![{title}]({str(plot_file.relative_to(folder))})\n"

                if summary:
                    markdown_str += "\n" + summary + "\n"

                markdown_str += "\n---\n"

            else:
                print("Couldn't find a common class to render summary"\
                    f" for suites of name {suite_name}")

    return markdown_str


def __render_cross_suite_summarizations(all_suites, folder, markdown_str, dpi,
                                        next_image_idx):
    # now attempt to generate cross-suite summarizations by finding
    # all the suites which use the same SuiteHandler
    suites_by_handler_class = {}
    for s in [s for s in all_suites if s.handler]:
        suites_by_handler_class.setdefault(s.handler.__class__, []).append(s)

    for handler_class in suites_by_handler_class:
        suites = suites_by_handler_class[handler_class]
        if handler_class.can_render_summarization_plot(suites):
            filename = f"meta_summary_{next_image_idx()}.png"
            plot_file = folder.joinpath("images").joinpath(filename)

            summary = handler_class.summarization_plot(suites, plot_file, dpi)

            markdown_str += f"\n\n## Meta Summary\n"
            markdown_str += \
                f"\n![Meta Summary]({str(plot_file.relative_to(folder))})\n"

            if summary:
                markdown_str += "\n" + summary + "\n"

            markdown_str += "\n---\n"

    return markdown_str


def render_report_document(report_files: List[Path], report_summary_file: Path,
                           doc_fmt: str, dpi: int):
    """Render a report (markdown, or HTML) of the report JSON data contained
    in report_files
    Args:
        report_files: List of report JSON files
        report_summary_file: Name of file to write summary Markdown or HTML to
        dpi: DPI for saving figures
    """
    suites_by_name = {}
    all_suites = []
    folder = report_summary_file.parent
    image_idx = 0

    def next_image_idx() -> int:
        nonlocal image_idx
        index = image_idx
        image_idx += 1
        return index

    for report_file in report_files:
        suites = load_suites(report_file)
        all_suites.extend(suites)
        for suite in suites:
            suites_by_name.setdefault(suite.name, []).append(suite)

    markdown_str = __render_suite_reports(suites_by_name, folder, dpi,
                                          next_image_idx)

    markdown_str = __render_cross_suite_summarizations(all_suites, folder,
                                                       markdown_str, dpi,
                                                       next_image_idx)

    # ensure the correct document extension
    if report_summary_file.suffix != DocumentFormat.extension(doc_fmt):
        report_summary_file = report_summary_file.parent.joinpath(
            report_summary_file.stem + DocumentFormat.extension(doc_fmt))

    # save the markdown string
    with open(report_summary_file, "w") as file:
        if doc_fmt == DocumentFormat.HTML:
            file.write(markdown2.markdown(markdown_str))
        elif doc_fmt == DocumentFormat.MARKDOWN:
            file.write(markdown_str)
