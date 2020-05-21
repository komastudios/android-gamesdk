#
# Copyright 2020 The Android Open Source Project
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
"""P3.2 Texture Loading Strategies."""

from typing import Dict, List, Set

import matplotlib.pyplot as plt
from matplotlib.ticker import FormatStrFormatter
import mpl_toolkits.axisartist.axislines as axislines
import numpy as np

from lib.graphers.suite_handler import SuiteHandler, SuiteSummarizer
from lib.report import Datum, SummaryContext, Suite
import lib.items as items


class TextureFormatsSuiteSummarizer(SuiteSummarizer):
    """Suite summarizer for texture format comparison."""
    @classmethod
    def default_handler(cls) -> SuiteHandler:
        return TextureFormatSuiteHandler


class ChannelCompositionStats:
    """Texture format stats for a particular channel composition."""

    def __init__(self, channel_composition: str):
        self.channel_composition = channel_composition
        self.__disk_size_per_format: Dict[str, int] = {}
        self.__mem_size_per_format: Dict[str, int] = {}
        self.__decode_time_per_format: Dict[str, int] = {}
        self.__gpu_trx_time_per_format: Dict[str, int] = {}
        self.missing_formats: Set[str] = set()

    def apply_data_points(self, event_type: str, datum: Datum):
        """Applies data points to stats."""
        texture_format = datum.get_custom_field("texture.format")
        if event_type == "TEXTURE_LOAD_END":
            self.__apply_load_data_points(texture_format, datum)
        elif event_type == "TEXTURE_LOAD_FAILURE":
            self.__apply_format_failure(texture_format, datum)
        elif event_type == "TEXTURE_GPU_TRANSFER":
            self.__apply_gpu_transfer(texture_format, datum)

    def __apply_load_data_points(self, texture_format: str, datum: Datum):
        """Incorporates size and load time info to stats."""
        if texture_format not in self.missing_formats:
            accumulate(datum.get_custom_field("size_at_rest"),
                       texture_format, self.__disk_size_per_format, 1.0/2**20)
            accumulate(datum.get_custom_field("size_in_memory"),
                       texture_format, self.__mem_size_per_format, 1.0/2**20)
            accumulate(datum.get_custom_field("loading_time"),
                       texture_format, self.__decode_time_per_format,
                       1.0/10**3)

    def __apply_gpu_transfer(self, texture_format: str, datum: Datum):
        """Incorporates CPU to GPU transfer info to stats."""
        if texture_format not in self.missing_formats:
            accumulate(datum.get_custom_field("gpu_trx_time"),
                       texture_format, self.__gpu_trx_time_per_format,
                       1.0/10**3)

    def __apply_format_failure(self, texture_format: str, datum: Datum):
        """Incorporates info about missing, incomplete texture formats. These
        formats won't be included in the charts."""
        if texture_format not in self.missing_formats:
            self.missing_formats.add(texture_format)
            delete_entry(texture_format, self.__disk_size_per_format)
            delete_entry(texture_format, self.__mem_size_per_format)
            delete_entry(texture_format, self.__decode_time_per_format)
            delete_entry(texture_format, self.__gpu_trx_time_per_format)

    def graph_usage(self, axes):
        """Arranges the usage graph on disk and in memory."""
        formats = self.__disk_size_per_format.keys()
        disk_sizes = self.__disk_size_per_format.values()
        mem_sizes = self.__mem_size_per_format.values()

        y = np.arange(len(formats))
        height = 0.4
        x_limit = max(mem_sizes) * 1.1

        axes.set_title("Total size (in Mb)")
        axes.barh(y - height/2, disk_sizes, height,
                  label="At rest", color="deepskyblue")
        axes.barh(y + height/2, mem_sizes, height,
                  label="In memory", color="sandybrown")
        axes.legend()

        axes.set_yticks(y)
        axes.set_yticklabels(formats)
        axes.invert_yaxis()
        axes.axis["left"].set_axis_direction("right")

        axes.set_xlim([0, x_limit])
        axes.get_xaxis().set_major_formatter(FormatStrFormatter('%d'))

    def graph_elapsed_times(self, axes):
        """Arranges the graph of all times involved on image decode and
        rendering."""
        formats = list(self.__decode_time_per_format.keys())

        total_times = {}
        for texture_format in formats:
            total_times[texture_format] = [
                self.__decode_time_per_format[texture_format],
                self.__gpu_trx_time_per_format[texture_format]
            ]
        data = np.array(list(total_times.values()))
        data_cum = data.cumsum(axis=1)
        x_limit = max(data_cum[:, 1]) * 1.1

        axes.set_title("Total times (in seconds)")
        time_names = ["Storage to CPU", "CPU to GPU"]
        time_colors = ["deepskyblue", "sandybrown"]

        for i, (colname, color) in enumerate(zip(time_names, time_colors)):
            widths = data[:, i]
            starts = data_cum[:, i] - widths
            axes.barh(formats, widths, left=starts, height=0.8,
                      label=colname, color=color)

        axes.legend(loc='best')

        axes.invert_yaxis()
        axes.axis["left"].set_axis_direction("right")

        axes.set_xlim([0, x_limit])
        axes.get_xaxis().set_major_formatter(FormatStrFormatter('%.1f'))

    def produce_comparative_table(self) -> items.Item:
        """Given some channel composition stats, creates a subsection with a
        table with percentual comparison."""
        formats = list(self.__disk_size_per_format.keys())
        uncompressed_format = formats[0]

        header = [f"Format vs. {uncompressed_format}", "Size at Rest",
                  "Size in Memory", "Storage to CPU", "CPU to GPU"]
        rows = []

        def percentual_ratio(values, compressed, uncompressed):
            return "{:.1%}".format(1.0 *
                                   values[compressed] / values[uncompressed])

        for compressed_format in formats[1:]:
            row = [compressed_format]
            row.append(percentual_ratio(
                self.__disk_size_per_format,
                compressed_format, uncompressed_format))
            row.append(percentual_ratio(
                self.__mem_size_per_format,
                compressed_format, uncompressed_format))
            row.append(percentual_ratio(
                self.__decode_time_per_format,
                compressed_format, uncompressed_format))
            row.append(percentual_ratio(
                self.__gpu_trx_time_per_format,
                compressed_format, uncompressed_format))
            rows.append(row)

        return items.Table(header, rows)


