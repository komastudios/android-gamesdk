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
"""Tests lib.build functionality
"""

import unittest

import lib.build

class TestLibBuild(unittest.TestCase):
    """Tests loading report data, parsing, etc
    """

    def setUp(self):
        pass

    def tearDown(self):
        pass

    def test_builds_apk(self):
        """Confirm an APK is built"""
        clean = True
        release = False
        expected_apk_path = lib.build.get_apk_path(release)
        if expected_apk_path.exists():
            expected_apk_path.unlink()

        apk_path = lib.build.build_apk(clean=clean, release=release)

        self.assertEqual(expected_apk_path, apk_path)
        self.assertTrue(apk_path.exists())
        self.assertTrue(apk_path.is_file())
