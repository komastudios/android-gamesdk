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

import sys
import csv
from pathlib import Path
from typing import Any, Dict, List
from lib.csv import CSVEntry
from lib.chart_components import *
from lib.suite_handlers.loader import create_suite_handler

import matplotlib
import matplotlib.pyplot as plt
import numpy as np

matplotlib.use('gtk3cairo')

# -----------------------------------------------------------------------------

#
# Configure plot font sizes
#

SMALL_SIZE = 8
MEDIUM_SIZE = 10
BIGGER_SIZE = 12

plt.rc('font', size=SMALL_SIZE)  # controls default text sizes
plt.rc('axes', titlesize=SMALL_SIZE)  # fontsize of the axes title
plt.rc('axes', labelsize=MEDIUM_SIZE)  # fontsize of the x and y labels
plt.rc('xtick', labelsize=SMALL_SIZE)  # fontsize of the tick labels
plt.rc('ytick', labelsize=SMALL_SIZE)  # fontsize of the tick labels
plt.rc('legend', fontsize=SMALL_SIZE)  # legend fontsize
plt.rc('figure', titlesize=BIGGER_SIZE)  # fontsize of the figure title

# -----------------------------------------------------------------------------


def label_line(line, label, x, y, color='0.5', size=12):
    """Add a label to a line, at the proper angle.

    Args:
        line : matplotlib.lines.Line2D object,
        label : str
        x : float
            x-position to place center of text (in data coordinated
        y : float
            y-position to place center of text (in data coordinates)
        color : str
        size : float
    """
    xdata, ydata = line.get_data()
    x1 = xdata[0]
    x2 = xdata[-1]
    y1 = ydata[0]
    y2 = ydata[-1]

    ax = line.axes
    text = ax.annotate(label,
                       xy=(x, y),
                       xytext=(-10, 0),
                       textcoords='offset points',
                       size=size,
                       color=color,
                       horizontalalignment='left',
                       verticalalignment='bottom')

    sp1 = ax.transData.transform_point((x1, y1))
    sp2 = ax.transData.transform_point((x2, y2))

    rise = (sp2[1] - sp1[1])
    run = (sp2[0] - sp1[0])

    slope_degrees = np.degrees(np.arctan2(rise, run))
    text.set_rotation(slope_degrees)
    return text


# -----------------------------------------------------------------------------


class SuiteImpl(Suite):

    def __init__(self, suite_name: str, entries: List[CSVEntry]):
        super().__init__(suite_name, entries)
        self.charts: List[Chart] = []
        self.handler = create_suite_handler(self)

        if not self.handler:
            print(f"No handler found for suite \"{suite_name}\"")
        else:
            self.datums_by_topic = {}
            for e in entries:
                d = Datum(e, e.timestamp, e.operation_id, e.token, e.value)
                topic = (e.operation_id, e.token)
                if not topic in self.datums_by_topic:
                    self.datums_by_topic[topic] = []
                self.datums_by_topic[topic].append(d)

            self.charts = []
            for topic in self.datums_by_topic:
                c = Chart(suite_name, topic[0], topic[1],
                          self.datums_by_topic[topic])
                if self.handler.assign_renderer(c):
                    self.charts.append(c)

            self.handler.charts = self.charts

    def topics(self):
        """Returns the topics repsented by this suite
        Returns:
            List of tuple of (operation id, field name)
        """
        return list(self.datums_by_topic.keys())

    def analyze(self, cb: Callable[[Any, Datum, str], None]):
        if self.handler:

            def _cb(s, d, m):
                print(f"Suite[{s.suite_name}]::analyze msg: {m}")
                cb(s, d, m)

            return self.handler.analyze(_cb)
        return None

    def plot(self, title: str, fields: List[str] = [], skip: List[str] = []):
        if not self.charts:
            print(f"No charts to render for {title}")
            return

        plt.figure()

        charts_to_render = []
        for c in self.charts:
            include = False

            if fields:
                for field_name in fields:
                    if field_name in c.field or field_name in c.operation_id:
                        include = True
                        break

            if skip:
                for field_name in skip:
                    if field_name in c.field or field_name in c.operation_id:
                        include = False
                        break

            if include or not fields:
                charts_to_render.append(c)

        start_time_seconds = min(
            map(lambda c: min(c.renderer.get_xs()), self.charts)) / NS_PER_S
        end_time_seconds = max(
            map(lambda c: max(c.renderer.get_xs()), self.charts)) / NS_PER_S

        count = len(charts_to_render)
        for i, chart in enumerate(charts_to_render):
            fig = plt.subplot(count, 1, i + 1)
            plt.xlim(0, end_time_seconds - start_time_seconds)
            chart.plot(fig, i, count, start_time_seconds, end_time_seconds)

            fig.legend(labels=None, loc="upper right")

            # hide the x axis for all but the last plot
            # since they're all showing time, and it is noisy
            if i < count - 1:
                plt.gca().axes.get_xaxis().set_visible(False)

        plt.suptitle(title)
        plt.xlabel("time (seconds since start of test)")
        plt.show()


def load_suites(csv_file) -> List[Suite]:
    with open(csv_file, newline='') as csvfile:
        dialect = csv.Sniffer().sniff(csvfile.read(1024))
        csvfile.seek(0)
        reader = csv.reader(csvfile, dialect)

        entries_by_suite: Dict[str, List[CSVEntry]] = {}

        next(reader)  # skip the header line
        for row in reader:
            e = CSVEntry(*row)
            entries_by_suite.setdefault(e.suite_id, []).append(e)

        suites: List[Suite] = []
        for suite_name in entries_by_suite:
            suite = SuiteImpl(suite_name, entries_by_suite[suite_name])
            suites.append(suite)

        return suites
