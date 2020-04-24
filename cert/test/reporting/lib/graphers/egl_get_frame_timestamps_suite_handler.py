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
"""EGL Get Frame Timestamps functional test suite handler.
"""

from typing import List

from lib.graphers.suite_handler import SuiteHandler, SuiteSummarizer
from lib.report import Datum, SummaryContext, Suite
import lib.summary_formatters.format_items as fmt


class EGLGetFrameTimestampsSummarizer(SuiteSummarizer):
    """Suite summarizer for EGL Get Frame Timestamps feature test."""
    @classmethod
    def default_handler(cls) -> SuiteHandler:
        return EGLGetFrameTimestampsSuiteHandler

    def render_synthesis(self, ctx):
        text = """The EGL_ANDROID_get_frame_timestamps extension has over a
dozen features, some of which may not be supported. The major components are
given in the "Summary" table, while the details of "Composite Query Support"
and "Frame Timestamp Support" are given below."""
        return [fmt.Text(text)]


class EGLGetFrameTimestampsSuiteHandler(SuiteHandler):
    """
    """

    def __init__(self, suite: Suite):
        super().__init__(suite)

        # Data is collected into three groups
        self.availability_data = None
        self.composite_data = None
        self.present_time_data = None

        # Get main data groups
        for d in self.data:
            custom = d.custom
            if 'has_ext_egl_android_get_frame_timestamps' in custom:
                self.availability_data = AvailabilityData(d)
            elif 'supports_composite_deadline' in custom:
                self.composite_data = CompositeData(d)
            elif 'supports_requested_present_time' in custom:
                self.present_time_data = FrameTimestampData(d)

        if self.availability_data \
            and self.composite_data \
            and self.present_time_data:

            # Add additional data to groups
            for d in self.data:
                custom = d.custom
                if 'next_frame_id' in custom:
                    self.availability_data.add_next_frame_id(d)
                elif 'measurement_name' in custom:
                    self.composite_data.add_measurement(d)
                elif 'event_name' in custom:
                    self.present_time_data.add_event(d)

            # Add results from supporting data to main group (availability)
            self.availability_data.composite_works = self.composite_data.works()
            self.availability_data.present_time_works = \
                self.present_time_data.works()

    @classmethod
    def can_handle_datum(cls, datum: Datum):
        return datum.suite_id == "EGL Get Frame Timestamps"

    def render(self, ctx: SummaryContext) -> List[fmt.Item]:
        if self.availability_data \
            and self.composite_data \
            and self.present_time_data:

            txt = "Pass" if self.availability_data.extension_works() else "Fail"
            return [
                fmt.Text(txt),
                self.availability_data.table(),
                self.composite_data.table(),
                self.present_time_data.table()
            ]

        return [fmt.Text("Unavailable")]

# ------------------------------------------------------------------------------

def table_row(description: str, available: bool, working: bool) -> List[str]:
    cols = [description]
    if available:
        cols.append("Y")
        cols.append("Y" if working else "N")
    else:
        cols.append("N")
        cols.append("n/a")
    return cols

# ------------------------------------------------------------------------------

class AvailabilityData():
    def __init__(self, datum: Datum):
        # General
        self.extension_available = datum.get_custom_field(
            "has_ext_egl_android_get_frame_timestamps")
        self.did_enable_surface_egl_timestamps = datum.get_custom_field(
            "did_enable_surface_egl_timestamps")

        # Next frame id
        self.next_frame_id_available = datum.get_custom_field(
            "has_fp_get_next_frame_id")
        self.next_frame_ids = []

        # Composite
        has_fp_get_compositor_timing_supported = datum.get_custom_field(
            "has_fp_get_compositor_timing_supported")
        has_fp_get_compositor_timing = datum.get_custom_field(
            "has_fp_get_compositor_timing")
        self.composite_available = has_fp_get_compositor_timing_supported and has_fp_get_compositor_timing
        self.composite_works = False

        # Frame timestamp
        has_fp_get_frame_timestamp_supported = datum.get_custom_field(
            "has_fp_get_frame_timestamp_supported")
        has_fp_get_frame_timestamps = datum.get_custom_field(
            "has_fp_get_frame_timestamps")
        self.frame_timestamp_available = has_fp_get_frame_timestamp_supported \
            and has_fp_get_frame_timestamps
        self.present_time_works = False

    def add_next_frame_id(self, datum: Datum):
        self.next_frame_ids.append(datum.get_custom_field('next_frame_id'))

    def next_frame_id_works(self):
        return len(set(self.next_frame_ids)) == len(self.next_frame_ids)

    def extension_works(self):
        return self.did_enable_surface_egl_timestamps and self.composite_works \
            and self.present_time_works and self.next_frame_id_works()

    def table(self) -> fmt.Table:
        extension = table_row("EGL_ANDROID_get_frame_timestamps",
                              self.extension_available, self.extension_works())
        composite = table_row("Composite Query Support",
                              self.composite_available, self.composite_works)
        frame_timestamp = table_row("Frame Timestamp Support",
                                    self.frame_timestamp_available,
                                    self.present_time_works)
        frame_id = table_row("Get Next Frame ID", self.next_frame_id_available,
                             self.next_frame_id_works())
        surface = table_row("Surface Timestamp Enabled",
                            self.did_enable_surface_egl_timestamps,
                            self.did_enable_surface_egl_timestamps)

        header = ["Summary", "Available", "Working"]
        rows = [extension, composite, frame_timestamp, frame_id, surface]
        return fmt.Table(header, rows)

# ------------------------------------------------------------------------------

