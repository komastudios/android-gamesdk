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
"""Tests task running from lib.tasks, lib.tasks_preflight
and lib.tasks_postflight
"""

from pathlib import Path
import shutil
import unittest

import yaml

import lib.common
import lib.tasks_runner
import lib.tasks_preflight
import lib.tasks_postflight


class TestLibTasksRunner(unittest.TestCase):
    """Tests task creation, execution, etc
    """

    def setUp(self):
        """setup a recipe to load from, and prep environment"""
        with open(Path("./test/data/tasks/tasks.yaml")) as recipe_file:
            tasks_dicts = yaml.load(recipe_file, Loader=yaml.FullLoader)
            self.preflight = tasks_dicts["preflight"]
            self.postflight = tasks_dicts["postflight"]
            self.device_deletion = tasks_dicts["device_deletion"]
            self.local_deletion = tasks_dicts["local_deletion"]

    def tearDown(self):
        pass

    def test_taskrunner_loader(self):
        """Confirm task runner loads correct tasks"""
        preflight_tasks = lib.tasks_runner.load(
            self.preflight, lib.tasks_runner.PREFLIGHT_TASKS)

        self.assertEqual(len(preflight_tasks), 3)
        for task in preflight_tasks:
            self.assertTrue(isinstance(task, lib.tasks_preflight.CopyTask))

        postflight_tasks = lib.tasks_runner.load(
            self.postflight, lib.tasks_runner.POSTFLIGHT_TASKS)

        self.assertEqual(len(postflight_tasks), 3)
        for task in postflight_tasks:
            self.assertTrue(isinstance(task, lib.tasks_postflight.CopyTask))

        dev_deletion_tasks = lib.tasks_runner.load(
            self.device_deletion, lib.tasks_runner.POSTFLIGHT_TASKS)

        self.assertEqual(len(dev_deletion_tasks), 3)
        for task in dev_deletion_tasks:
            self.assertTrue(isinstance(task, lib.tasks_postflight.DeleteTask))

        local_deletion_tasks = lib.tasks_runner.load(
            self.local_deletion, lib.tasks_runner.POSTFLIGHT_TASKS)

        self.assertEqual(len(local_deletion_tasks), 1)
        for task in local_deletion_tasks:
            self.assertTrue(isinstance(task, lib.tasks_postflight.DeleteTask))

    def test_copy_tasks(self):
        """Confirm copy tasks work"""
        preflight_tasks = lib.tasks_runner.load(
            self.preflight, lib.tasks_runner.PREFLIGHT_TASKS)

        postflight_tasks = lib.tasks_runner.load(
            self.postflight, lib.tasks_runner.POSTFLIGHT_TASKS)

        device_id = lib.common.get_attached_devices()[0]
        env = lib.tasks.Environment()
        src_file = Path("./test/data/tasks/hello.txt")
        self.assertTrue(src_file.exists())

        # each device->local copy task copies to this location
        dst_file = Path("./tmp/hello.txt")
        lib.common.ensure_dir(dst_file)
        if dst_file.exists():
            dst_file.unlink()

        for preflight, postflight in zip(preflight_tasks, postflight_tasks):
            # run preflight task to copy file over,
            # then the postflight to copy to tmp, then
            # confirm file existance and delete it
            lib.tasks_runner.run([preflight, postflight], device_id, env)
            self.assertTrue(dst_file.exists())
            dst_file.unlink()

    def test_local_deletion_tasks(self):
        """Test that deletion of local files works"""

        local_deletion_tasks = lib.tasks_runner.load(
            self.local_deletion, lib.tasks_runner.POSTFLIGHT_TASKS)

        src_file = Path("./test/data/tasks/hello.txt")
        dst_file = Path("./tmp/hello.txt")
        self.assertTrue(src_file.exists())

        device_id = lib.common.get_attached_devices()[0]
        env = lib.tasks.Environment()

        for task in local_deletion_tasks:
            # make a copy of our test file in tmp/
            shutil.copy(src_file, dst_file)
            self.assertTrue(dst_file.exists())

            lib.tasks_runner.run([task], device_id, env)
            self.assertFalse(dst_file.exists())

    def test_device_deletion_tasks(self):
        """Test that deletion of files on device works"""

        preflight_tasks = lib.tasks_runner.load(
            self.preflight, lib.tasks_runner.PREFLIGHT_TASKS)

        dev_deletion_tasks = lib.tasks_runner.load(
            self.device_deletion, lib.tasks_runner.POSTFLIGHT_TASKS)

        device_id = lib.common.get_attached_devices()[0]
        env = lib.tasks.Environment()

        for copy_task, delete_task in zip(preflight_tasks, dev_deletion_tasks):
            # copy the file over, then delete it
            lib.tasks_runner.run([copy_task, delete_task], device_id, env)
