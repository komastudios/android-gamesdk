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
from docx.shared import Inches

from lib.summary_formatters.formatter import SummaryFormatter


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

    def on_init(self, title: str, summary_utc: str) -> type(None):
        self.__writer.add_heading(title, 0)
        self.__writer.add_paragraph(summary_utc)

    def on_device(self, device_id: str,
                  plot_path: Path, plot_path_relative: Path,
                  summary: Union[str, type(None)]) -> type(None):
        self.__writer.add_heading(device_id, 3)
        self.__writer.add_picture(str(plot_path), Inches(6.18))
        if summary is not None:
            self.__writer.add_paragraph(summary)

        self.__writer.add_page_break()

    def on_cross_suite(self, plot_path: Path, plot_path_relative: Path,
                       summary: Union[str, type(None)]) -> type(None):
        self.__writer.add_heading("Meta Summary", 2)
        self.__writer.add_picture(str(plot_path), Inches(6.18))
        if summary is not None:
            self.__writer.add_paragraph(summary)