class CompositeData():
    def __init__(self, datum: Datum):
        self.supports_composite_deadline = datum.get_custom_field(
            "supports_composite_deadline")
        self.supports_composite_interval = datum.get_custom_field(
            "supports_composite_interval")
        self.supports_composite_to_present_latency = datum.get_custom_field(
            "supports_composite_to_present_latency")
        self.measurements = []

    def add_measurement(self, datum: Datum):
        self.measurements.append(datum.custom)

    def deadline_query_works(self):
        for measurement in self.measurements:
            if measurement['measurement_name'] == "EGL_COMPOSITE_DEADLINE_ANDROID":
                return True
        return False

    def interval_query_works(self):
        for measurement in self.measurements:
            if measurement['measurement_name'] == "EGL_COMPOSITE_INTERVAL_ANDROID":
                return True
        return False

    def latency_query_works(self):
        for measurement in self.measurements:
            if measurement['measurement_name'] == "EGL_COMPOSITE_TO_PRESENT_LATENCY_ANDROID":
                return True
        return False

    def works(self):
        return self.deadline_query_works() and self.interval_query_works() \
            and self.latency_query_works()

    def table(self) -> fmt.Table:
        deadline = table_row("Composite Deadline",
                             self.supports_composite_deadline,
                             self.deadline_query_works())

        interval = table_row("Composite Interval",
                             self.supports_composite_interval,
                             self.interval_query_works())

        latency = table_row("Composite to Present Latency",
                      self.supports_composite_to_present_latency,
                      self.latency_query_works())

        header = ["Composite Query Support", "Available", "Working"]
        rows = [deadline, interval, latency]
        return fmt.Table(header, rows)

# ------------------------------------------------------------------------------

class FrameTimestampData():
    def __init__(self, datum: Datum):
        self.supports_requested_present_time = datum.custom[
            "supports_requested_present_time"]
        self.supports_rendering_complete_time = datum.custom[
            "supports_rendering_complete_time"]
        self.supports_composition_latch_time = datum.custom[
            "supports_composition_latch_time"]
        self.supports_first_composition_start_time = datum.custom[
            "supports_first_composition_start_time"]
        self.supports_last_composition_start_time = datum.custom[
            "supports_last_composition_start_time"]
        self.supports_first_comp_gpu_finished_time = datum.custom[
            "supports_first_composition_gpu_finished_time"]
        self.supports_display_present_time = datum.custom[
            "supports_display_present_time"]
        self.supports_dequeue_ready_time = datum.custom[
            "supports_dequeue_ready_time"]
        self.supports_reads_done_time = datum.custom["supports_reads_done_time"]
        self.events = []

    def add_event(self, datum: Datum):
        self.events.append(datum.custom)

    def event_works(self, name: str):
        for event in self.events:
            if event['event_name'] == name:
                return True
        return False

    def requested_present_time_works(self):
        return self.event_works("EGL_REQUESTED_PRESENT_TIME_ANDROID")

    def rendering_complete_time_works(self):
        return self.event_works("EGL_RENDERING_COMPLETE_TIME_ANDROID")

    def composition_latch_time_works(self):
        return self.event_works("EGL_COMPOSITION_LATCH_TIME_ANDROID")

    def first_comp_start_time_works(self):
        return self.event_works("EGL_FIRST_COMPOSITION_START_TIME_ANDROID")

    def last_comp_start_time_works(self):
        return self.event_works("EGL_LAST_COMPOSITION_START_TIME_ANDROID")

    def first_comp_finished_time_works(self):
        return self.event_works(
            "EGL_FIRST_COMPOSITION_GPU_FINISHED_TIME_ANDROID")

    def display_present_time_works(self):
        return self.event_works("EGL_DISPLAY_PRESENT_TIME_ANDROID")

    def dequeue_ready_time_works(self):
        return self.event_works("EGL_DEQUEUE_READY_TIME_ANDROID")

    def reads_done_time_works(self):
        return self.event_works("EGL_READS_DONE_TIME_ANDROID")

    def works(self):
        return self.requested_present_time_works() \
            and self.rendering_complete_time_works() \
            and self.composition_latch_time_works() \
            and self.first_comp_start_time_works() \
            and self.last_comp_start_time_works() \
            and self.first_comp_finished_time_works() \
            and self.display_present_time_works() \
            and self.dequeue_ready_time_works() \
            and self.reads_done_time_works()

    def table(self) -> fmt.Table:
        req_present = table_row("Requested Present Time",
                                self.supports_requested_present_time,
                                self.requested_present_time_works())
        rendering_complete = table_row("Rendering Complete Time",
                             self.supports_rendering_complete_time,
                             self.rendering_complete_time_works())
        latch = table_row("Composition Latch Time",
                          self.supports_composition_latch_time,
                          self.composition_latch_time_works())
        first_comp = table_row("First Composition Start Time",
                               self.supports_first_composition_start_time,
                               self.first_comp_start_time_works())
        last_comp = table_row("Last Composition Start Time",
                              self.supports_last_composition_start_time,
                              self.last_comp_start_time_works())
        comp_finished = table_row("First Composition GPU Finished Time",
                                  self.supports_first_comp_gpu_finished_time,
                                  self.first_comp_finished_time_works())
        display_present = table_row("Display Present Time",
                                    self.supports_display_present_time,
                                    self.display_present_time_works())
        dequeue_ready = table_row("Dequeue Ready Time",
                                  self.supports_dequeue_ready_time,
                                  self.dequeue_ready_time_works())
        reads_done = table_row("Reads Done Time", self.supports_reads_done_time,
                               self.reads_done_time_works())

        header = ["Frame Timestamp Support", "Available", "Working"]
        rows = [
            req_present, rendering_complete, latch, first_comp, last_comp,
            comp_finished, display_present, dequeue_ready, reads_done
        ]
        return fmt.Table(header, rows)
