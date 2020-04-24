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
"""Provides base class SuiteHandler

When processing report data, lib.reporting.Suite instances are created
which respectively contain report datums of homogenous suite_id. To process
and analyze the contents of these suites, implement subclasses of SuiteHandler
to have a domain-specific analysis of the suite's data.
"""

from abc import ABC, abstractclassmethod, abstractmethod
from pathlib import Path
from typing import Callable, List, Optional

import matplotlib.pyplot as plt
import numpy as np

from lib.common import ensure_dir, nanoseconds_to_seconds
from lib.report import Datum, ReportContext, Suite
import lib.summary_formatters.format_items as fmt
from lib.common import Indexer


# ------------------------------------------------------------------------------


class SuiteHandler(ABC): # TODO: (baxtermichael@google.com): update
    """SuiteHandler handles data for a report suite.
    Custom suite handler implemenations may select to render analysis
    and plots, etc, of collections of Datum instances of the same suite

    Custom implementations at minimum need to implement:
        - can_handle_suite
        - render_plt

    Custom implementations may choose to implement:
        - can_render_summarization_plot
        - render_summarization_plot (if the above is implemented)

    """

    def __init__(self, suite: Suite):
        super().__init__()
        self.suite = suite

        # all data
        self.data = suite.data

        # datums stored by cpu_id
        self.data_by_cpu_id = {}
        for datum in self.data:
            self.data_by_cpu_id.setdefault(datum.cpu_id, []).append(datum)

        # sorted list of cpu_ids
        self.cpu_ids = list(self.data_by_cpu_id.keys())
        self.cpu_ids.sort()

    @abstractmethod
    def render_report(self, ctx: ReportContext) -> List[fmt.Item]:
        """Subclasses implement this method to render their portion of the
        report document.
        Args:
            ctx: context used to determine dpi and default paths to save images
        Return:
            List of items that can be written out by the document Formatter.
            This can include Headers, Paragraphs, Images, Tables, etc., as well
            as raw strings.
        """
        raise NotImplementedError(
            "SuiteHandler subclass must implement render_report() function")

    def title(self):
        """The title of the figure rendered in render_plot()

        Override this method to return a custom title for your plot.
        """
        return self.suite.description()

    # convenience functions

    def plot(self,
             ctx: ReportContext,
             plotter: Callable[[], None],
             title: str = '',
             path: Optional[Path] = None) -> Path:
        """Plot and save a graph as a PNG image.
        Args:
            ctx: context used to determine dpi and default path to save image
            plotter: callback function, used to modify the graph
            title: graph's title
            path: save figure PNG image to this file
        Returns:
            The path to where the image was saved
        """
        if not path:
            prefix = self.suite.file.stem
            path = make_image_path(ctx.folder, prefix, ctx.indexer)

        make_plot(path, title, ctx.dpi, plotter)
        return path

    def get_xs_as_nanoseconds(self) -> List[int]:
        """Helper function to get an array of datum timestamps as nanoseconds"""
        return np.array([d.timestamp for d in self.data])

    def get_x_axis_as_seconds(self):
        """Helper function to get an array of datum timestamps as seconds"""
        x_values = self.get_xs_as_nanoseconds()
        return nanoseconds_to_seconds(x_values - x_values[0])

    def get_ys(self) -> List[float]:
        """Helper function to get an array of datum numeric values"""
        return np.array([d.numeric_value for d in self.data])


# ------------------------------------------------------------------------------


class SuiteSummarizer:
    def __init__(self, reports: List[Suite]): # TODO: change to Report
        """Gets a list of all reports (copies) it can handle.
        Gives all reports with all datum that pass the filter_datum test
        (by default, those that pass can_handle_datum).
        """
        self.reports = reports
        for report in reports:
            report.data = list(filter(self.filter_datum, report.data))
        default_handler = self.default_handler()
        self.suite_handlers = [default_handler(r) for r in self.reports]

    @abstractclassmethod
    def default_handler(cls) -> SuiteHandler:
        """If more than one SuiteHandler is needed, override __init__."""
        raise NotImplementedError(
            "SuiteSummarizer subclass must implement default_handler() function")

    @classmethod
    def filter_datum(cls, datum: Datum):
        """Which data to keep in a report"""
        return cls.can_handle_datum(datum)

    @abstractclassmethod
    def can_handle_datum(cls, datum: Datum):
        """Determines which datum to include when Suites are passed to the
        __init__ method (thereby also determining which reports are considered).
        """
        raise NotImplementedError(
            "SuiteSummarizer subclass must implement can_handle_datum() function")

    @classmethod
    def can_handle_report(cls, report: Suite): # TODO: change to Report
        """An instance _may_ override this if it's desirable"""
        for datum in report.data:
            if cls.can_handle_datum(datum):
                return True
        return False

    def render_summary(self, ctx: ReportContext) -> List[fmt.Section]:
        """An instance _may_ override this if it's desirable"""
        sections = []
        for handler in self.suite_handlers:
            title = handler.suite.identifier() # TODO: .report
            items = handler.render_report(ctx)
            sections.append(fmt.Section(title, items))
        return sections

    def plot(self,
             ctx: ReportContext,
             plotter: Callable[[], None],
             title: str = '',
             path: Optional[Path] = None) -> Path:
        """Plot and save a graph as a PNG image.
        Args:
            ctx: context used to determine dpi and default path to save image
            plotter: callback function, used to modify the graph
            title: graph's title
            path: save figure PNG image to this file
        Returns:
            The path to where the image was saved
        """
        if not path:
            # TODO(baxtermichael@google.com): consider using title as URI prefix
            path = make_image_path(ctx.folder, 'summary', ctx.indexer)

        make_plot(path, title, ctx.dpi, plotter)
        return path


# ------------------------------------------------------------------------------


def make_image_path(folder: Path,
                      prefix: str,
                      indexer: Optional[Indexer] = None) -> Path:
    """Creates an image path based on folder, prefix and optional increment.
        Args:
            folder: path relative to where script is run
            prefix: the file name prefix (example: "folder/prefix_1.png")
            indexer: optional indexer for incrementing the prefix name to avoid
                name collision
        Returns:
            The path where a png can be saved.
        """
    name = f'{prefix}'
    if indexer:
        name += f'_{indexer.next_index()}'
    return folder.joinpath("images").joinpath(f'{name}.png')


def make_plot(path: Path, title: str, dpi: int, plotter: Callable[[], None]):
    """Plot and save a graph as a PNG image.
    Args:
        path: save figure PNG image to this file
        title: graph's title
        dpi: resolution for image
        plotter: callback function, used to modify the graph
    """
    ensure_dir(path)
    plt.ioff()
    plotter()
    if title:
        plt.suptitle(title)
    plt.savefig(str(path), dpi=dpi)
    plt.close()
