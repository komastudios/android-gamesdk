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
Summary general components.
"""

from abc import ABC, abstractclassmethod, abstractmethod
from pathlib import Path
from typing import ContextManager, List, TypeVar, Union

from lib.common import get_readable_utc
from lib.graphing import load_suites


class SummaryFormatter(ABC):
    """
    Abstract class that encapsulates the common summary generation logic.
    Subclasses go onto the specific for the final format.
    This abstract class emits events (create-summary, init-summary, etc.)
    Subclasses react to these events by properly formatting the summary.
    """

    @abstractclassmethod
    def can_publish(cls: TypeVar("SummaryFormatter")) -> bool:
        """True if the summary formatter subclass can also publish the outcome.
        """
        raise NotImplementedError(
            "SummaryFormatter subclass must implement can_publish()")

    @abstractmethod
    def create_writer(self, summary_path: Path) -> ContextManager:
        """Given a summary path, returns a ContextManager instance that will
        act as the summary writer.

        Args:
            summary_path: the report summary Path.

        Returns: a Python ContextManager to take care of the summary writing.
        """
        raise NotImplementedError(
            "SummaryFormatter subclass must implement create_writer()")

    @abstractmethod
    def init_summary(self, title: str,
                     utc: str, writer: TypeVar("ContextManager")) \
        -> type(None):
        """
        Initiates the summary report.

        Args:
            title: the summary title.
            utc: a string containing the UTC at the time of this report.
            writer: a ContextManager subclass.
        """
        raise NotImplementedError(
            "SummaryFormatter subclass must implement init_summary()")

    @abstractmethod
    def device_summary(self, device_id: str, plot_path: Path,
                       plot_path_relative: Path,
                       summary: Union[str, type(None)],
                       writer: TypeVar("ContextManager")) -> type(None):
        """Appends device results to the summary.

        Args:
            device_id: the device manufacturer, model and API level.
            plot_path: a Path to the device graph for a given suite.
            plot_path_relative: the same path, but relative to the summary
                                being written.
            summary: a string containing some description about the test run on
                    the device, or None.
            writer: a ContextManager subclass.
        """
        raise NotImplementedError(
            "SummaryFormatter subclass must implement device_summary()")

    @abstractmethod
    def suite_summary(self, suite_name: str, device_id: str, plot_path: Path,
                      plot_path_relative: Path,
                      summary: Union[str, type(None)],
                      writer: TypeVar("ContextManager")) -> type(None):
        """Appends a suite summary to the report.

        Args:
            suite_name: self-explanatory.
            device_id: the device manufacturer, model and API level.
            plot_path: a Path to the device graph for a given suite.
            plot_path_relative: the same path, but relative to the summary
                                being written.
            summary: a string containing some description about the test run on
                    the device, or None.
            writer: a ContextManager subclass.
        """
        raise NotImplementedError(
            "SummaryFormatter subclass must implement suite_summary()")

    @abstractmethod
    def cross_suite_summary(self, plot_path: Path, plot_path_relative: Path,
                            summary: Union[str, type(None)],
                            writer: TypeVar("ContextManager")) -> type(None):
        """Appends a cross suite summary to the report.

        Args:
            plot_path: a Path to the device graph for a given suite.
            plot_path_relative: the same path, but relative to the summary
                                being written.
            summary: a string containing some description about the test run on
                    the device, or None.
            writer: a ContextManager subclass.
        """
        raise NotImplementedError(
            "SummaryFormatter subclass must implement cross_suite_summary()")

    def finish_summary(self, writer: TypeVar("ContextManager")) \
        -> type(None):
        """
        Subclasses may override this to write any format-specific closing
        marks.

        Args:
            writer: a ContextManager subclass.
        """

    def generate_summary(self, report_files: List[Path],
                         report_summary_file: Path,
                         figure_dpi: int) -> type(None):
        """Render a report of the report JSON data contained in report_files
        Args:
            report_files: List of report JSON files
            report_summary_file: Name of file to write summary to
            figure_dpi: DPI for saving figures
        """

        suites_by_name = {}
        all_suites = []
        folder = report_summary_file.parent
        image_idx = 0

        def next_image_idx() -> int:
            nonlocal image_idx
            index = image_idx
            image_idx += 1
            return index

        for report_file in report_files:
            suites = load_suites(report_file)
            all_suites.extend(suites)
            for suite in suites:
                suites_by_name.setdefault(suite.name, []).append(suite)

        with self.create_writer(report_summary_file) as file_writer:
            for suite_name in suites_by_name:
                # trim suites list to just those with a handler
                suites = [s for s in suites_by_name[suite_name] if s.handler]

                if suites:
                    self.init_summary(suite_name, get_readable_utc(),
                                      file_writer)

                    for suite in suites:
                        base_name = suite.file.stem
                        device_id = suite.identifier()
                        plot_path = folder.joinpath("images").joinpath(
                            f"{base_name}_{next_image_idx()}.png")
                        self.device_summary(
                            device_id, plot_path, plot_path.relative_to(folder),
                            suite.handler.plot(plot_path, figure_dpi),
                            file_writer)

                # check that all suites handler's are same class
                if len({c.__class__ for c in suites}) == 1:
                    handler_class = suites[0].handler.__class__
                    if handler_class.can_render_summarization_plot(suites):
                        filename = f"summary_{next_image_idx()}.png"
                        plot_path = folder.joinpath("images").joinpath(filename)
                        self.suite_summary(
                            suite_name, device_id, plot_path,
                            plot_path.relative_to(folder),
                            handler_class.summarization_plot(
                                suites, plot_path, figure_dpi), file_writer)
                    else:
                        print("Couldn't find a common class to render summary"\
                            f" for suites of name {suite_name}")

            # now attempt to generate cross-suite summarizations by finding
            # all the suites which use the same SuiteHandler
            suites_by_handler_class = {}
            for s in [s for s in all_suites if s.handler]:
                suites_by_handler_class.setdefault(s.handler.__class__,
                                                   []).append(s)

            for handler_class in suites_by_handler_class:
                suites = suites_by_handler_class[handler_class]
                if handler_class.can_render_summarization_plot(suites):
                    filename = f"meta_summary_{next_image_idx()}.png"
                    plot_path = folder.joinpath("images").joinpath(filename)

                    self.cross_suite_summary(
                        plot_path, plot_path.relative_to(folder),
                        handler_class.summarization_plot(
                            suites, plot_path, figure_dpi), file_writer)

            self.finish_summary(file_writer)
