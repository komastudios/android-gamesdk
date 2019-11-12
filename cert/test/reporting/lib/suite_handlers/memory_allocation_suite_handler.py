import matplotlib
import matplotlib.pyplot as plt
import numpy as np

from typing import List

from lib.chart_components import *


def plot_event_ignore(renderer, fig, index, count, start_time_seconds,
                      end_time_seconds):
    pass


def plot_ignore(renderer):
    pass


def plot_event_on_trim_level(renderer, fig, index, count, start_time_seconds,
                             end_time_seconds):
    for i, d in enumerate(renderer.data):
        s = (d.timestamp / NS_PER_S) - start_time_seconds
        y = fig.get_ylim()[1]
        label = f"on_trim: level {d.value}"
        color = create_color(i, len(renderer.data))
        line = fig.axvline(s, color=color, label=label)
        # label_line(line, label, s, y, size=8)


def plot_event_is_free(renderer, fig, index, count, start_time_seconds,
                       end_time_seconds):
    mid = (fig.get_ylim()[1] + fig.get_ylim()[0]) / 2
    for d in renderer.data:
        s = (d.timestamp / NS_PER_S) - start_time_seconds
        color = [0.2, 1, 0.2]
        fig.scatter(s, mid, color=color, s=10)


def plot_event_is_malloc_fail(renderer, fig, index, count, start_time_seconds,
                              end_time_seconds):
    pass


def plot_event_default(renderer, fig, index, count, start_time_seconds,
                       end_time_seconds):
    pass


def plot_default(renderer):
    plt.plot(renderer.get_x_axis_as_seconds(),
             renderer.get_ys(),
             label=renderer.chart.field)


def plot_oom(renderer):
    plt.ylim(0, 1000)
    plt.plot(renderer.get_x_axis_as_seconds(),
             renderer.get_ys(),
             label=renderer.chart.field)


def plot_memory(renderer):
    plt.gca().yaxis.set_major_formatter(
        matplotlib.ticker.FormatStrFormatter('%d mb'))
    plt.plot(renderer.get_x_axis_as_seconds(),
             renderer.get_ys() / (1024 * 1024),
             label=renderer.chart.field)


def plot_boolean(renderer):
    plt.ylim(0, 1)
    plt.plot(renderer.get_x_axis_as_seconds(),
             renderer.get_ys(),
             label=renderer.chart.field)


def plot_time_ns(renderer):
    plt.gca().yaxis.set_major_formatter(
        matplotlib.ticker.FormatStrFormatter('%.3f ms'))
    plt.plot(renderer.get_x_axis_as_seconds(),
             renderer.get_ys() / NS_PER_MS,
             label=renderer.chart.field)


def create_color(index, count):
    h = index / count
    s = 0.7
    v = 0.7
    return matplotlib.colors.hsv_to_rgb([h, s, v])


class MemoryChartRenderer(ChartRenderer):

    event_plotters = {
        "on_trim_level": plot_event_on_trim_level,
        "is_free": plot_event_is_free,
        "is_malloc_fail": plot_event_is_malloc_fail
    }
    plotters = {
        "memory_state.available_memory": plot_memory,
        "memory_state.native_allocated": plot_memory,
        "sys_mem_info.available_memory": plot_memory,
        "sys_mem_info.native_allocated": plot_memory,
        "total_allocation_bytes": plot_memory,
        "memory_state.oom_score": plot_oom,
        "sys_mem_info.low_memory": plot_ignore,
        "memory_state.low_memory": plot_ignore,
        "perf.max_frame_time_ns": plot_time_ns,
        "perf.min_frame_time_ns": plot_time_ns,
    }

    def __init__(self, chart: Chart):
        super().__init__(chart)

    @classmethod
    def can_render_chart(cls, chart: Chart):
        return chart.operation_id == "MemoryAllocOperation" and (
            chart.field in cls.event_plotters or chart.field in cls.plotters)

    def is_event_chart(self):
        return self.chart.field in [
            "on_trim_level", "is_free", "is_malloc_fail"
        ]

    def plot(self, fig, index, count, start_time_seconds, end_time_seconds):
        if self.is_event_chart():
            self.event_plotters.get(self.chart.field,
                                    plot_event_default)(self, fig, index, count,
                                                        start_time_seconds,
                                                        end_time_seconds)
        else:
            plt.gca().xaxis.set_major_formatter(
                matplotlib.ticker.FormatStrFormatter('%d s'))
            self.plotters.get(self.chart.field, plot_ignore)(self)


class MemoryAllocationSuiteHandler(SuiteHandler):

    def __init__(self, suite):
        super().__init__(suite)

    @classmethod
    def can_handle_suite(cls, suite: Suite):
        return "Memory allocation" in suite.suite_name

    def assign_renderer(self, chart: Chart):
        chart.set_renderer(MemoryChartRenderer(chart))
        return True

    def analyze(self, cb: Callable[[Any, Datum, str], None]):
        """TODO(shamyl@google.com): No analysis as yet on memory allocation data"""
        return None
