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

import re
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


class ReportUtils:
    """Constants and useful presentation functions to use throughout this
    report."""
    JPG = "JPG"
    PNG = "PNG"

    RGB = "RGB"
    RGBA = "RGBA"

    DEFLATED = "Deflated"
    LZ4 = "LZ4"

    BYTES_AS_MEGAS = 2**-20
    MILLIS_AS_SECS = 10**-3

    @classmethod
    def is_traditional_format(cls, which_format: str) -> bool:
        """Traditional texture formats that we are comparing against
        alternatives. For now, just JPG and PNG. It could be TIFF, BMP, etc.,
        in the future."""
        return cls.JPG == which_format or cls.PNG == which_format

    @classmethod
    def is_extra_compressed(cls, which_format: str) -> bool:
        """True if the format is gzip'ed or lz4'ed."""
        return cls.DEFLATED == which_format or cls.LZ4 == which_format

    @staticmethod
    def decompose_format(texture_format: str) -> (str, str):
        """Given a texture format, it retrieves its original format and,
        separately, its extra-compression if any.
        So, if input is "ASTC 2BPP", output is "ASTC 2BPP", "".
        If "Deflated (ASTC 2BPP)", then "ASTC 2BPP", "Deflated".
        """
        parts = re.match(r"^(([^\ ]+)\ \((.*)\))|(.+)$", texture_format)
        return (parts[4], "") if parts[4] else (parts[3], parts[2])

    @staticmethod
    def key(channels: str, extra_compression: str = "") -> str:
        """Composes an index key from channels and extra_compression."""
        return f"{channels}-{extra_compression}" if extra_compression \
            else channels


