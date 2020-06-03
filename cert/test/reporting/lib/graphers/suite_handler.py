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
from lib.report import Datum, Report, Suite, SummaryContext
import lib.items
from lib.common import Indexer


# ------------------------------------------------------------------------------


class SuiteHandler(ABC):
    """SuiteHandler handles data for a report suite.
    Custom suite handler implementations may select to render analysis
    and plots, etc, of collections of Datum instances of the same suite

    Custom implementations need to implement:
        - render
        - can_handle_datum
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

    @abstractclassmethod
    def can_handle_datum(cls, datum: Datum):
        """Determines which datum the SuiteHandler is capable of handling.
        """
        raise NotImplementedError(
            "SuiteHandler subclass must implement can_handle_datum() function")

    @abstractmethod
    def render(self, ctx: SummaryContext) -> List[lib.items.Item]:
        """Subclasses implement this method to render their portion of the
        report document.
        Args:
            ctx: context used to determine dpi and default paths to save images
        Return:
            List of items that can be written out by the document Formatter.
            This can include any subclass of Item, such as Header, Text, Image,
            Table, etc..
        """
        raise NotImplementedError(
            "SuiteHandler subclass must implement render() function")

    def title(self):
        """The default title of the figure rendered in plot()."""
        return self.suite.description()

    def device(self):
        """The device name for the report associated with this SuiteHandler
        instance."""
        return self.suite.identifier()

    # convenience functions

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

    def plot(self,
             ctx: SummaryContext,
             plotter: Callable[[], None],
             custom_title: Optional[str] = None,
             path: Optional[Path] = None) -> Path:
        """Plot and save a graph as a PNG image. THIS IS NOT MEANT TO BE
        OVERRIDDEN, as the `plotter` callback argument allows for graphs to be
        customized to a high degree. Nevertheless, it may be overridden if the
        default implementation is insufficient.

        Args:
            ctx: context used to determine dpi and default path to save image
            plotter: callback function, used to modify the graph
            custom_title: graph's title. If None, uses self.title(). If empty
                string, hides the title altogether.
            path: save figure PNG image to this file
        Returns:
            The path to where the image was saved
        """
        if not path:
            prefix = self.suite.file.stem
            path = make_image_path(ctx.folder, prefix, ctx.indexer)
        # Allows for custom_title to be an empty string.
        title = self.title() if custom_title is None else custom_title
        make_plot(path, title, ctx.dpi, plotter)
        return path


# ------------------------------------------------------------------------------


class SuiteSummarizer:
    """A SuiteSummarizer is a group of SuiteHandlers, which are generally all of
    the same type. It is responsible for determining which report data to
    consider for its SuiteHandlers, for initializing those SuiteHandlers, and
    for collection (and possibly sumamrizing) results from those SuiteHandlers.
    This is the preferred way to initialize a SuiteHandler.

    This abstraction removes some prior responsibility from SuiteHandlers. It
    provides a high level of flexibility and customization for each author to
    determine the best way to present and format their results while providing
    good defaults with minimal boilerplate.

    Subclasses must implement the following methods (typically one-liners):
        - default_handler
    """
    def __init__(self, reports: List[Report]):
        """Initializes a SuiteHandler (default_handler) for each Suite in
        reports.

        Subclasses are not generally expected to override this method.

        By default, creates a single SuiteHandler for each device report, but
        this can be overridden to separate an individual device report into
        different Suites (and therefore SuiteHandlers) by suite_id or any other
        criteria if the need calls for it. (This scenario might occur when
        running the same test multiple times with different configurations, in
        which case the data will be concatenated together into a single report
        file for each device.)

        Args:
            reports: _should be_ any report that passes the can_handle_report
                check. (It is the caller's responsibility to ensure this.)
        """
        # Filter unwanted data from reports
        self.reports = reports
        for report in self.reports:
            report.data = list(filter(self.datum_filter, report.data))

        # Make individual suites from reports
        self.suites = []
        for report in self.reports:
            self.suites.extend(report.derive_suites())

        # SuiteHandler instances
        default_handler = self.default_handler()
        self.suite_handlers = [default_handler(s) for s in self.suites]

    # Required methods

    @abstractclassmethod
    def default_handler(cls) -> SuiteHandler:
        """Gives the associated SuiteHandler, instances of which are initialized
        with the data passed in __init__.
        Note: If more than one SuiteHandler is needed, override __init__."""
        raise NotImplementedError(
            "SuiteSummarizer subclass must implement default_handler() function")

    # Overridable methods

    def render_synthesis(self, ctx: SummaryContext) -> List[lib.items.Item]:
        """Allows for providing a synthesis (or "summary") of the results across
        all devices (or SuiteHandlers). A synthesis can include graphs, tables,
        text, etc..

        For example, the associated SuiteHandler might set a `self.success`
        property on itself, which can then be accessed in this
        `render_synthesis` method by iterating over `self.suite_handlers` to
        determine the total number of successes.

        Returns:
            A list of items consumable by a Formatter.
        """
        return []

    # Convenience methods

    @classmethod
    def datum_filter(cls, datum: Datum):
        """A filter function that determines which data to keep in a report.

        This method _can_ be overridden, but it is usually preferable to work
        directly with the `can_handle_datum` method, which is a required method
        for all subclasses."""
        return cls.can_handle_datum(datum)

    @classmethod
    def can_handle_datum(cls, datum: Datum):
        """Determines which datum to include when Suites are passed to the
        __init__ method (thereby also determining which reports are considered).
        The default is to ask the default SuiteHandler which data it would like.

        Note: This can be overridden to collect data across multiple suite_ids
        from the same report.
        """
        return cls.default_handler().can_handle_datum(datum)

    @classmethod
    def can_handle_report(cls, report: Suite):
        """Determines if this SuiteSummarizer can handle a given report. This is
        merely a convenience method making it easier for other classes to
        determine what data this class can handle.

        This method _can_ be overridden, but it is usually preferable to work
        directly with the `can_handle_datum` method, which is a required method
        for all subclasses."""
        for datum in report.data:
            if cls.can_handle_datum(datum):
                return True
        return False

    def render_summary(self, ctx: SummaryContext) -> List[lib.items.Item]:
        """This method executes the core functionality of this class, collating
        the results from all associated SuiteHandlers and providing any custom
        synthesis or cross-device summary.

        This method _can_ be overridden, but it is usually preferable to work
        directly with the `render_synthesis` method. (One might wish to override
        this method if the default reporting order or formatting is
        insufficient.)

        Returns:
            A list of items consumable by a Formatter.
        """
        reports = []
        # Add each SuiteHandler rendering to the summary.
        for handler in self.suite_handlers:
            items = handler.render(ctx)
            if items:
                reports.append(lib.items.Heading(handler.device(), 3))
                reports.extend(items)
                reports.append(lib.items.Separator())

        # If there's a cross-device summary, add it before per-device reports.
        # (This is called _after_ the SuiteHandlers so that the Summarizer's
        # `render_synthesis` method may access computed results from any/all
        # of the SuiteHandlers in self.suite_handlers.)
        synthesis = self.render_synthesis(ctx)
        if synthesis:
            synthesis.append(lib.items.Separator())
            return synthesis + reports

        return reports

    def title(self) -> str:
        """The default title of the figure rendered in plot()."""
        if self.suite_handlers:
            return self.suite_handlers[0].suite.name
        return ""

    def plot(self,
             ctx: SummaryContext,
             plotter: Callable[[], None],
             custom_title: Optional[str] = None,
             path: Optional[Path] = None) -> Path:
        """Plot and save a graph as a PNG image. THIS IS NOT MEANT TO BE
        OVERRIDDEN, as the `plotter` callback argument allows for graphs to be
        customized to a high degree. Nevertheless, it may be overridden if the
        default implementation is insufficient.

        Args:
            ctx: context used to determine dpi and default path to save image
            plotter: callback function, used to modify the graph
            custom_title: graph's title. If None, uses self.title(). If empty
                string, hides the title altogether.
            path: save figure PNG image to this file
        Returns:
            The path to where the image was saved
        """
        if not path:
            # TODO(baxtermichael@google.com): consider using title as URI prefix
            path = make_image_path(ctx.folder, 'summary', ctx.indexer)
        # Allows for custom_title to be an empty string.
        title = self.title() if custom_title is None else custom_title
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
