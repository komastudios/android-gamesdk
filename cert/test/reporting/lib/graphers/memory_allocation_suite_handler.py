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

from lib.graphing_components import *


def plot_event_on_trim_level(fig, xs, ys, label):
    for x, y in zip(xs, ys):
        label = f"on_trim: level {int(y)}"
        color = [1, 0, 0]
        line = fig.axvline(x, color=color, label=label)


def plot_event_is_free(fig, xs, ys, label):
    mid = (fig.get_ylim()[1] + fig.get_ylim()[0]) / 2
    for x in xs:
        color = [0.2, 1, 0.2]
        fig.scatter(x, mid, color=color, s=10)


def plot_event_is_malloc_fail(fig, xs, ys, label):
    pass


def plot_default(fig, xs, ys, label, scale_factor=None, y_label_format=None):
    """
    In its simplest form, this function plots the renderer data as is.
    If a scale factor is passed, the y values are adjusted by it.
    If a format is passed, the y labels are formatted based on it.
    """
    if y_label_format is not None:
        plt.gca().yaxis.set_major_formatter(
            matplotlib.ticker.FormatStrFormatter(y_label_format))

    plt.plot(xs,
             ys if scale_factor is None \
                else ys / scale_factor,
             label=label)


def plot_boolean(fig, xs, ys, label):
    """
    Plots boolean values as "Yes" / "No".
    """
    plt.yticks([0, 1], ["No", "Yes"])
    plot_default(fig, xs, ys, label)


BYTES_PER_MEGABYTE = 1024 * 1024


def plot_memory_as_mb(fig, xs, ys, label):
    """
    Given a renderer loaded with memory data in bytes, plots the data as
    megabytes.
    """
    plot_default(fig, xs, ys, label, BYTES_PER_MEGABYTE, "%d mb")


class MemoryAllocationSuiteHandler(SuiteHandler):

    def __init__(self, suite):
        super().__init__(suite)

        # we're plotting just these 6 fields
        self.plotters_by_field_name = {
            "on_trim_level": plot_event_on_trim_level,
            "is_free": plot_event_is_free,
            #"is_malloc_fail": plot_event_is_malloc_fail,
            "sys_mem_info.available_memory": plot_memory_as_mb,
            "sys_mem_info.native_allocated": plot_memory_as_mb,
            "sys_mem_info.low_memory": plot_boolean,
            "total_allocation_bytes": plot_memory_as_mb,
        }

    @classmethod
    def can_handle_suite(cls, suite: Suite):
        return "Memory allocation" in suite.name

    def summary(self):
        return None

    def render_plot(self):
        x_axis_seconds = self.get_x_axis_as_seconds()
        start_time_seconds = self.get_xs()[0] / NS_PER_S
        end_time_seconds = self.get_xs()[-1] / NS_PER_S

        happy_color = [0.2, 1, 0.2]
        bad_color = [1, 0.2, 0.2]

        rows = len(self.plotters_by_field_name)
        keys = list(self.plotters_by_field_name.keys())
        keys.sort()

        for i, field_name in enumerate(keys):
            fig = plt.subplot(rows, 1, i + 1)
            plt.ylabel(field_name)
            fig.set_xlim(0, end_time_seconds - start_time_seconds)

            xs = []
            ys = []
            for datum in self.data:
                value = datum.get_custom_field_numeric(field_name)
                if value is not None:
                    s = (datum.timestamp / NS_PER_S) - start_time_seconds
                    xs.append(s)
                    ys.append(value)

            xs = np.array(xs)
            ys = np.array(ys)
            self.plotters_by_field_name[field_name](fig, xs, ys, field_name)
