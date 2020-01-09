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
"""Google Drive summary upload tests.
"""

from os import makedirs
from pathlib import Path
from shutil import rmtree
import unittest

TMP_DIR = "./tmp/integration/test_google_drive"

class TestGoogleDriveAccess(unittest.TestCase):
    """Google Drive access test suite."""

    def setUp(self):
        self.__out_dir = Path(TMP_DIR)

        if self.__out_dir.exists():
            rmtree(self.__out_dir)

        # (re)-create it
        if not self.__out_dir.exists():
            makedirs(self.__out_dir)

    def test_canary(self):
        pass

    def tearDown(self):
        if self.__out_dir.exists():
            rmtree(self.__out_dir)
