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
"""Tests lib.graphing functionality
"""

from pathlib import Path
import unittest

import lib.graphing


class TestLibGraphing(unittest.TestCase):
    """Tests load_suites
    TODO(shamyl@google.com) How to test actual graphing functionality?
    """

    def setUp(self):
        pass

    def tearDown(self):
        pass

    def test_load_suites(self):
        """Test that load_suites works as expected"""
        report_file = Path("./test/unit/data/graphing/report.json")
        suites = lib.graphing.load_suites(report_file)

        # the report_file is a synthetic report with contents
        # from 2 different test runs (16 datums each)
        self.assertEqual(len(suites), 2)

        names = [s.name for s in suites]
        self.assertTrue("Affinity Test" in names)
        self.assertTrue("32bit Depth Clear" in names)

        for suite in suites:
            self.assertEqual(suite.build["MANUFACTURER"], "Google")
            self.assertEqual(suite.file, report_file)
            self.assertEqual(len(suite.data), 16)
            self.assertEqual(suite.identifier(),
                             "Google Pixel 3 (blueline) SDK 29")
