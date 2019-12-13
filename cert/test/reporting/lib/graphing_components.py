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

import os
import platform
import matplotlib
import matplotlib.pyplot as plt
import numpy as np

from pathlib import Path
from typing import Any, Dict, List, Tuple

from lib.common import ensure_dir, NS_PER_S
from lib.report import Datum, BuildInfo, Suite

if platform.system == "Linux":
    matplotlib.use('gtk3cairo')

# ------------------------------------------------------------------------------

SMALL_SIZE = 5
MEDIUM_SIZE = 7
BIGGER_SIZE = 10

plt.rc('font', size=SMALL_SIZE)  # controls default text sizes
plt.rc('axes', titlesize=SMALL_SIZE)  # fontsize of the axes title
plt.rc('axes', labelsize=MEDIUM_SIZE)  # fontsize of the x and y labels
plt.rc('xtick', labelsize=SMALL_SIZE)  # fontsize of the tick labels
plt.rc('ytick', labelsize=SMALL_SIZE)  # fontsize of the tick labels
plt.rc('legend', fontsize=SMALL_SIZE)  # legend fontsize
plt.rc('figure', titlesize=BIGGER_SIZE)  # fontsize of the figure title

# ------------------------------------------------------------------------------


class SuiteHandler(object):
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

    @classmethod
    def can_handle_suite(cls, suite: Suite):
        """Check if this SuiteHandler class can be used to handle this
        suite's data
        Args:
            suite: The suite in question
        Returns:
            True if this SuiteHandler class should handle
                the contents of the provided Suite instance
        """
        return False

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

    def render_plot(self) -> str:
        """Subclasses implement this method to render their data to matplotlib
        Note: Don't call plt.suptitle() or title() - that's already been done.
        You're just rendering data to 1 or more figures.
        Return:
            (optional) a summary string for a given dataset
            If a report has some interesting data (outlier behavior, failures, etc)
            generate a summary string here and return it. Otherwise, returning
            None or an empty string will result in nothing printed.
        """
        plt.plot([1, 2, 3, 4])
        plt.ylabel('some numbers')
        return "An interesting datapoint, or None"

    def title(self):
        """The title of the figure rendered in render_plot()"""
        return self.suite.description()

    @classmethod
    def can_render_summarization_plot(cls,
                                      suites: List['SuiteHandler']) -> bool:
        """Subclasses should return True to indicate that they can
        render a summarization plot for a collection of suites of same
        SuiteHandler class
        Args:
            suites: Multiple suites of data, each of which have SuiteHandler's
            of the same class (meaning, they represent different result data)
            from the same test.
        Return:
            True to indicate that a summarization plot can be rendered
        """
        return False

    @classmethod
    def render_summarization_plot(cls, suites: List['SuiteHandler']) -> str:
        """Subclasses render a summarization plot of the
        data represented by multiple suites
        Args:
            suites: Multiple suites of data, each of which have SuiteHandler's
            of the same class (meaning, they represent different result data)
            from the same test.
        Return:
            summary string for display in the document
        """
        return None

    @classmethod
    def summarization_plot(cls, suites: List['SuiteHandler'],
                           plot_file_name: Path, dpi: int) -> str:

        ensure_dir(plot_file_name)
        plt.ioff()
        summary_str = cls.render_summarization_plot(suites)
        plt.savefig(str(plot_file_name), dpi=dpi)
        plt.close()
        return summary_str

    # convenience functions

    def get_xs(self) -> List[int]:
        return np.array(list(map(lambda d: d.timestamp, self.data)))

    def get_x_axis_as_seconds(self):
        xs = self.get_xs()
        return (xs - xs[0]) / NS_PER_S

    def get_ys(self) -> List[float]:
        return np.array(list(map(lambda d: d.numeric_value, self.data)))
