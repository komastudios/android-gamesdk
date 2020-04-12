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
"""Nougat Crash functional test suite handler.
"""

from typing import List

import lib.graphers as graph_commons
from lib.graphers.suite_handler import SuiteHandler
from lib.report import Suite


class NougatCrashSuiteHandler(SuiteHandler):
    """Grapher for nougat crash functional test (F1.20)."""

    def __init__(self, suite: Suite):
        super().__init__(suite)

        self.__memory_was_constrained = False
        self.__crash_did_happen = False
        self.__test_finished_normally = False

    @classmethod
    def can_handle_suite(cls, suite: Suite):
        return 'Nougat Crash' in suite.name

    @classmethod
    def can_render_summarization_plot(cls, suites: List['Suite']) -> bool:
        return False

    @classmethod
    def render_summarization_plot(cls, suites: List['Suite']) -> str:
        return None

    def determine_test_result_index(self):
        """Based on collected data, assess whether the test passed, not passed
        or inconclusive.
        A test that crashed is clear: it didn't pass.
        A test that didn't crash but it's memory wasn't constrained either,
        it's undetermined whether it would have crashed or not.
        A test that didn't crash but didn't finish normally, even if its
        memory was constrained, it's also undetermined.
        A test that didn't crash, got its memory constrained and finished
        normally, certainly passed.
        """
        result = 1
        if self.__crash_did_happen:
            result = 0
        elif self.__test_finished_normally and self.__memory_was_constrained:
            result = 2

        return result

    def compose_summary(self) -> str:
        """Based on the collected data, composes a string detailing why the
        conclusion was such."""
        sdk = self.suite.build['SDK_INT']
        os_name = 'Nougat' if sdk == 24 or sdk == 25 else 'non-Nougat'

        summary = ''
        if self.__crash_did_happen:
            summary = f'Crash occurred on this {os_name} device'
            if self.__memory_was_constrained:
                summary += ', as memory was constrained. '
            else:
                summary += ', in spite of not being memory-constrained. '
        else:
            summary = f"Crash didn't occur on this {os_name} device"
            if self.__memory_was_constrained:
                summary += ', in spite of being memory-constrained. '
            else:
                summary += ", as memory wasn't constrained either. "

        if not self.__test_finished_normally:
            summary += "Test finish wasn't detected, though."

        return summary

    def render_plot(self) -> str:
        for datum in self.data:
            if datum.operation_id == 'NougatSigabrtOperation':
                event = datum.get_custom_field('event')
                if event == 'on_trim_memory':
                    self.__memory_was_constrained = True
                elif event == 'SIGABRT':
                    self.__crash_did_happen = True
                elif event == 'test_ends':
                    self.__test_finished_normally = True

        result_index = self.determine_test_result_index()

        graph_commons.graph_functional_test_result(
            result_index, ['CRASH', 'UNDETERMINED', 'PASSED'])

        return self.compose_summary()

    @classmethod
    def handles_entire_report(cls, suites: List['Suite']):
        return False

    @classmethod
    def render_report(cls, raw_suites: List['Suite']):
        return ''
