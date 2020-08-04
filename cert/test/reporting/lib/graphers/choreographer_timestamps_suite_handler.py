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
"""
A grapher for the Choreographer Timestamps operation.
"""

from typing import List, Optional

import matplotlib.pyplot as plt
from matplotlib.patches import Rectangle

from lib.graphers.suite_handler import SuiteHandler, SuiteSummarizer
from lib.report import Datum, SummaryContext
import lib.items as items

# The maximum number of milliseconds a Choreographer timestamp can be from an
# EGL timestamp before the comparison is considered to be incompatible.
# (In other words, only allow for about half of 16.7 ms frame time.)
MAX_COMPARABLE_VARIANCE_MS = 9

# The arbitrary, maximum variance in milliseconds between a Choreographer
# timestamp and an EGL timestamp that is considered "passing".
MAX_PASSING_VARIANCE_MS = 3


class ChoreographerTimestamp:

    def __init__(self, timestamp_ns, vsync_offset_ns):
        self.timestamp_ns = timestamp_ns
        self.vsync_offset_ns = vsync_offset_ns

    def adjusted_timestamp(self):
        """Returns what the timestamp would have been without the VSYNC offset.
        """
        return self.timestamp_ns - self.vsync_offset_ns


class ChoreographerTimestampsSummarizer(SuiteSummarizer):
    """Suite summarizer for Choreographer Timestamps test."""

    @classmethod
    def default_handler(cls) -> SuiteHandler:
        return ChoreographerTimestampsSuiteHandler

    def render_synthesis(self, ctx):
        passed = 0
        failed = 0
        for handler in self.suite_handlers:
            if handler.fail:
                failed += 1
            else:
                passed += 1

        msg = f"{passed} passed, {failed} failed."
        return [items.Text(msg)]


