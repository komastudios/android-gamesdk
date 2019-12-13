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
import re as regex

from pathlib import Path
from typing import Any, Dict, List, Tuple

if platform.system == "Linux":
    matplotlib.use('gtk3cairo')

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

NS_PER_S = 1e9
MS_PER_S = 1e3
NS_PER_MS = 1e6
NANOSEC_EXPRESSION = "^(\d+) (nanoseconds|ns|nsec)$"
MILLISEC_EXPRESSION = "^(\d+) (milliseconds|ms|msec)$"
SEC_EXPRESSION = "^(\d+) (seconds|s|sec)$"
TIME_EXPRESSIONS = [NANOSEC_EXPRESSION, MILLISEC_EXPRESSION, SEC_EXPRESSION]


def to_float(v: Any) -> float:
    try:
        return float(v)
    except ValueError:
        if v == "True":
            return 1
        elif v == "False":
            return 0
        else:
            for e in TIME_EXPRESSIONS:
                match = regex.search(e, v)
                if match is not None and \
                    len(match.groups()) > 1:
                    return float(match.group(1))

    return None


def ensure_dir(file: Path):
    file_dir = file.parent
    if not os.path.exists(file_dir):
        os.makedirs(file_dir)


def flatten_dict(d: dict) -> List[Tuple[str, Any]]:
    """Recursively flatten a dict to a list of (pathname,value) tuples
    Args:
        d: the dict to flatten
    Returns:
        List of tuples where the 0th element is the full path name of the leaf
            value, and the 1th element is the value
    Note:
        doesn't attempt to handle cycles so be careful
    """
    result = []
    for k in d:
        v = d[k]
        if type(v) is dict:
            for r in flatten_dict(v):
                p = k + "." + r[0]
                result.append((p, r[1]))
        else:
            result.append((k, v))

    return result


class Datum(object):

    def __init__(self, suite_id: str, operation_id: str, thread_id: str,
                 cpu_id: int, timestamp: int, custom: dict):
        self.suite_id = suite_id
        self.operation_id = operation_id
        self.thread_id = thread_id
        self.cpu_id = cpu_id
        self.timestamp = timestamp

        # the raw custom JSON dict from the report JSON entry
        self.custom = custom

        # the custom dict flattened for easier access
        self.custom_fields_flattened = {}
        for field_name, field_value in flatten_dict(custom):
            self.custom_fields_flattened[field_name] = field_value

    @classmethod
    def from_json(cls, data):
        return cls(**data)

    def get_custom_field(self, name) -> Any:
        if name in self.custom_fields_flattened:
            return self.custom_fields_flattened[name]
        return None

    def get_custom_field_numeric(self, name) -> float:
        if name in self.custom_fields_flattened:
            return to_float(self.custom_fields_flattened[name])
        return None


class BuildInfo(object):

    def __init__(self, d: Dict[str, str]):
        self._d = d

    def get(self, feature: str) -> str:
        return self._d[feature]

    def __getitem__(self, key):
        return self._d[key]

    @classmethod
    def from_json(cls, data):
        return cls(data)


class Suite(object):

    def __init__(self, name: str, build: BuildInfo, data: List[Datum],
                 file: Path):
        self.name = name
        self.build = build
        self.data = data
        self.file = file
        self.handler = None

    def identifier(self):
        return self.build["MANUFACTURER"] + " " + self.build[
            "DEVICE"] + " SDK " + str(self.build["SDK_INT"])

    def description(self):
        return self.name + " (" + self.build["MANUFACTURER"] + " " + self.build[
            "DEVICE"] + " SDK " + str(self.build["SDK_INT"]) + ")"


class SuiteHandler(object):

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
