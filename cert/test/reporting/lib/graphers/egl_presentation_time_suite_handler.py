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
"""A grapher for the EGL Presentation Time operation, which tests the
EGL_ANDROID_presentation_time extension."""

from typing import List, Dict
from enum import Enum

from lib.graphers.suite_handler import SuiteHandler
from lib.report import Suite
from lib.graphers.common_graphers import graph_functional_test_result


class TestResult(Enum):
    """Result used for either a specific piece of data or the test in general"""
    SUCCESS = 1
    FAILURE = 2
    INCONCLUSIVE = 3
    UNAVAILABLE = 4


class EGLPresentationTimeSuiteHandler(SuiteHandler):
    """Implements SuiteHandler for EGL Presentation Time test results."""

    def __init__(self, suite):
        super().__init__(suite)

        self.extension_availability = None
        self.rows = []

        for row in self.data:
            if 'requested' in row.custom and 'reported' in row.custom:
                self.rows.append(row.custom)
            elif 'ext_egl_android_presentation_time' in row.custom:
                self.extension_availability = row.custom

    @classmethod
    def can_handle_suite(cls, suite: Suite):
        return 'EGL Presentation Time' in suite.name

    @classmethod
    def can_render_summarization_plot(cls, suites: List['Suite']) -> bool:
        return False

    @classmethod
    def render_summarization_plot(cls, suites: List['Suite']) -> str:
        return None

    def render_plot(self) -> str:
        """Evalutes the data, renders the (success|failure|inconclusive) plot,
        and returns a list of any issues encountered."""
        text = ''

        # Evaluate availability
        result = self.evaluate_availability(self.extension_availability)

        if result is TestResult.UNAVAILABLE:
            text += 'Extension unavailable\n\n'
        elif result is TestResult.INCONCLUSIVE:
            text += 'Supporting extension EGL_ANDROID_get_frame_timestamps '
            text += 'was not available (used to determine when a frame was '
            text += 'presented)\n\n'
        elif result is TestResult.SUCCESS:
            # Evaluate timestamps
            successes, failures, inconclusive, error_messages = \
                self.evaluate_timestamps(self.rows)
            if failures > 0:
                result = TestResult.FAILURE # update result
            text += f'{successes} sucessful, '
            text += f'{failures} failed, '
            text += f'{inconclusive} inconclusive.\n\n'
            text += error_messages

            # Check for duplicate frame ids. (Not regarded as a failure because
            # it's not a feature of the EGL_ANDROID_presentation_time extension,
            # but it would be nice to know if there are any duplicates.)
            frame_ids = [row['frame_id'] for row in self.rows]
            if len(frame_ids) is not len(set(frame_ids)):
                error_messages += '- One or more duplicate frame ids.\n'

        # Summarize results
        result_labels = ['Failure', 'Inconclusive', 'Success']
        if result is TestResult.SUCCESS:
            result_flag = 2
        elif result is TestResult.FAILURE:
            result_flag = 0
        elif result is TestResult.INCONCLUSIVE:
            result_flag = 1
        elif result is TestResult.UNAVAILABLE:
            result_flag = 1

        graph_functional_test_result(result_flag, result_labels)

        return text

    def evaluate_availability(self, info: Dict) -> TestResult:
        """Returns a TestResult based on data like the following:
        "custom": {
            "enabled_surface_timestamps": true,
            "ext_egl_android_presentation_time": true,
            "pfn_presentation_time_android": true,
            "ext_egl_android_get_frame_timestamps": true,
            "pfn_get_next_frame_id": true,
            "pfn_get_frame_timestamps": true,
            "pfn_get_frame_timestamp_supported": true,
            "request_timestamp_supported": true,
            "present_timestamp_supported": true
        }"""
        if not info['ext_egl_android_presentation_time'] \
            or not info['pfn_presentation_time_android']:
            return TestResult.UNAVAILABLE

        #pylint: disable=too-many-boolean-expressions
        if not info['ext_egl_android_get_frame_timestamps'] \
            or not info['pfn_get_next_frame_id'] \
            or not info['pfn_get_frame_timestamps'] \
            or not info['pfn_get_frame_timestamp_supported'] \
            or not info['request_timestamp_supported'] \
            or not info['present_timestamp_supported']:
            return TestResult.INCONCLUSIVE

        if not info['enabled_surface_timestamps']:
            return TestResult.FAILURE

        return TestResult.SUCCESS


    def evaluate_timestamps(self, rows: List[Dict]) -> (int, int, int, str):
        """Returns the number of successes, failures, and inconclusive results
        from setting the presentation time, as well as a list of any issues
        encountered for failures."""
        successes = 0
        failures = 0
        inconclusive = 0
        error_messages = ''

        for row in rows:
            result, msg = self.did_presentation_time_succeed(row)
            if result is TestResult.SUCCESS:
                successes += 1
            elif result is TestResult.FAILURE:
                failures += 1
            elif result is TestResult.INCONCLUSIVE:
                inconclusive += 1
            if len(msg) > 0:
                error_messages += f"- {msg}\n"

        return (successes, failures, inconclusive, error_messages)

    def did_presentation_time_succeed(self, datum: Dict) -> (TestResult, str):
        """Returns the result of a datum with a string an error message.
        A individual datum looks like the following:
        "custom": {
            "frame_id": 24,
            "requested": {
                "time": 236706770967,
                "set": true
            },
            "reported": {
                "requested": {
                    "time": 236706770967,
                    "set": true
                },
                "actual": {
                    "time": 236723202745,
                    "set": true
                },
                "requested_lte_actual": true
            }
        }"""
        if not datum['requested']['set'] or \
            not datum['reported']['requested']['set'] or \
            not datum['reported']['actual']['set']:
            return (TestResult.INCONCLUSIVE, '')

        frame_id = datum['frame_id']
        requested_ns = datum['requested']['time']
        reported_requested_ns = datum['reported']['requested']['time']
        reported_actual_ns = datum['reported']['actual']['time']

        if requested_ns != reported_requested_ns:
            msg = f"Reported requested presentation time \
{reported_requested_ns} for frame {frame_id} does not match the time that was \
set {requested_ns}."
            return (TestResult.FAILURE, msg)

        if reported_actual_ns < reported_requested_ns:
            msg = f"Frame {frame_id} was presented at {reported_actual_ns} but \
scheduled for no earlier than {reported_requested_ns}."
            return (TestResult.FAILURE, msg)

        return (TestResult.SUCCESS, '')
