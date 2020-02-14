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
from typing import ContextManager, TypeVar, Union


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

    @abstractmethod
    def on_init(self, title: str, summary_utc: str) -> type(None):
        """Writes formatted initial data (title, report time, etc.)

        Args:
            title: the summary title.
            summary_utc: string containing the UTC at the time of this report.
        """
        raise NotImplementedError(
            "SummaryFormatter subclass must implement on_init()")

    @abstractmethod
    def on_device(self, device_id: str, plot_path: Path,
                  plot_path_relative: Path,
                  summary: Union[str, type(None)]) -> type(None):
        """Writes formatted results for a given device.

        Args:
            device_id: the device manufacturer, model and API level.
            plot_path: a Path to the device graph for a given suite.
            plot_path_relative: the same path, but relative to the summary
                                being written.
            summary: a string containing some description about the test run on
                     the device, or None.
        """
        raise NotImplementedError(
            "SummaryFormatter subclass must implement on_device()")

    @abstractmethod
    def on_cross_suite(self, plot_path: Path, plot_path_relative: Path,
                       summary: Union[str, type(None)]) -> type(None):
        """Writes a formatted cross suite summary to the report.

        Args:
            plot_path: a Path to the device graph for a given suite.
            plot_path_relative: the same path, but relative to the summary
                                being written.
            summary: a string containing some description about the test run on
                     the device, or None.
        """
        raise NotImplementedError(
            "SummaryFormatter subclass must implement on_cross_suite()")

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
