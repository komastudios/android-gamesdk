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
"""Base class for all summary formatters.
"""

from abc import ABC, abstractclassmethod, abstractmethod
from pathlib import Path
from typing import ContextManager, List, Optional, TypeVar

import lib.summary_formatters.format_items as fmt


class SummaryFormatter(ABC):
    """Abstract class that encapsulates the common events that occur during a
    summary generation.
    Subclasses define, before each event, how to get format.
    """

    @abstractclassmethod
    def is_gdrive_compatible(cls: TypeVar("SummaryFormatter")) -> bool:
        """True if the format is accepted by Google Drive.
        """
        raise NotImplementedError(
            "SummaryFormatter subclass must implement is_gdrive_compatible()")

    @abstractmethod
    def create_writer(self, summary_path: Path) -> ContextManager:
        """Given a summary path, returns a writer instance for the formatter.
        Its type is left to each SummaryFormatter subtype.

        Args:
            summary_path: the report summary Path.

        Returns: a Python ContextManager to take care of the summary writing.
        """
        raise NotImplementedError(
            "SummaryFormatter subclass must implement create_writer()")

    # --------------------------------------------------------------------------

    def on_summary(self, title: str, date: str,
                   items: List[fmt.Item]) -> type(None):
        """Writes a formatted summary with title, date, and format items

        Args:
            title: the title to appear as an H1.
            date: the date string to be printed under a title
            items: a list of format items.
        """
        self.write_heading(fmt.Heading(title, 1))
        self.write_paragraph(fmt.Paragraph(date))
        self.write_items(items)
        if not isinstance(items[-1], fmt.Separator):
            self.write_separator()

    @abstractmethod
    def on_errors_available(self, title: str) -> type(None):
        """Writes a title for a section on failed tests.

        Args:
            title: Failed tests header.
        """
        raise NotImplementedError(
            "SummaryFormatter subclass must implement on_errors_available()")

    @abstractmethod
    def on_device_error(self, device_id: str, summary: str) -> type(None):
        """Writes formatted error results for a given device.

        Args:
            device_id: the device manufacturer, model and API level.
            summary: a string containing some description about the failure.
        """
        raise NotImplementedError(
            "SummaryFormatter subclass must implement on_device_error()")

    def on_finish(self) -> type(None):
        """(Optional) Writes formatted closing marks to the summary."""

    # --------------------------------------------------------------------------

    @abstractmethod
    def write_separator(self):
        raise NotImplementedError(
            "SummaryFormatter subclass must implement __write_separator()")

    @abstractmethod
    def write_heading(self, heading: fmt.Heading):
        raise NotImplementedError(
            "SummaryFormatter subclass must implement __write_heading()")

    @abstractmethod
    def write_paragraph(self, paragraph: fmt.Paragraph):
        raise NotImplementedError(
            "SummaryFormatter subclass must implement __write_paragraph()")

    @abstractmethod
    def write_image(self, image: fmt.Image):
        raise NotImplementedError(
            "SummaryFormatter subclass must implement __write_image()")

    @abstractmethod
    def write_table(self, table: fmt.Table):
        raise NotImplementedError(
            "SummaryFormatter subclass must implement __write_table()")

    def write_items(self, items: List[fmt.Item]):
        for item in items:
            if isinstance(item, fmt.Separator):
                self.write_separator()
            elif isinstance(item, fmt.Paragraph):
                self.write_paragraph(item)
            elif isinstance(item, fmt.Heading):
                self.write_heading(item)
            elif isinstance(item, fmt.Image):
                self.write_image(item)
            elif isinstance(item, fmt.Table):
                self.write_table(item)
