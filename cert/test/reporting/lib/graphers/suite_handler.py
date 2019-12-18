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
from typing import List

import matplotlib.pyplot as plt
import numpy as np

from lib.common import ensure_dir, nanoseconds_to_seconds
from lib.report import Suite


class SuiteHandler(ABC):
    """SuiteHandler is handles data for a report suite
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

    # TODO: Refactor into single/multiple versions
    @abstractclassmethod
    def can_handle_suite(cls, suite: Suite):
        """Check if this SuiteHandler class can be used to handle this
        suite's data
        Args:
            suite: The suite in question
        Returns:
            True if this SuiteHandler class should handle
                the contents of the provided Suite instance
        """
        raise NotImplementedError(
            "SuiteHandler subclass must implement can_handle_suite() function")

    def plot(self, plot_file_name: Path, dpi: int):
        """Plot this suite's graph
        Args:
            plot_file_name: save figure PNG image to this file
            dpi: figures are saved at this DPI
        """
        ensure_dir(plot_file_name)
        plt.ioff()
        plt.suptitle(self.title())
        summary_str = self.render_plot()
        plt.savefig(str(plot_file_name), dpi=dpi)
        plt.close()
        return summary_str

    @abstractmethod
    def render_plot(self) -> str:
        """Subclasses implement this method to render their data to matplotlib
        Note: Don't call plt.suptitle() or title() - that's already been done.
        You're just rendering data to 1 or more figures.
        Return:
            (optional) a summary string for a given dataset
            If a report has some interesting data (outlier behavior,
            failures, etc) generate a summary string here and return it.
            Otherwise, returning None or an empty string will result in
            nothing printed.
        """
        raise NotImplementedError(
            "SuiteHandler subclass must implement render_plot() function")


    @abstractclassmethod
    def handles_entire_report(cls):
        """Check if this suite handler wants to parse all suites as a single graph.
        """
        return False

    #@abstractclassmethod
    @classmethod
    def can_render_single(cls):
        """Check if this suite handler wants to draw its own plot.
        """
        return True

    @classmethod
    def render_entire_report(cls, suites: List['SuiteHandler'],
                             plot_file_name: Path, dpi: int) -> str:
        ensure_dir(plot_file_name)
        plt.ioff()
        # TODO(tmillican@google.com): Let report author give a name/description
        plt.suptitle(suites[0].identifier())
        summary_str = cls.render_report(suites)
        plt.savefig(str(plot_file_name), dpi=dpi)
        plt.close()
        return summary_str

    @abstractclassmethod
    def render_report(cls, raw_suites: List['SuiteHandler']) -> str:
        """Subclass implement this method to render multiple suites' data to matplotlib
        Note: Don't call suptitle() that's already been done.
        Return:
            (optional) a summary string for a given dataset
            If a report has some interesting data (outlier behavior,
            failures, etc) generate a summary string here and return it.
            Otherwise, returning None or an empty string will result in
            nothing printed.
        """
        raise NotImplementedError(
            "SuiteHandler subclass must implement render_report() function")


    def title(self):
        """The title of the figure rendered in render_plot()

        Override this method to return a custom title for your plot.
        """
        return self.suite.description()

    @abstractclassmethod
    def can_render_summarization_plot(cls,
                                      suites: List['SuiteHandler']) -> bool:
        """Can this SuiteHandler class render summarization plots for a
        homogenous collection of Suites

        Implementations could, for example, render a histogram of failures
        by core count, or show which CPU architectures result in some
        outlier result.

        If a SuiteHandler implementation returns True here, it *must*
        implement render_summarization_plot()

        Args:
            suites: Multiple suites of data, each of which have SuiteHandlers
            of the same class (meaning, they represent different result data)
            from the same test.
        Return:
            True to indicate that a summarization plot can be rendered
        """
        raise NotImplementedError(
            "SuiteHandler subclass must implement "\
                "can_render_summarization_plot() function"
        )

    @abstractclassmethod
    def render_summarization_plot(cls, suites: List['SuiteHandler']) -> str:
        """Render a plot representing multiple datasets

        Subclass of SuiteHandler that return True for
        can_render_summarization_plot() must implement this method.

        Implementations could, for example, render a histogram of failures
        by core count, or show which CPU architectures result in some
        outlier result.

        Args:
            suites: Multiple suites of data, each of which have SuiteHandlers
            of the same class (meaning, they represent different result data)
            from the same test.
        Return:
            summary string for display in the document, or None if no
            summary is required
        """
        raise NotImplementedError(
            "SuiteHandler subclass must implement "\
                "render_summarization_plot() function"
        )

    @classmethod
    def summarization_plot(cls, suites: List['SuiteHandler'],
                           plot_file_name: Path, dpi: int) -> str:
        """Generates a summarization plot from the provided suites,
        calling this class's render_summarization_plot implementation"""
        ensure_dir(plot_file_name)
        plt.ioff()
        summary_str = cls.render_summarization_plot(suites)
        plt.savefig(str(plot_file_name), dpi=dpi)
        plt.close()
        return summary_str

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