class ChoreographerTimestampsSuiteHandler(SuiteHandler):
    """
    Implements SuiteHandler for Choreographer Timestamps test results.
    """

    def __init__(self, suite):
        super().__init__(suite)

        # Feature availability information
        self.has_fp_post_frame_callback = False
        self.has_egl_extension = False
        self.enable_surface_timestamps_success = False
        self.has_fp_get_instance = False
        self.has_fp_get_next_frame_id = False
        self.has_fp_get_frame_timestamp_supported = False

        self.fail = False

        # Collected data
        self.choreographer_timestamps = []
        self.vsync_offsets = {}
        self.egl_frame_timestamps = []
        self.egl_missed_frame_ids = []
        self.has_corrected_timestamps = False
        self.cannot_correct_timestamps = False

        # Data for plotting
        self.timestamp_pairs = []
        self.timestamp_variance = []
        self.variance_vsync_offsets = []

        self.parse_data()
        self.group_timestamps()
        self.determine_variances()

    @classmethod
    def can_handle_datum(cls, datum: Datum):
        return "ChoreographerTimestampsOperation" in datum.operation_id

    def parse_data(self):
        choreographer_ts_name = "choreographer_timestamp_ns"
        vsync_offset_name = "vsync_offset_ns"
        egl_frame_ts_name = "egl_frame_timestamp_ns"

        # Order data from report file
        for d in self.data:
            if "has_egl_extension" in d.custom:
                self.has_egl_extension = d.get_custom_field("has_egl_extension")
                self.enable_surface_timestamps_success = d.get_custom_field(
                    "enable_surface_timestamps_success")
                self.has_fp_get_instance = d.get_custom_field(
                    "has_fp_get_instance")
                self.has_fp_post_frame_callback = d.get_custom_field(
                    "has_fp_post_frame_callback")
                self.has_fp_get_next_frame_id = d.get_custom_field(
                    "has_fp_get_next_frame_id")
                self.has_fp_get_frame_timestamp_supported = d.get_custom_field(
                    "has_fp_get_frame_timestamp_supported")

            elif choreographer_ts_name in d.custom:
                # Collect Choreographer timestamps
                choreographer_ts = d.get_custom_field(choreographer_ts_name)
                vsync_offset_ns = d.get_custom_field(vsync_offset_name)

                # Check if timestamps need to be corrected (as in the case where
                # they were truncated to 32-bit integers).
                corrected_ts = correct_timestamp(choreographer_ts, d.timestamp)
                if corrected_ts is None:
                    self.cannot_correct_timestamps = True
                    break

                if choreographer_ts != corrected_ts:
                    self.has_corrected_timestamps = True

                corrected_ts -= vsync_offset_ns
                self.choreographer_timestamps.append(corrected_ts)
                self.vsync_offsets[corrected_ts] = vsync_offset_ns

            elif egl_frame_ts_name in d.custom:
                # Collect EGL display present timestamps
                if d.get_custom_field("success"):
                    timestamp = d.get_custom_field(egl_frame_ts_name)
                    self.egl_frame_timestamps.append(timestamp)
                else:
                    frame_id = d.get_custom_field("frame_id")
                    self.egl_missed_frame_ids.append(frame_id)

        self.choreographer_timestamps.sort()
        self.egl_frame_timestamps.sort()

    def group_timestamps(self):
        """Removes corresponding pairs of timestamps out of
        self.choreographer_timestamps and self.egl_frame_timestamps, placing
        them in self.timestamp_pairs."""

        # We wish to match Choreographer and EGL timestamps into pairs
        # corresponding to the same frame, but since we don't have comparable
        # frame IDs, we must rely on their timestamp values. (Even attempting
        # to assign frame IDs from the app is error prone, as the two timestamp
        # methods are asynchronous with the respect to one another.)

        # The goal is to take a Choreographer timestamp, find the EGL timestamp
        # nearest to it (and less than a frame's time away), and pair the two
        # together, at the same time removing their values from the "pool" of
        # Choreographer and EGL timestamps. Values remaining in either pool
        # after this process will help to identify cases where one timestamp
        # method provided a value and the other did not (or else some other type
        # of failure).

        def in_proximity(a: int, b: int):
            return abs(a - b) < (MAX_COMPARABLE_VARIANCE_MS * 1e6)

        for i, timestamp in enumerate(self.choreographer_timestamps):
            egl_index = binary_search_nearest(self.egl_frame_timestamps,
                                              timestamp)
            if not egl_index:
                continue
            egl_timestamp = self.egl_frame_timestamps[egl_index]
            if in_proximity(timestamp, egl_timestamp):
                self.timestamp_pairs.append((timestamp, egl_timestamp))
                self.egl_frame_timestamps.pop(egl_index)
                self.choreographer_timestamps[i] = 0  # flag for deletion

        # Remove values flagged for deletion
        self.choreographer_timestamps = [
            x for x in self.choreographer_timestamps if x != 0
        ]

    def determine_variances(self):
        """Calculate variance (how much Choreographer timestamps varied from EGL
        frame timestamps).
        """

        for choreographer_ts, egl_ts in self.timestamp_pairs:
            variance = egl_ts - choreographer_ts
            self.timestamp_variance.append(variance)
            self.variance_vsync_offsets.append(
                self.vsync_offsets[choreographer_ts])

    def render(self, ctx: SummaryContext) -> List[items.Item]:
        # Fail early if there is not enough data to run the evaluation
        if not self.has_fp_post_frame_callback:
            msg = "Missing function pointer for Choreographer " \
                "postFrameCallback."
            return [items.Text(msg)]

        if not self.has_egl_extension \
            or not self.has_fp_get_instance \
            or not self.has_fp_get_next_frame_id \
            or not self.has_fp_get_frame_timestamp_supported:
            msg = "EGL_ANDROID_get_frame_timestamps extension is unsupported."
            return [items.Text(msg)]

        if not self.enable_surface_timestamps_success:
            msg = "Failed to enable surface timestamps."
            return [items.Text(msg)]

        if not self.timestamp_pairs or not self.timestamp_variance:
            msg = f"Missing data:\n"
            msg += f"- {len(self.choreographer_timestamps)} timestamps from " \
                "Choreographer\n"
            msg += f"- {len(self.egl_frame_timestamps)} timestamps from " \
                "EGL_ANDROID_get_frame_timestamps"
            return [items.Text(msg)]

        if self.cannot_correct_timestamps:
            msg = f"Unable to correct truncated timestamps from Choreographer."
            return [items.Text(msg)]

        return self.render_report(ctx)

    def render_report(self, ctx: SummaryContext):
        variance_image_path = self.plot(ctx, self.render_variance_plot)
        variance_image = items.Image(variance_image_path, self.device())

        msg = "- Fail\n" if self.fail else "- Pass\n"
        if self.has_corrected_timestamps:
            msg += "- Has corrected timestamps\n"

        for missed_frame_id in self.egl_missed_frame_ids:
            msg += f"- Missed frame ({missed_frame_id})\n"

        if self.choreographer_timestamps:
            unused_count = len(self.choreographer_timestamps)
            label = "timestamps" if unused_count > 1 else "timestamp"
            msg += f"- {unused_count} unused Choreographer {label}\n"

        choreographer_detal_title = "Choreographer Deltas"
        choreographer_delta_image_path = self.plot(
            ctx, self.render_choreographer_delta_plot,
            choreographer_detal_title)
        choreographer_delta_image = items.Image(choreographer_delta_image_path,
                                                choreographer_detal_title)

        egl_delta_title = "EGL Display Present Deltas"
        egl_delta_image_path = self.plot(ctx, self.render_egl_delta_plot,
                                         egl_delta_title)
        egl_delta_image = items.Image(egl_delta_image_path, egl_delta_title)

        return [
            variance_image,
            choreographer_delta_image, egl_delta_image,
            items.Text(msg)
        ]

    def render_variance_plot(self):
        """
        We create a plot showing how much the Choreographer timestamps varied
        from the present values gather from EGL_ANDROID_get_frame_timestamps.
        The graph has an upper and lower bound, such that if a Choreographer
        timestamp varied from a present timestamp by enough to pass the upper or
        lower bound, we consider the device as failing the test, and the plot
        will be marked accordingly.
        """

        _, axes = plt.subplots()

        # Convert to milliseconds
        variance_ms = [i / 1_000_000 for i in self.timestamp_variance]

        if not variance_ms:
            return

        offsets_ms = [i / 1_000_000 for i in self.variance_vsync_offsets]

        # Set bounds
        max_y = MAX_COMPARABLE_VARIANCE_MS
        min_y = -max_y
        max_x = len(variance_ms)
        axes.axis([0, max_x, min_y, max_y])
        axes.set_xlabel("Frame")
        axes.set_ylabel("Timestamp Variance (ms)")

        upper_bound = MAX_PASSING_VARIANCE_MS
        lower_bound = -upper_bound

        # Line for upper bound
        axes.axhline(y=upper_bound, \
            linestyle='dashed', \
            linewidth=1, \
            color='black')

        # Line for lower bound
        axes.axhline(y=lower_bound, \
            linestyle='dashed', \
            linewidth=1, \
            color='black')

        # Plot points
        axes.plot(variance_ms, label="Variance")
        axes.plot(offsets_ms, label="VSYNC offset")
        axes.legend()

        # Set PASS/FAIL indicators
        lowest_val = min(variance_ms)
        highest_val = max(variance_ms)

        def add_region_marker(label: str, y: int, height: int, color: str):
            color_indicator = Rectangle((0, y - height / 2), \
                max_x, height, \
                facecolor=color, alpha=0.5)
            axes.add_patch(color_indicator)

            axes.text(max_x / 2, y, \
                label, \
                fontsize=24, \
                alpha=0.75, \
                horizontalalignment='center', \
                verticalalignment='center')

        if lowest_val >= lower_bound and highest_val <= upper_bound:
            add_region_marker('PASS', 0.0, upper_bound - lower_bound, 'green')

        if lowest_val < lower_bound:
            self.fail = True
            add_region_marker('FAIL', (min_y + lower_bound) / 2,
                              lower_bound - min_y, 'red')

        if highest_val > upper_bound:
            self.fail = True
            add_region_marker('FAIL', (max_y + upper_bound) / 2,
                              max_y - upper_bound, 'red')

    def render_choreographer_delta_plot(self):
        choreographer_deltas = []
        prev_choreographer_ts = None

        for choreographer_ts, _ in self.timestamp_pairs:
            choreographer_ts /= 1_000_000
            if prev_choreographer_ts:
                choreographer_deltas.append(choreographer_ts -
                                            prev_choreographer_ts)
            prev_choreographer_ts = choreographer_ts

        _, axes = plt.subplots()
        axes.axis([0, len(self.timestamp_pairs) - 1, 0, 67])
        axes.set_xlabel("Frame")
        axes.set_ylabel("Delta (ms)")
        axes.plot(choreographer_deltas)

    def render_egl_delta_plot(self):
        egl_deltas = []
        prev_egl_ts = None

        for _, egl_ts in self.timestamp_pairs:
            egl_ts /= 1_000_000
            if prev_egl_ts:
                egl_deltas.append(egl_ts - prev_egl_ts)
            prev_egl_ts = egl_ts

        _, axes = plt.subplots()
        axes.axis([0, len(self.timestamp_pairs) - 1, 0, 67])
        axes.set_xlabel("Frame")
        axes.set_ylabel("Delta (ms)")
        axes.plot(egl_deltas)


