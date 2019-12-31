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
"""Tests lib.report functions
"""

from pathlib import Path
import shutil
import unittest

import lib.common
import lib.report


class TestLibReport(unittest.TestCase):
    """Tests loading report data, parsing, etc
    """

    def setUp(self):
        pass

    def tearDown(self):
        pass

    def test_to_float(self):
        """Test to_float helper function"""
        self.assertEqual(lib.report.to_float(123.45), 123.45)
        self.assertEqual(lib.report.to_float("123.45"), 123.45)
        self.assertEqual(lib.report.to_float("True"), 1)
        self.assertEqual(lib.report.to_float("False"), 0)

        self.assertEqual(lib.report.to_float("500 nanoseconds"), 500)
        self.assertEqual(lib.report.to_float("500 ns"), 500)
        self.assertEqual(lib.report.to_float("500 nsec"), 500)

        self.assertEqual(lib.report.to_float("500 milliseconds"), 500)
        self.assertEqual(lib.report.to_float("500 ms"), 500)
        self.assertEqual(lib.report.to_float("500 msec"), 500)

        self.assertEqual(lib.report.to_float("500 seconds"), 500)
        self.assertEqual(lib.report.to_float("500 s"), 500)
        self.assertEqual(lib.report.to_float("500 sec"), 500)

    def test_flatten_dict(self):
        """Test the lib.report.flatten_dict method"""
        test_dict = {
            "foo": 1,
            "bar": {
                "baz": 2
            },
            "baz": {
                "qux": 5,
                "corge": {
                    "grault": 6
                }
            }
        }

        expected = [("foo", 1), ("bar.baz", 2), ("baz.qux", 5),
                    ("baz.corge.grault", 6)]

        flattened = lib.report.flatten_dict(test_dict)
        self.assertEqual(len(flattened), 4)


        for expected_tup in expected:
            self.assertTrue(expected_tup in flattened)

    def test_report_load(self):
        """Test report loading functionality"""

        # this is a standard report JSON file, but short for convenience
        report_file = Path("./test/data/report/report.json")
        build_info, data = lib.report.load_report(report_file)

        # just verify build info exists and has an expected field
        self.assertTrue("MANUFACTURER" in build_info)
        self.assertEqual(build_info["MANUFACTURER"], "Google")

        self.assertEqual(len(data), 16)
        for datum in data:

            # test that data loaded correctly
            self.assertTrue(isinstance(datum, lib.report.Datum))
            self.assertGreaterEqual(datum.cpu_id, 0)
            self.assertGreaterEqual(datum.issue_id, 0)
            self.assertGreaterEqual(int(datum.thread_id), 0)
            self.assertGreaterEqual(datum.timestamp, 0)
            self.assertEqual(datum.suite_id, "Affinity Test")
            self.assertEqual(datum.operation_id, "ScheduleAffinityOperation")
            self.assertGreaterEqual(
                datum.get_custom_field_numeric("expected_cpu"), 0)

    def test_datum_serialization(self):
        """Test datum serialization and load"""
        report_file = Path("./test/data/report/report.json")
        _, data = lib.report.load_report(report_file)

        for datum in data:
            json_data = datum.to_json()
            clone_datum = lib.report.Datum.from_json(json_data)
            self.assertEqual(datum, clone_datum)

    def test_report_save(self):
        """Test that we write reports correctly"""
        report_file = Path("./test/data/report/report.json")
        build_info, data = lib.report.load_report(report_file)

        # copy loaded report data to a tmp file
        tmp_report_file = Path("./tmp/report.json")
        lib.common.ensure_dir(tmp_report_file)
        lib.report.save_report(tmp_report_file, build_info, data)

        # load clone and compare
        build_info_clone, data_clone = lib.report.load_report(tmp_report_file)
        self.assertEqual(build_info, build_info_clone)
        self.assertEqual(data, data_clone)

        tmp_report_file.unlink()

    def test_report_name_normalization_and_parsing(self):
        """Test lib.report.normalize_report_name and
        lib.report.get_device_product_and_api
        """
        report_file = Path("./test/data/report/report.json")
        tmp_report_file = Path("./tmp/report.json")
        shutil.copy(report_file, tmp_report_file)

        tmp_report_file = lib.report.normalize_report_name(tmp_report_file)
        self.assertEqual(tmp_report_file.name, "report_blueline_29.json")

        product, api = lib.report.get_device_product_and_api(tmp_report_file)
        self.assertEqual(product, "blueline")
        self.assertEqual(api, 29)

        tmp_report_file.unlink()