class ChannelCompositionStats:
    """Texture format stats for a particular channel composition and, optional,
    extra compression."""

    def __init__(self, channel_composition: str, extra_compression: str):
        self.channel_composition = channel_composition
        self.extra_compression = extra_compression
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
            accumulate(datum.get_custom_field("size_at_rest"), texture_format,
                       self.__disk_size_per_format,
                       ReportUtils.BYTES_AS_MEGAS)
            accumulate(datum.get_custom_field("size_in_memory"), texture_format,
                       self.__mem_size_per_format,
                       ReportUtils.BYTES_AS_MEGAS)
            accumulate(datum.get_custom_field("loading_time"), texture_format,
                       self.__decode_time_per_format,
                       ReportUtils.MILLIS_AS_SECS)

    def __apply_gpu_transfer(self, texture_format: str, datum: Datum):
        """Incorporates CPU to GPU transfer info to stats."""
        if texture_format not in self.missing_formats:
            accumulate(datum.get_custom_field("gpu_trx_time"), texture_format,
                       self.__gpu_trx_time_per_format,
                       ReportUtils.MILLIS_AS_SECS)

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
        axes.barh(y - height / 2,
                  disk_sizes,
                  height,
                  label="At rest",
                  color="deepskyblue")
        axes.barh(y + height / 2,
                  mem_sizes,
                  height,
                  label="In memory",
                  color="sandybrown")

        axes.legend(loc='best')

        axes.set_yticks(y)
        axes.set_yticklabels(formats)
        axes.invert_yaxis()
        axes.axis["left"].set_axis_direction("right")

        axes.set_xlim([0, x_limit])
        axes.get_xaxis().set_major_formatter(FormatStrFormatter('%d'))

    def graph_elapsed_times(self, axes):
        """Arranges the graph of times involved on image decode and rendering.
        """
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
            axes.barh(formats,
                      widths,
                      left=starts,
                      height=0.8,
                      label=colname,
                      color=color)

        axes.legend(loc='best')

        axes.invert_yaxis()
        axes.axis["left"].set_axis_direction("right")

        axes.set_xlim([0, x_limit])
        axes.get_xaxis().set_major_formatter(FormatStrFormatter('%.1f'))

    def graph_usage_extra(self, axes):
        """Arranges the normal vs. extra compressed usage graph."""
        formats = []
        normal_size_at_rest = []
        extra_compressed_size_at_rest = []

        formats_all = list(self.__disk_size_per_format.keys())
        disk_sizes_all = list(self.__disk_size_per_format.values())
        for index in range(0, len(formats_all), 2):
            formats.append(formats_all[index])
            normal_size_at_rest.append(disk_sizes_all[index])
            extra_compressed_size_at_rest.append(disk_sizes_all[index + 1])

        y = np.arange(len(formats))
        height = 0.4
        x_limit = max(normal_size_at_rest) * 1.1

        axes.set_title("Total size at rest (in Mb)")
        axes.barh(y - height / 2,
                  normal_size_at_rest,
                  height,
                  label="Normal",
                  color="deepskyblue")
        axes.barh(y + height / 2,
                  extra_compressed_size_at_rest,
                  height,
                  label=self.extra_compression,
                  color="sandybrown")

        axes.legend(loc='best')

        axes.set_yticks(y)
        axes.set_yticklabels(formats)
        axes.invert_yaxis()
        axes.axis["left"].set_axis_direction("right")

        axes.set_xlim([0, x_limit])
        axes.get_xaxis().set_major_formatter(FormatStrFormatter('%d'))

    def graph_elapsed_extra(self, axes):
        """Arranges the normal vs. extra compressed loading time graph."""
        formats = []
        normal_time = []
        extra_compressed_time = []

        formats_all = list(self.__decode_time_per_format.keys())
        load_times_all = list([sum(x) for x in zip(*[
            self.__decode_time_per_format.values(),
            self.__gpu_trx_time_per_format.values()
        ])])
        for index in range(0, len(formats_all), 2):
            formats.append(formats_all[index])
            normal_time.append(load_times_all[index])
            extra_compressed_time.append(load_times_all[index + 1])

        y = np.arange(len(formats))
        height = 0.4
        x_limit = max(extra_compressed_time) * 1.1

        axes.set_title("Total time from storage to GPU (in seconds)")
        axes.barh(y - height / 2,
                  normal_time,
                  height,
                  label="Normal",
                  color="deepskyblue")
        axes.barh(y + height / 2,
                  extra_compressed_time,
                  height,
                  label=self.extra_compression,
                  color="sandybrown")

        axes.legend(loc='best')

        axes.set_yticks(y)
        axes.set_yticklabels(formats)
        axes.invert_yaxis()
        axes.axis["left"].set_axis_direction("right")

        axes.set_xlim([0, x_limit])
        axes.get_xaxis().set_major_formatter(FormatStrFormatter('%.1f'))

    def chart_format_comparison(self) -> List[items.Table]:
        """Given a traditional format (JPG, etc.), creates a table with
        comparison against compacted alternatives (size on disk, memory and
        loading times)."""
        formats = list(self.__disk_size_per_format.keys())
        subject_format = formats[0]

        header = [
            f"{subject_format}=100%", "Size at Rest",
            "Size in Memory", "Storage to GPU"
        ]
        rows = []

        for compressed_format in formats[1:]:
            row = [compressed_format]
            row.append(
                percentual_ratio(subject_format, compressed_format,
                                 self.__disk_size_per_format))
            row.append(
                percentual_ratio(subject_format, compressed_format,
                                 self.__mem_size_per_format))
            row.append(
                percentual_ratio(subject_format, compressed_format,
                                 self.__decode_time_per_format,
                                 self.__gpu_trx_time_per_format))
            rows.append(row)

        return [
            items.Text(
                f"{subject_format} VS. COMPACTED ALTERNATIVES"),
            items.Text(
                "(A negative percentage means smaller space; shorter time)."),
            items.Table(header, rows)
        ]

    def chart_extra_compression_comparison(self) -> List[items.Table]:
        """Given compact alternatives like ASTC, ETC2, etc., creates a chart to
        contrast their sizes, load times compared agains their extra compressed
        versions (deflate, LZ4).
        """
        formats = list(self.__disk_size_per_format.keys())

        header = [f"Non-{self.extra_compression}=100%", "Size at Rest",
                  "Storage to GPU"]
        rows = []

        for index in range(0, len(formats), 2):
            subject_format = formats[index]
            extra_compressed_format = formats[index + 1]

            row = [f"{subject_format} {self.extra_compression}"]
            row.append(
                percentual_ratio(subject_format, extra_compressed_format,
                                 self.__disk_size_per_format))
            row.append(
                percentual_ratio(subject_format, extra_compressed_format,
                                 self.__decode_time_per_format,
                                 self.__gpu_trx_time_per_format))
            rows.append(row)

        return [
            items.Text(f"DOES {self.extra_compression.upper()} "
                       "REALLY IMPROVES MATTERS?"),
            items.Text(
                "(A negative percentage means smaller space; shorter time)."),
            items.Table(header, rows)
        ]


def accumulate(value: int,
               texture_format: str,
               accumulators: Dict[str, int],
               scalar: float = 1.0):
    """General accumulator function to be used to accumulate all data
    points."""
    accumulator = accumulators.get(texture_format, 0)
    accumulators[texture_format] = accumulator + value * scalar


def delete_entry(key, dictionary):
    """Given a dictionary, removes an entry if it exists."""
    if key in dictionary:
        del dictionary[key]


