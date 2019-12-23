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
from typing import Union, TypeVar

from docx import Document

from lib.summary_components import SummaryFormatter


class WordFormatter(SummaryFormatter):
    """Word summary formatter."""

    @classmethod
    def can_publish(cls: TypeVar("SummaryFormatter")) -> bool:
        return True

    @contextmanager
    def create_writer(self, summary_path: Path) -> Document:
        document_writer = Document()

        try:
            yield document_writer
        finally:
            document_writer.save(str(summary_path))

    def init_summary(self, title: str,
                     utc: str, writer: Document) \
        -> type(None):
        writer.add_heading(title, 0)
        writer.add_paragraph(utc)

    def device_summary(self, device_id: str,
                       plot_path: Path, plot_path_relative: Path,
                       summary: Union[str, type(None)], writer: Document) \
                           -> type(None):
        writer.add_heading(device_id, 3)
        writer.add_picture(str(plot_path))
        if summary is not None:
            writer.add_paragraph(summary)

        writer.add_page_break()

    def suite_summary(self, suite_name: str, device_id: str,
                      plot_path: Path, plot_path_relative: Path,
                      summary: Union[str, type(None)], writer: Document) \
                           -> type(None):
        writer.add_heading(f"Summary for {suite_name}", 2)
        writer.add_picture(str(plot_path))
        if summary is not None:
            writer.add_paragraph(summary)

        writer.add_page_break()

    def cross_suite_summary(self,
                            plot_path: Path, plot_path_relative: Path,
                            summary: Union[str, type(None)], writer: Document) \
                                -> type(None):
        writer.add_heading("Meta Summary", 2)
        writer.add_picture(str(plot_path))
        if summary is not None:
            writer.add_paragraph(summary)

        writer.add_page_break()