def binary_search_nearest(values: List[int], target: int) -> Optional[int]:
    """
    Given a sorted list of integers, `values`, and some `target` value, returns
    the index of `target`, or else the index of the value closest to `target`.
    Returns None if the list is empty.
    """
    if not values:
        return None

    low = 0
    high = len(values) - 1
    mid = 0

    # We modify the traditional binary search so that it closes in around a
    # value, rather than only finding an exact match. Once we narrow the list
    # down to two values, we can directly compare them.
    while high - low > 1:
        mid = int((low + high) / 2)

        # Usually binary search would set `high = mid - 1` or `low = mid + 1`,
        # but we can't be so quick to rule out the `mid` value just because
        # it wasn't an exact match--it may wind up being the closest value.
        if target < values[mid]:
            high = mid
        elif target > values[mid]:
            low = mid
        else:
            return mid

    if abs(target - values[low]) < abs(target - values[high]):
        return low
    return high


def make_unsigned_int(value: int, n: int) -> Optional[int]:
    """Convert an n-bit integer into an unsigned 64-bit integer equivalent,
    assuming two's complement encoding. (This is useful in recovering part of a
    64-bit unsigned integer that was truncated into a 32-bit signed integer.)

    Returns:
        None if the value is not an n-bit integer.
    """
    # Only handle n-bit integers.
    if value > (2**n) - 1 or value < -2**(n - 1):
        return None

    if value < 0:
        return 2**n + value

    return value