def percentual_ratio(subject, compressed, metric_1, metric_2=None):
    """Given some metrics, calculates the percentage of reduction that a
    compressed format brings over a subject one.
    It could be PNG (subject) vs. ETC2.
    It could be ETC2 (subject) vs. ETC2 Deflated. And so on.
    """
    compressed_value = metric_1[compressed] if not metric_2 \
        else metric_1[compressed] + metric_2[compressed]
    subject_value = metric_1[subject] if not metric_2 \
        else metric_1[subject] + metric_2[subject]

    return "{:.1%}".format(
        (1.0 * compressed_value / subject_value - 1.0)
    )


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
        return "Texture Formats" in datum.suite_id

    def __pump_datum_to_stats(self, datum: Datum):
        """Pushes the datum entry to the overall stats."""
        event_type = datum.get_custom_field("event_type")
        if event_type == "TEXTURE_LOAD_END" \
                or event_type == "TEXTURE_LOAD_FAILURE" \
                or event_type == "TEXTURE_GPU_TRANSFER":
            channels = datum.get_custom_field("texture.channels")
            texture_format, extra_compression = ReportUtils.decompose_format(
                datum.get_custom_field("texture.format"))

            composition_stats = self.__get_composition_stats(
                channels, extra_compression)
            composition_stats.apply_data_points(event_type, datum)

            if not ReportUtils.is_extra_compressed(extra_compression) \
                    and not ReportUtils.is_traditional_format(texture_format):
                composition_stats = self.__get_composition_stats(
                    channels, ReportUtils.DEFLATED)
                composition_stats.apply_data_points(event_type, datum)

                composition_stats = self.__get_composition_stats(
                    channels, ReportUtils.LZ4)
                composition_stats.apply_data_points(event_type, datum)

    def __get_composition_stats(self, channels: str, extra_compression: str) \
            -> ChannelCompositionStats:
        """Retrieves stats for a channel composition. If it doesn't exist yet,
        it creates these."""
        key = ReportUtils.key(channels, extra_compression)
        composition_stats = self.__stats_per_composition.get(key)
        if composition_stats is None:
            composition_stats = ChannelCompositionStats(channels,
                                                        extra_compression)
            self.__stats_per_composition[key] = composition_stats

        return composition_stats

    def render(self, ctx: SummaryContext) -> List[items.Item]:
        report_items = []

        report_items.extend(self.__report_channels_analysis(
            ReportUtils.RGB, ctx))
        report_items.append(items.Separator())
        report_items.extend(self.__report_channels_analysis(
            ReportUtils.RGBA, ctx))

        return report_items

    def __report_channels_analysis(self, channels: str,
                                   ctx: SummaryContext) -> List[items.Item]:
        """Given a channel composition (e.g., RGBA, reports an analysis of
        traditional formats vs. compressed ones, and compressed formats vs.
        extra compressed).
        """
        report_items = []

        composition_stats = self.__stats_per_composition.get(channels)
        if composition_stats:
            report_items.extend(self.__report_traditional_vs_compressed(
                channels, composition_stats, ctx))
            report_items.extend(
                self.__report_compressed_vs_extra(
                    channels, ReportUtils.DEFLATED, ctx
                ))

        return report_items

    def __report_traditional_vs_compressed(
            self, channels: str,
            composition_stats: ChannelCompositionStats,
            ctx: SummaryContext) -> List[items.Item]:
        """Reports stats about traditional formats (e.g., PNG) against
        compressed alternatives like ETC2."""
        report_items = [
            self.__graph_stats(f"{channels} Texture Formats Compared",
                               composition_stats, ctx)
        ]
        report_items.extend(composition_stats.chart_format_comparison())

        return report_items

    def __report_compressed_vs_extra(
            self, channels: str, extra_compression: str, ctx: SummaryContext) \
            -> List[items.Item]:
        """Reports stats about a compressed format like ASTC vs itself but
        extra compressed (e.g., LZ4)."""
        report_items = []
        extra_compressed_stats = \
            self.__stats_per_composition.get(
                ReportUtils.key(channels, extra_compression)
            )
        if extra_compressed_stats:
            report_items.append(items.Separator())
            report_items.append(
                self.__graph_stats(f"{channels} Vs. " +
                                   extra_compressed_stats.extra_compression,
                                   extra_compressed_stats, ctx)
            )
            report_items.extend(
                extra_compressed_stats.chart_extra_compression_comparison()
            )

        return report_items

    def __graph_stats(self,
                      title: str,
                      composition_stats: ChannelCompositionStats,
                      ctx: SummaryContext) -> items.Image:
        """Creates charts that show the different texture format performances
        in a compared way."""
        def plot_callback():
            fig = plt.figure(figsize=(4, 1.5))
            fig.subplots_adjust(top=0.75, hspace=0.6)

            total_columns = 2

            def create_axes(column):
                axes = axislines.Subplot(fig, 1, total_columns, 1 + column)
                fig.add_subplot(axes)
                return axes

            if (ReportUtils.is_extra_compressed(
                    composition_stats.extra_compression)):
                composition_stats.graph_usage_extra(create_axes(0))
                composition_stats.graph_elapsed_extra(create_axes(1))
            else:
                composition_stats.graph_usage(create_axes(0))
                composition_stats.graph_elapsed_times(create_axes(1))

        image_path = self.plot(ctx, plot_callback, title)
        return items.Image(image_path, self.device())
