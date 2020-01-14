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
"""Helper functions and classes for generating and formatting test summaries.
"""

import glob
from pathlib import Path
from typing import Dict, List, Union

from lib.gdrive import GDrive, GoogleDriveRelatedError
from lib.common import get_indexable_utc, get_readable_utc, Recipe
from lib.graphing import load_suites
from lib.report import Suite
from lib.summary_formatters.formatter import SummaryFormatter
from lib.summary_formatters.loader \
    import create_summary_formatter, get_summary_formatter_type


def __get_summary_config(recipe: Recipe) -> (bool, str, int):
    """
    Extracts, from the recipe, summary related arguments.
    These are:

    * enabled: a boolean telling whether a summary is enabled at all.
    * format: a file extension to specify if the summary format. By default,
              markdown.
    * dpi: the desired image resolution (300 if missing).
    * publish: a boolean to indicate that the resulting document must be
               uploaded to Google Drive.
    """
    if recipe.lookup("summary.enabled", fallback=False):
        output_format = recipe.lookup("summary.format", fallback="md")
        dpi = recipe.lookup("summary.image-resolution", fallback=300)
        publish = recipe.lookup("summary.publish", fallback=False)

        my_type = get_summary_formatter_type(output_format)

        if publish is True and my_type is not None:
            if my_type.is_gdrive_compatible() is False:
                print(f"Format {format} invalid for publishing. Skipping.")
                publish = False

        return True, output_format, dpi, publish

    return False, "", 0, False


class Indexer:
    """Utility to get incremental indices."""

    def __init__(self):
        self.__index = 0

    def next_index(self) -> int:
        """Returns incremental indices starting from 0 (zero)."""
        index = self.__index
        self.__index += 1
        return index


def __make_image_path(folder: Path, prefix: str, indexer: Indexer) -> Path:
    """Creates image paths base on a folder and incremental indices."""

    return folder.joinpath("images").joinpath(
        f"{prefix}_{indexer.next_index()}.png")


#------------------------------------------------------------------------------


def __generate_formatted_summary_from_suites(suites_by_name: Dict[str, Suite],
                                             folder: Path,
                                             formatter: SummaryFormatter,
                                             figure_dpi: int) -> type(None):
    image_indexer = Indexer()
    for suite_name in suites_by_name:
        # trim suites list to just those with a handler
        suites = [s for s in suites_by_name[suite_name] if s.handler]

        if suites:
            formatter.on_init(suite_name, get_readable_utc())

            for suite in suites:
                base_name = suite.file.stem
                plot_path = __make_image_path(folder, base_name, image_indexer)
                formatter.on_device(suite.identifier(), plot_path,
                                    plot_path.relative_to(folder),
                                    suite.handler.plot(plot_path, figure_dpi))


def __generate_formatted_cross_suite_summary(all_suites: List[Suite],
                                             folder: Path,
                                             formatter: SummaryFormatter,
                                             figure_dpi: int):
    image_indexer = Indexer()
    # now attempt to generate cross-suite summarizations by finding
    # all the suites which use the same SuiteHandler
    suites_by_handler_class = {}
    for s in [s for s in all_suites if s.handler]:
        suites_by_handler_class.setdefault(s.handler.__class__, []).append(s)

    for handler_class in suites_by_handler_class:
        suites = suites_by_handler_class[handler_class]
        if handler_class.can_render_summarization_plot(suites):
            plot_path = __make_image_path(folder, "meta_summary", image_indexer)
            formatter.on_cross_suite(
                plot_path, plot_path.relative_to(folder),
                handler_class.summarization_plot(suites, \
                                                 plot_path, figure_dpi))


def __generate_formatted_summary_from_reports(summary_path: Path,
                                              reports: List[Path],
                                              formatter: SummaryFormatter,
                                              figure_dpi: int) -> type(None):
    """Iterates through a list of JSON reports, creating graphics and appending
    the result formatted to the summary
    """
    suites_by_name = {}
    all_suites = []
    folder = summary_path.parent

    for report in reports:
        suites = load_suites(report)
        all_suites.extend(suites)
        for suite in suites:
            suites_by_name.setdefault(suite.name, []).append(suite)

    with formatter.create_writer(summary_path):
        __generate_formatted_summary_from_suites(suites_by_name, folder,
                                                 formatter, figure_dpi)

        __generate_formatted_cross_suite_summary(all_suites, folder, formatter,
                                                 figure_dpi)


def generate_summary(reports: List[Path], output_format: str,
                     figure_dpi: int) -> Union[Path, None]:
    """
    Creates a single doc holding results for all involved devices. Great for a
    comparative, all-up analysis.

    Args:
        reports: path to the folder holding per-device data.
        output_format: summary format.
        figure_dpi: image resolution.

    Returns:
        a path to the reported summary.
    """
    if len(reports) == 0:
        return None

    reports_dir = reports[0].parent

    summary_file_name = \
        f"summary_{get_indexable_utc()}_{reports_dir.stem}" \
            f".{output_format}"
    summary_path = reports_dir.joinpath(summary_file_name)

    summary_formatter = create_summary_formatter(output_format)
    if summary_formatter is not None:
        __generate_formatted_summary_from_reports(summary_path, reports,
                                                  summary_formatter, figure_dpi)
    else:
        print(f'No summary formatter for format "{output_format}".')

    return summary_path


#------------------------------------------------------------------------------


def perform_summary_if_enabled(recipe: Recipe, reports_dir: Path) -> type(None):
    """
    When the recipe enables summary, this function performs all the steps to
    get it produced.

    Args:
        recipe: the input dictionary with test arguments.
        reports_dir: a path to the place where all JSON collections got
                     extracted.
    """
    summary = __get_summary_config(recipe)
    if summary[0]:  # enabled
        summary_path = generate_summary(
            [Path(f) for f in sorted(glob.glob(f"{reports_dir}/*.json"))],
            summary[1], summary[2])

        if summary[3]:  # publish to Google Drive
            try:
                GDrive().publish(summary_path)
                print(f"{summary_path} published to Google Drive.")
            except GoogleDriveRelatedError as error:
                print(f"""{summary_path} publishing to Google Drive failure:
{repr(error)}""")