def correct_timestamp(timestamp: int, ref_timestamp: int) -> Optional[int]:
    """Attempts to undo the truncation of a 64-bit `timestamp` into a 32-bit
    integer by using an untruncated reference timestamp. If the `timestamp` is
    negative, two's-complement encoding is assumed and the value is converted
    to the unsigned form before attempting to handle truncation. If it can be
    determined that the `timestamp` was not truncated, this function will merely
    return a `timestamp` unaltered.

    `ref_timestamp` should be an untruncated 64-bit timestamp (in nanoseconds),
    whose value is within roughly two seconds of the original (untruncated)
    timestamp in order for the information recovery to succeed. IT IS IMPORTANT
    TO NOTE THAT THIS FUNCTION WILL BE UNABLE TO TELL IF THIS ASSUMPTION IS
    VIOLATED, so `ref_timestamp` must be close enough to the original timestamp.

    Returns:
        Timestamp (in nanoseconds) relative to ref_timestamp, or None if
        `timestamp` was less than 2^32 (clearly neither a 32-bit signed int nor
        a 64-bit unsigned int).
    """
    if (timestamp >> 32) > 0:
        return timestamp

    u_timestamp = make_unsigned_int(timestamp, 32)
    if u_timestamp is None:
        # Likely a negative 64-bit integer, which we can't handle
        return None

    # We wish to borrow the most significant 32 bits of `ref_timestamp` to fill
    # in the truncated bits of `timestamp`. This can be accomplished by a
    # bitwise operation on the two timestamps.
    #
    # However, we need to account for boundary conditions around multiples of
    # 2**32. If `ref_timestamp` was on one side of a boundary, and the original
    # untruncated `timestamp` was on the other, we will have to compensate by
    # adjusting `ref_timestamp`). Since we cannot know for sure which side the
    # original timestamp was on, we make the assumption that `ref_timestamp`
    # and the original timestamp were close enough together that we can use
    # their least significant 32 bits to make our decision. This assumption will
    # fail if the original timestamp and `ref_timestamp` were more than 2^16
    # nanoseconds apart. If the assumption holds, we can split the least
    # significant 32 bits into two ranges: [0, 2^16) and [2^16, 2^32).

    mid = 2**16

    # Get least significant 32 bits of `ref_timestamp` and `timestamp`
    ref_timestamp_lsb = ref_timestamp & 0x00000000ffffffff
    u_timestamp_lsb = u_timestamp & 0x00000000ffffffff

    if ref_timestamp_lsb < mid and u_timestamp_lsb >= mid:
        # Assumed to indicate that `ref_timestamp` is on the higher side of the
        # 2**32 boundary, while `timestamp` is on the lower side, so we need to
        # account for `ref_timestamp`'s upper 32-bits being too high.
        if ref_timestamp > 2**32:
            # Avoid wrap-around from underflow
            ref_timestamp -= 2**32
    elif ref_timestamp_lsb >= mid and u_timestamp_lsb < mid:
        # Assumed to indicate the opposite, that we need to account for
        # `ref_timestamp`'s upper 32-bits being too low. (We ignore handling
        # integer overflow by assuming our timestamps won't come near 2^64 ns.)
        ref_timestamp += 2**32

    return (ref_timestamp & 0xffffffff00000000) | (u_timestamp &
                                                   0x00000000ffffffff)


def test_correction():

    class Test:

        def __init__(self, ref, val):
            self.ref = ref
            self.val = val

    ref_delta = 5
    val_delta = 10
    tests = [
        Test(2**32 - val_delta, 2**32 - ref_delta),
        Test(2**32 + val_delta, 2**32 - ref_delta),
        Test(2**32 - val_delta, 2**32 + ref_delta),
        Test(2**32 + val_delta, 2**32 + ref_delta),
        Test(2**33 - val_delta, 2**33 - ref_delta),
        Test(2**33 + val_delta, 2**33 - ref_delta),
        Test(2**33 - val_delta, 2**33 + ref_delta),
        Test(2**33 + val_delta, 2**33 + ref_delta),
        Test(2**32, 2**32),
        Test(2**32 - 1, 2**32 - 1),
        Test(2**32 + 1, 2**32 + 1),
    ]

    passed = True
    for i, test in enumerate(tests):
        val = test.val & 0x00000000ffffffff
        result = correct_timestamp(val, test.ref)
        if result != test.val:
            passed = False
            print(f"- Test case {i+1}: want {test.val}, got {result}")
    if passed:
        print("test_correction passed")
    else:
        print("test_correction failed")


if __name__ == "__main__":
    test_correction()
