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

import matplotlib
import matplotlib.pyplot as plt

from ..chart_components import MS_PER_S, NS_PER_MS


BYTES_PER_MEGABYTE = 1024 * 1024


def plot_default(renderer, scale_factor=None, y_label_format=None):
    """
    In its simplest form, this function plots the renderer data as is.
    If a scale factor is passed, the y values are adjusted by it.
    If a format is passed, the y labels are formatted based on it.
    """
    if y_label_format is not None:
        plt.gca().yaxis.set_major_formatter(
            matplotlib.ticker.FormatStrFormatter(y_label_format)
        )

    plt.plot(renderer.get_x_axis_as_seconds(),
             renderer.get_ys() if scale_factor is None \
                else renderer.get_ys() / scale_factor,
             label=renderer.chart.field)


def plot_boolean(renderer):
    """
    Plots boolean values as "Yes" / "No".
    """
    plt.yticks([0, 1], ["No", "Yes"])
    plot_default(renderer)


def plot_fps(renderer):
    """
    Plots frames per seconds.
    """
    plot_default(renderer, y_label_format="%.3f fps")


def plot_thermal(renderer):
    """
    Plots the thermal status taxonomoy.
    """
    plt.yticks(range(-1, 6), [
        "Unknown", "None", "Light", "Moderate", "Severe", "Critical",
        "Emergency", "Shutdown"
    ])
    plot_default(renderer)


def plot_memory_as_mb(renderer):
    """
    Given a renderer loaded with memory data in bytes, plots the data as
    megabytes.
    """
    plot_default(renderer, BYTES_PER_MEGABYTE, "%d mb")


def plot_time_ns_as_ms(renderer):
    """
    Plots renderer data (expressed in nanoseconds) converted to milliseconds.
    """
    plot_default(renderer, NS_PER_MS, "%d ms")


def plot_time_ms_as_sec(renderer):
    """
    Plots renderer data (expressed in milliseconds) converted to seconds.
    """
    plot_default(renderer, MS_PER_S, "%.3f sec")


def plot_time_as_ms(renderer):
    """
    Plots renderer data expressed in milliseconds.
    """
    plot_default(renderer, y_label_format="%.3f ms")


def plot_oom(renderer):
    """
    Plots the out-of-memory score as a factor of 0 to 1000.
    """
    plt.ylim(0, 1000)
    plot_default(renderer)


def plot_ignore(renderer):
    """
    Wildcard NO-OP plotting function.
    """
    pass


def add_plotters_to_default(plotters):
    """
    Receives a list of plotters, returns an enhanced plotter list that
    includes the ones for MonitorOperation.
    """
    all_plotters = {
        "memory_state.available_memory": plot_memory_as_mb,
        "memory_state.native_allocated": plot_memory_as_mb,
        "memory_state.oom_score": plot_oom,
        "memory_state.low_memory": plot_boolean,
        "perf_info.fps": plot_fps,
        "perf_info.max_frame_time": plot_time_ns_as_ms,
        "perf_info.min_frame_time": plot_time_ns_as_ms,
        "thermal_info.status": plot_thermal,
    }
    all_plotters.update(plotters)

    return all_plotters
