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
Word summary formatter.
"""

from contextlib import contextmanager
from pathlib import Path
from typing import List, TypeVar

from docx.oxml.ns import nsdecls
from docx.oxml import parse_xml
from docx import Document
from docx.shared import Inches

from lib.summary_formatters.formatter import SummaryFormatter
import lib.summary_formatters.format_items as fmt


class WordFormatter(SummaryFormatter):
    """Word summary formatter."""

    def __init__(self):
        self.__writer = None

    @classmethod
    def is_gdrive_compatible(cls: TypeVar("SummaryFormatter")) -> bool:
        return True

    @contextmanager
    def create_writer(self, summary_path: Path) -> Document:
        self.__writer = Document()

        try:
            yield self.__writer
        finally:
            self.__writer.save(str(summary_path))

    # --------------------------------------------------------------------------

    def on_errors_available(self, title: str) -> type(None):
        self.write_heading(fmt.Heading(title, 2))

    def on_device_error(self, device_id: str, summary: str) -> type(None):
        self.write_heading(fmt.Heading(device_id, 4))
        self.write_text(fmt.Text(summary))

    # --------------------------------------------------------------------------

    def write_separator(self):
        self.__writer.add_page_break()

    def write_heading(self, heading: fmt.Heading):
        self.__writer.add_heading(heading.text, heading.level - 1)

    def write_text(self, text: fmt.Text):
        self.__writer.add_paragraph(text.text)

    def write_image(self, image: fmt.Image):
        self.__writer.add_picture(str(image.path), Inches(6.18))

    def write_table(self, table: fmt.Table):
        col_count = max(len(row) for row in table.rows)
        doc_table = self.__writer.add_table(rows=1, cols=col_count)
        doc_table.style = self.__writer.styles['Light Grid']

        def set_background(cell, color):
            cell._tc.get_or_add_tcPr().append(
                parse_xml(f'<w:shd {{}} w:fill="{color}"/>'.format(
                    nsdecls('w'))))

        header = doc_table.rows[0].cells
        for i, col in enumerate(table.header):
            header[i].text = col
            set_background(header[i], "D9D9D9")

        for row in table.rows:
            cells = doc_table.add_row().cells
            for i, col in enumerate(row):
                cells[i].text = col
                set_background(cells[i], "FFFFFF")

        self.write_text(fmt.Text("")) # extra line for bottom margin
