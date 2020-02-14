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
"""Tests confirming that running a recipe results in a valid report
"""

import glob
import os
from pathlib import Path
import shutil
from typing import Dict, List
import unittest

import matplotlib.pyplot as plt

import lib.common
import lib.deployment
import lib.graphing
import lib.graphers

from lib.report import Suite
from lib.graphers.suite_handler import SuiteHandler
from run import RECIPE_DEFAULTS


class FakeOperationSuiteHandler(SuiteHandler):
    """SuiteHandler implementation for FakeOperation
    """

    @classmethod
    def can_handle_suite(cls, suite: Suite):
        return "FakeOperation" in suite.name

    @classmethod
    def can_render_summarization_plot(cls,
                                      suites: List['SuiteHandler']) -> bool:
        return True

    @classmethod
    def render_summarization_plot(cls, suites: List['SuiteHandler']) -> str:

        # fake summarization plot - just sum up counts
        start_count = 0
        wait_start_count = 0
        wait_end_count = 0
        stop_ordered_count = 0
        heartbeat_count = 0
        for suite in suites:
            our_data = [
                datum for datum in suite.data
                if datum.operation_id == "FakeOperation"
            ]
            for datum in our_data:
                event = datum.get_custom_field("event")
                if event == "Start":
                    start_count += 1
                elif event == "WaitStarted":
                    wait_start_count += 1
                elif event == "WaitFinished":
                    wait_end_count += 1
                elif event == "StopOrdered":
                    stop_ordered_count += 1
                elif event == "Heartbeat":
                    heartbeat_count += 1

        plt.bar([0, 1, 2, 3, 4], [
            start_count, wait_start_count, heartbeat_count, stop_ordered_count,
            wait_end_count
        ])

        plt.xticks(
            [0, 1, 2, 3, 4],
            ["Start", "WaitStart", "Heartbeat", "StopOrdered", "WaitFinished"])
        plt.xlabel("Event Counts")

        return "Summarization Looks Fantastic"

    def render_plot(self) -> str:

        our_data = [
            datum for datum in self.data
            if datum.operation_id == "FakeOperation"
        ]

        start_count = 0
        wait_start_count = 0
        wait_end_count = 0
        stop_ordered_count = 0
        heartbeat_count = 0
        for datum in our_data:
            event = datum.get_custom_field("event")
            if event == "Start":
                start_count += 1
            elif event == "WaitStarted":
                wait_start_count += 1
            elif event == "WaitFinished":
                wait_end_count += 1
            elif event == "StopOrdered":
                stop_ordered_count += 1
            elif event == "Heartbeat":
                heartbeat_count += 1

        plt.bar([0, 1, 2, 3, 4], [
            start_count, wait_start_count, heartbeat_count, stop_ordered_count,
            wait_end_count
        ])

        plt.xticks(
            [0, 1, 2, 3, 4],
            ["Start", "WaitStart", "Heartbeat", "StopOrdered", "WaitFinished"])
        plt.xlabel("Event Counts")

        return "Everything Looks Fine"


lib.graphers.HANDLERS.append(FakeOperationSuiteHandler)


