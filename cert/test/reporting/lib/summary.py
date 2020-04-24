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

import json
import glob
from pathlib import Path
import re
from typing import List, Optional

from lib.common import get_indexable_utc, get_readable_utc, Indexer
from lib.device import DeviceCatalog
from lib.gdrive import GDrive, GoogleDriveRelatedError
from lib.graphing import load_suite_summarizers
from lib.recipe import Recipe
from lib.report import ReportContext
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
                print(f"Format {output_format} invalid for publishing. "
                      "Skipping.")
                publish = False

        return True, output_format, dpi, publish

    return False, "", 0, False


def __default_summary_name_based_on_directory(directory: str) -> str:
    """Tries to infer the best summary name from specific patterns in the
    directory it's being stored."""
    directory_parts = re.match(r"^out\/([^\/]+)(\/?.*)$", directory)

    return directory_parts.group(
        1) if directory_parts and len(directory_parts.groups()) >= 1 else ""


# ------------------------------------------------------------------------------


def __generate_formatted_summaries(summary_path: Path, reports: List[Path],
                                   formatter: SummaryFormatter,
                                   figure_dpi: int) -> type(None):
    """Generates summaries and meta summaries for each report. Allows handlers
    to determine which of these to write (or possibly both)."""
    indexer = Indexer()
    suite_summarizers = load_suite_summarizers(reports)
    if not suite_summarizers:
        print("Warning: no handlers found")

    utc = get_readable_utc()
    ctx = ReportContext(summary_path.parent, figure_dpi, indexer)
    for summarizer in suite_summarizers:
        formatter.on_summary(summarizer.title(), utc,
                             summarizer.render_summary(ctx))


def __generate_formatted_summary_from_errors(excluded: List[Path],
                                             formatter: SummaryFormatter
                                            ) -> type(None):
    """Flushes through the formatter all the info pertaining to devices whose
    runs finished abnormally."""
    if len(excluded) < 1:
        return

    formatter.on_errors_available("Failed Tests")
    for excluded_path in excluded:
        with open(excluded_path, "r") as excluded_file:
            exclusion_data = json.load(excluded_file)
            device_info = DeviceCatalog()[exclusion_data["codename"]]
            if device_info is None:
                # If brand & model lookup fails, just go with codename
                device_info = f'Codename "{exclusion_data["codename"]}"'
            formatter.on_device_error(
                repr(device_info), f'{exclusion_data["outcome"]}: '
                f'{exclusion_data["test_details"]}')


def __generate_formatted_summary_from_reports(summary_path: Path,
                                              reports: List[Path],
                                              excluded: List[Path],
                                              formatter: SummaryFormatter,
                                              figure_dpi: int) -> type(None):
    """Writes a formatted summary file, collated from a list of JSON reports.
    Args:
        summary_path: directory where generated files will be saved.
        reports: list of paths to successful device test reports.
        excluded: list of paths to device test run errors.
        formatter: handles writing formatted summaries.
        figure_dpi: image resolution.
    """
    with formatter.create_writer(summary_path):
        __generate_formatted_summaries(summary_path, reports, formatter,
                                       figure_dpi)
        __generate_formatted_summary_from_errors(excluded, formatter)


def generate_summary(reports: List[Path],
                     excluded: List[Path],
                     output_format: str,
                     figure_dpi: int,
                     name: str = "") -> Optional[Path]:
    """
    Creates a single doc holding results for all involved devices. Great for a
    comparative, all-up analysis.

    Args:
        reports: list of paths to successful device test reports.
        excluded: list of paths to device test run errors.
        output_format: summary format.
        figure_dpi: image resolution.
        name: the name of this summary.

    Returns:
        a path to the reported summary.
    """
    if len(reports) + len(excluded) == 0:
        return None

    summary_dir = reports[0].parent if len(reports) > 0 else excluded[0].parent

    if not name:
        name = __default_summary_name_based_on_directory(str(summary_dir))

    summary_file_name = f"summary_{get_indexable_utc()}_{name}.{output_format}"
    summary_path = summary_dir.joinpath(summary_file_name)

    summary_formatter = create_summary_formatter(output_format)
    if summary_formatter is not None:
        __generate_formatted_summary_from_reports(summary_path, reports,
                                                  excluded, summary_formatter,
                                                  figure_dpi)
    else:
        print(f'No summary formatter for format "{output_format}".')

    return summary_path


# ------------------------------------------------------------------------------


def perform_summary_if_enabled(recipe: Recipe, reports_dir: Path) -> type(None):
    """
    When the recipe enables summary, this function performs all the steps to
    get it produced. It also attempts to publish the summary to Google Drive
    if the recipe is configured with `"summary.publish": true`.

    Args:
        recipe: the input dictionary with test arguments.
        reports_dir: a path to the place where all JSON collections got
                     extracted.
    """
    summary = __get_summary_config(recipe)
    if summary[0]:  # enabled
        summary_path = generate_summary([
            Path(f) for f in sorted(glob.glob(f"{reports_dir}/*_report*.json"))
        ], [Path(f) for f in sorted(glob.glob(f"{reports_dir}/*_error*.json"))],
                                        summary[1], summary[2],
                                        recipe.get_name())

        if summary[3]:  # publish to Google Drive
            try:
                GDrive().publish(summary_path)
                print(f"{summary_path} published to Google Drive.")
            except GoogleDriveRelatedError as error:
                print(f"""{summary_path} publishing to Google Drive failure:
{error}""")
