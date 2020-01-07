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
"""Provides a factory function to gather the right summary formatter given a
format.
"""

from typing import TypeVar, Union

from lib.summary_formatters.formatter import SummaryFormatter

from lib.summary_formatters.html_formatter import HtmlFormatter
from lib.summary_formatters.markdown_formatter import MarkdownFormatter
from lib.summary_formatters.word_formatter import WordFormatter

__FORMATTERS = {
    "docx": WordFormatter,
    "html": HtmlFormatter,
    "md": MarkdownFormatter,
}

#------------------------------------------------------------------------------


def get_summary_formatter_type(
        output_format: str
) -> Union[TypeVar("SummaryFormatter"), type(None)]:
    """
    Returns a summary formatter type (or None if the format is unsupported).
    """
    return __FORMATTERS.get(output_format)


def create_summary_formatter(
        output_format: str
) -> Union[SummaryFormatter, type(None)]:
    """
    Creates a summary formatter instance.
    """
    formatter_type = get_summary_formatter_type(output_format)

    return None if formatter_type is None else formatter_type()
