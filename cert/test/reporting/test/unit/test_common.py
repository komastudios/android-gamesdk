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
"""Tests for functions in lib.common
"""

from pathlib import Path
import shutil
import unittest

import lib.common


class TestLibCommon(unittest.TestCase):
    """Tests for functions in lib.common
    """

    def test_dict_lookup(self):
        """Test dict_lookup function"""
        test_dict = {
            "a": 1,
            "b": [0, 1, 2],
            "c": {
                "a": 2,
                "b": [4, 5, 6],
                "c": {
                    "a": 3
                }
            }
        }

        self.assertEqual(lib.common.dict_lookup(test_dict, "a", fallback=None),
                         1)
        self.assertEqual(lib.common.dict_lookup(test_dict, "b", fallback=None),
                         [0, 1, 2])
        self.assertEqual(
            lib.common.dict_lookup(test_dict, "c.a", fallback=None), 2)
        self.assertEqual(
            lib.common.dict_lookup(test_dict, "c.b", fallback=None), [4, 5, 6])
        self.assertEqual(
            lib.common.dict_lookup(test_dict, "c.c.a", fallback=None), 3)

        self.assertEqual(
            lib.common.dict_lookup(test_dict, "not.a.key.path", fallback=1), 1)

        with self.assertRaises(lib.common.MissingPropertyError):
            lib.common.dict_lookup(test_dict, "not.a.key.path", fallback=None)

    def test_ensure_dir(self):
        """Test that ensure_dir works"""
        lib.common.ensure_dir(Path("./a/b/c/d.txt"))

        the_dir = Path("./a/b/c")
        self.assertTrue(the_dir.exists())
        self.assertTrue(the_dir.is_dir())

        shutil.rmtree("./a")

    def test_create_output_dir(self):
        """Test that create_output_dir works"""

        # test basic usage
        lib.common.create_output_dir()
        self.assertTrue(Path("./out").exists())
        self.assertTrue(Path("./out").is_dir())

        # create a faux postfix
        lib.common.create_output_dir("__test")
        self.assertTrue(Path("./out/__test").exists())
        self.assertTrue(Path("./out/__test").is_dir())
        shutil.rmtree("./out/__test")

    def test_get_attached_devices(self):
        """Test that get_attached_devices works
        Note: To verify this works, a device must be attached!
        """
        result = lib.common.get_attached_devices()
        self.assertTrue(result)
        self.assertTrue(len(result) > 0)
        self.assertTrue(isinstance(result, list))

    def test_run_command(self):
        """Test that run_command works"""
        lib.common.run_command("touch foo.txt")
        self.assertTrue(Path("foo.txt").exists())
        self.assertTrue(Path("foo.txt").is_file())

        lib.common.run_command("rm foo.txt")
        self.assertFalse(Path("foo.txt").exists())
        self.assertFalse(Path("foo.txt").is_file())
