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

"""
Loader and common functions in summary formatting.
"""

import glob
from pathlib import Path
from typing import Dict, TypeVar, Union

from lib import gdrive
from lib.common import dict_lookup, get_indexable_utc
from lib.summary_components import SummaryFormatter

from .html_formatter import HtmlFormatter
from .markdown_formatter import MarkdownFormatter

__FORMATTERS = {
    "html": HtmlFormatter,
    "md": MarkdownFormatter,
}

# -----------------------------------------------------------------------------


def __get_summary_formatter_type(
        output_format: str
) -> Union[TypeVar("SummaryFormatter"), type(None)]:
    """
    Returns a summary formatter type (or None if the format is unsupported).
    """
    return __FORMATTERS.get(output_format)


def __create_summary_formatter(
        output_format: str
) -> Union[SummaryFormatter, type(None)]:
    """
    Creates a summary formatter instance.
    """
    formatter_type = __get_summary_formatter_type(output_format)

    return None if formatter_type is None else formatter_type()


# -----------------------------------------------------------------------------


def __get_summary_config(recipe: Dict) -> (bool, str, int):
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
    if "summary" in recipe and dict_lookup(
            recipe, "summary.enabled", fallback=False):
        output_format = dict_lookup(recipe, "summary.format", fallback="md")
        dpi = dict_lookup(recipe, "summary.image-resolution", fallback=300)
        publish = dict_lookup(recipe, "summary.publish", fallback=False)

        my_type = __get_summary_formatter_type(output_format)

        if publish is True and my_type is not None:
            if my_type.can_publish() is False:
                print(f"Format {format} invalid for publishing.")
                publish = False

        return True, output_format, dpi, publish

    return False, "", 0, False


def __generate_summary(report_files_dir: Path, output_format: str,
                       figure_dpi: int) -> Path:
    """
    Creates a single doc holding results for all involved devices. Great for a
    comparative, all-up analysis.

    Args:
        report_files_dir: path to the folder holding per-device data.
        output_format: summary format.
        figure_dpi: image resolution.

    Returns:
        a path to the reported summary.
    """
    report_files = [
        Path(f) for f in glob.glob(str(report_files_dir) + '/*.json')
    ]

    report_summary_file_name = \
        f"summary_{get_indexable_utc()}_{str(report_files_dir.stem)}" \
            f".{output_format}"
    report_summary_file = report_files_dir.joinpath(report_summary_file_name)

    summary_formatter = __create_summary_formatter(output_format)
    if summary_formatter is not None:
        summary_formatter.generate_summary(report_files, report_summary_file,
                                           figure_dpi)
    else:
        print(f'No summary formatter for format "{output_format}".')

    return report_summary_file


def perform_summary_if_enabled(recipe: Dict,
                               report_files_dir: Path) -> type(None):
    """
    When the recipe enables summary, this function performs all the steps to
    get it produced.

    Args:
        recipe: the input dictionary with test arguments.
        report_files_dir: a path to the place where all JSON collections got
                          extracted.
    """
    summary = __get_summary_config(recipe)
    if summary[0]:  # enabled
        summary_path = __generate_summary(report_files_dir, summary[1],
                                          summary[2])

        if summary[3]:  # publish to Google Drive
            gdrive.publish(summary_path)
