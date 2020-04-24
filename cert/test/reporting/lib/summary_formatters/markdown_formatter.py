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
"""Markdown summary formatter."""

from pathlib import Path
from typing import List, TextIO, TypeVar

from lib.summary_formatters.formatter import SummaryFormatter
import lib.summary_formatters.format_items as fmt


class MarkdownFormatter(SummaryFormatter):
    """Markdown summary formatter."""

    def __init__(self):
        self.__writer = None
        self.__summary_path = None

    @classmethod
    def is_gdrive_compatible(cls: TypeVar("SummaryFormatter")) -> bool:
        return False

    def create_writer(self, summary_path: Path) -> TextIO:
        self.__summary_path = summary_path
        self.__writer = open(summary_path, "w")
        return self.__writer

    def on_init(self, title: str, summary_utc: str) -> type(None):
        self.__write_header(fmt.Header(title, 1))
        self.__write_paragraph(fmt.Paragraph(summary_utc))

    # --------------------------------------------------------------------------

    def __write_horizontal_line(self):
        self.__writer.write(f'---\n\n')

    def __write_header(self, header: fmt.Header):
        level = ''.join(['#' for i in range(header.level)])
        self.__writer.write(f'{level} {header.text}\n\n')

    def __write_paragraph(self, paragraph: fmt.Paragraph):
        self.__writer.write(f'{paragraph.text}\n\n')

    def __write_image(self, image: fmt.Image):
        relative_path = image.path.relative_to(self.__summary_path.parent)
        self.__writer.write(
            f'<img src="{relative_path}" width="90%" alt="{image.alt}" />\n\n')

    def __write_table(self, table: fmt.Table):

        def row_to_str(cells: List[str]):
            return '| ' + ' | '.join(cell for cell in cells) + ' |\n'

        text = row_to_str(table.header)
        text += row_to_str(['---' for cell in table.header])
        for row in table.rows:
            text += row_to_str(row)
        self.__writer.write(f'{text}\n')

    def __write_items(self, items: List[fmt.Item]):
        for item in items:
            if isinstance(item, fmt.Paragraph):
                self.__write_paragraph(item)
            elif isinstance(item, fmt.Header):
                self.__write_header(item)
            elif isinstance(item, fmt.Image):
                self.__write_image(item)
            elif isinstance(item, fmt.Table):
                self.__write_table(item)
            else:
                self.__writer.write(f'{item}\n\n')  # try to write it anyway

    # --------------------------------------------------------------------------

    def on_device(self, device_id: str, items: List[fmt.Item]) -> type(None):
        self.__write_header(fmt.Header(device_id, 3))
        self.__write_items(items)
        self.__write_horizontal_line()

    def on_cross_suite(self, items: List[fmt.Item]) -> type(None):
        self.__write_header(fmt.Header('Meta Summary', 2))
        self.__write_items(items)
        self.__write_horizontal_line()

    def on_errors_available(self, title: str) -> type(None):
        self.__write_header(fmt.Header(title, 2))

    def on_device_error(self, device_id: str, summary: str) -> type(None):
        self.__write_header(fmt.Header(device_id, 3))
        self.__write_paragraph(fmt.Paragraph(summary))
        self.__write_horizontal_line()