def accumulate(value: int, texture_format: str,
               accumulators: Dict[str, int], scalar: float = 1.0):
    """General accumulator function to be used to accumulate all data
    points."""
    accumulator = accumulators.get(texture_format, 0)
    accumulators[texture_format] = accumulator + value * scalar


def delete_entry(key, dictionary):
    """Given a dictionary, removes an entry if it exists."""
    if key in dictionary:
        del dictionary[key]


class TextureFormatSuiteHandler(SuiteHandler):
    """Grapher for texture format comparison."""

    def __init__(self, suite: Suite):
        super().__init__(suite)

        self.__stats_per_composition: \
            Dict[str, ChannelCompositionStats] = {}

        for datum in self.data:
            if datum.operation_id == "TextureLoadingGLES3Operation":
                self.__pump_datum_to_stats(datum)

    @classmethod
    def can_handle_datum(cls, datum: Datum):
        return "Texture Loading" in datum.suite_id

    def __pump_datum_to_stats(self, datum: Datum):
        """Pushes the datum entry to the overall stats."""
        event_type = datum.get_custom_field("event_type")
        if event_type == "TEXTURE_LOAD_END" \
                or event_type == "TEXTURE_LOAD_FAILURE" \
                or event_type == "TEXTURE_GPU_TRANSFER":
            channels = datum.get_custom_field("texture.channels")
            composition_stats = self.__get_composition_stats(channels)
            composition_stats.apply_data_points(event_type, datum)

    def __get_composition_stats(self, channels: str) \
            -> ChannelCompositionStats:
        """Retrieves stats for a channel composition. If it doesn't exist yet,
        it creates these."""
        composition_stats = self.__stats_per_composition.get(channels)
        if composition_stats is None:
            composition_stats = ChannelCompositionStats(channels)
            self.__stats_per_composition[channels] = composition_stats

        return composition_stats

    def render(self, ctx: SummaryContext) -> List[items.Item]:
        report_items = []

        report_items.append(self.__produce_graph(ctx))

        for channel in self.__stats_per_composition.values():
            report_items.append(channel.produce_comparative_table())

        return report_items

    def __produce_graph(self, ctx: SummaryContext) -> items.Image:
        """Creates charts that show the different texture format performances
        in a compared way."""
        image_path = self.plot(ctx, self.__graph_stats)
        return items.Image(image_path, self.device())

    def __graph_stats(self):
        """Arranges graphs in columns per channel component."""
        fig = plt.figure(figsize=(4, 2.5))
        fig.subplots_adjust(bottom=-0.3, top=0.75, hspace=0.6)

        total_columns = len(self.__stats_per_composition)
        row = 0

        def create_axes(column):
            axes = axislines.Subplot(
                fig, 3, total_columns, 1 + row * total_columns + column)
            fig.add_subplot(axes)
            return axes

        for composition_stats in self.__stats_per_composition.values():
            composition_stats.graph_usage(create_axes(0))
            composition_stats.graph_elapsed_times(create_axes(1))

            row += 1