class TestReportGeneration(unittest.TestCase):
    """Runs a fake operation locally and confirms the generated
    output both exists, and is valid
    """

    def setUp(self):
        self.out_dir = Path('./tmp/integration/test_report_generation')

        if self.out_dir.exists() and self.out_dir.is_dir():
            shutil.rmtree(self.out_dir)

        # (re)-create it
        if not os.path.exists(self.out_dir):
            os.makedirs(self.out_dir)

    def tearDown(self):
        pass

    def test_local_deployment(self):
        """Confirm test runs on local device(s)"""

        recipe_path = Path(
            './test/integration/data/recipes/fake_operation.yaml')

        recipe = lib.common.Recipe(recipe_path, Path(RECIPE_DEFAULTS))
        args = {"local": True}
        report_files = self.run_operation(recipe, args)
        attached_devices = lib.common.get_attached_devices()

        # we should have one report per attached device
        self.assertEqual(len(attached_devices), len(report_files))

        # confirm that report json has expected data in it
        for report_file in report_files:
            self.verify_report_file_contents(report_file, True)

        # render a report, verify it exists, etc
        self.verify_summary(verify_cross_suite_summary=False)

    def test_ftl_deployment(self):
        '''Confirms test runs on ftl'''

        recipe_path = Path(
            './test/integration/data/recipes/fake_operation.yaml')

        recipe = lib.common.Recipe(recipe_path, Path(RECIPE_DEFAULTS))
        args = {"ftl": True}
        report_files = self.run_operation(recipe, args)
        for report_file in report_files:
            self.verify_report_file_contents(report_file, False)

        # render a report, verify it exists, etc
        self.verify_summary(verify_cross_suite_summary=True)

    def run_operation(self, recipe_path: Path, args: Dict) -> List[Path]:
        '''run the fake operation recipe and return generated report json files
        '''
        lib.deployment.run_recipe(recipe_path, args, self.out_dir)

        # confirm that expected files exist
        report_files = [
            Path(f) for f in glob.glob(str(self.out_dir) + '/*.json')
        ]

        return report_files

    def verify_report_file_contents(self, report_file: Path,
                                    verify_event_counts: bool):
        '''Verify the contents of the given report_file
        Args:
            report_file: The report to verify
            verify_event_counts: if true, do a sanity check on the datum event
            data (this is optional, because tests run on FTL might fail, and
            as such, a successful deployment could result in results we would
            fail)
        '''
        suites = lib.graphing.load_suites(report_file)
        if not suites:
            print(f"report file {report_file} contained no suite data;"\
                " the device likely crashed")
            return

        suites_by_name = {suite.name: suite for suite in suites}
        self.assertIn("FakeOperation", suites_by_name.keys())

        suite = suites_by_name["FakeOperation"]

        our_data = [
            datum for datum in suite.data
            if datum.operation_id == "FakeOperation"
        ]

        if our_data:
            start_count = 0
            wait_start_count = 0
            wait_end_count = 0
            stop_ordered_count = 0
            heartbeat_count = 0
            for datum in our_data:
                event = datum.get_custom_field("event")
                if event == "Start":
                    start_count += 1
                elif event == "WaitStarted":
                    wait_start_count += 1
                elif event == "WaitFinished":
                    wait_end_count += 1
                elif event == "StopOrdered":
                    stop_ordered_count += 1
                elif event == "Heartbeat":
                    heartbeat_count += 1
                else:
                    self.fail("Unexpected event name")

                self.assertEqual(
                    datum.get_custom_field_numeric("duration_value"), 123)
                self.assertEqual(datum.get_custom_field_numeric("int_value"),
                                 32768)
                self.assertEqual(datum.get_custom_field("string_value"),
                                 "Hello World")
                self.assertEqual(datum.get_custom_field("bool_value"), True)

            if verify_event_counts:
                self.assertEqual(start_count, 1)
                self.assertEqual(wait_start_count, 1)
                self.assertGreaterEqual(heartbeat_count, 1)

    def verify_summary(self, verify_cross_suite_summary):
        '''verifies a summary document was created;
        looks for images and keywords. Does not validate that
        correct markdown was generated. More of a sniff-test.
        Args:
            verify_cross_suite_summary: If true, looks for the
            cross-suite summary section as well

        '''

        summary_files = [
            Path(f) for f in glob.glob(str(self.out_dir) + '/*.md')
        ]

        self.assertEqual(len(summary_files), 1)
        summary_file = summary_files[0]

        # the above will have created a report file; we can
        # search that file for expected content

        images_dir = self.out_dir / "images"
        image_files = [Path(f) for f in glob.glob(str(images_dir) + '/*.png')]
        needles = {image.name for image in image_files}
        needles.add('# FakeOperation')
        needles.add('Everything Looks Fine')
        if verify_cross_suite_summary:
            needles.add('Summarization Looks Fantastic')

        found = set()

        with open(summary_file, 'r') as file:
            for line in file:
                for image_name in needles:
                    if image_name in line:
                        found.add(image_name)

        self.assertEqual(needles, found)
