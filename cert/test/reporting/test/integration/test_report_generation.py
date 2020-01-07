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
from typing import List
import unittest

import matplotlib.pyplot as plt

import lib.common
import lib.deployment
import lib.graphing

from lib.report import Suite
from lib.graphers.suite_handler import SuiteHandler
from lib.graphers import HANDLERS


class FakeOperationSuiteHandler(SuiteHandler):
    """SuiteHandler implementation for FakeOperation
    """

    @classmethod
    def can_handle_suite(cls, suite: Suite):
        return "FakeOperation" in suite.name

    @classmethod
    def can_render_summarization_plot(cls,
                                      suites: List['SuiteHandler']) -> bool:
        return False

    @classmethod
    def render_summarization_plot(cls, suites: List['SuiteHandler']) -> str:
        return None

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


HANDLERS.append(FakeOperationSuiteHandler)


class TestReportGeneration(unittest.TestCase):
    """Runs a fake operation locally and confirms the generated
    output both exists, and is valid
    """

    def setUp(self):
        self.out_dir = Path('./tmp/integration/test_report_generation')

        # if the out dir exists, ensure it's gone
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
            './test/integration/data/recipes/fake_operation_local.yaml')

        report_files = self.run_operation(recipe_path)
        attached_devices = lib.common.get_attached_devices()

        # we should have one report per attached device
        self.assertTrue(len(attached_devices) == len(report_files))

        # confirm that report json has expected data in it
        for report_file in report_files:
            self.verify_report_file_contents(report_file)

        # render a report, verify it exists, etc
        self.verify_report_render(report_files)

    def test_ftl_deployment(self):
        '''Confirms test runs on ftl'''

        recipe_path = Path(
            './test/integration/data/recipes/fake_operation_ftl.yaml')

        report_files = self.run_operation(recipe_path)
        for report_file in report_files:
            self.verify_report_file_contents(report_file)

    def run_operation(self, recipe_path: Path) -> List[Path]:
        '''run the fake operation recipe and return generated report json files
        '''
        lib.deployment.run_recipe(recipe_path, self.out_dir)

        # confirm that expected files exist
        report_files = [
            Path(f) for f in glob.glob(str(self.out_dir) + '/*.json')
        ]

        return report_files

    def verify_report_file_contents(self, report_file: Path):
        '''Verify the contents of the given report_file
        '''
        suites = lib.graphing.load_suites(report_file)
        self.assertTrue(len(suites) == 1)

        suite = suites[0]
        self.assertEqual("FakeOperation", suite.name)

        our_data = [
            datum for datum in suite.data
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
            else:
                self.fail("Unexpected event name")

            self.assertEqual(datum.get_custom_field_numeric("duration_value"),
                             123)
            self.assertEqual(datum.get_custom_field_numeric("int_value"), 32768)
            self.assertEqual(datum.get_custom_field("string_value"),
                             "Hello World")
            self.assertEqual(datum.get_custom_field("bool_value"), True)

        self.assertEqual(start_count, 1)
        self.assertEqual(wait_start_count, 1)
        self.assertGreaterEqual(heartbeat_count, 1)

    def verify_report_render(self, report_files: List[Path]):
        '''renders the provided report files and minimally verifies contents'''
        report_file = self.out_dir / "report.md"
        lib.graphing.render_report_document(
            report_files, report_file,
            lib.graphing.DocumentFormat.MARKDOWN, 150)

        # the above will have created a report file; we can
        # search that file for expected content

        images_dir = self.out_dir / "images"
        image_files = [
            Path(f) for f in glob.glob(str(images_dir) + '/*.png')
        ]
        needles = {image.name for image in image_files}
        needles.add('# FakeOperation')
        needles.add('Everything Looks Fine')
        found = set()

        with open(report_file, 'r') as file:
            for line in file:
                for image_name in needles:
                    if image_name in line:
                        found.add(image_name)

        self.assertEqual(needles, found)
