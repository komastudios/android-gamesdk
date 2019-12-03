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

from pathlib import Path
from typing import Any, Dict, List

from lib.build import APP_ID
from lib.common import *


class CopyTaskError(Error):
    """Exception raised when CopyTask can't do it's thing

    Attributes:
        message -- explanation of the error
    """

    def __init__(self, message):
        self.message = message


class Task(object):
    """Base class for preflight tasks"""

    def __init__(self, config: Dict):
        self.action = config["action"]

    def run(self, device_id: str):
        pass


class CopyTask(Task):
    """Represents a copy action copying files from local filesystem to a 
    locally-attached device.
    """
    APP_FILES_DIR_TOKEN = "${APP_FILES_DIR}"
    OOB_DATA_DIR_TOKEN = "${APP_OOB_DATA_DIR}"

    def __init__(self, config: Dict):
        super().__init__(config)
        self.src = Path(config["src"])
        self.src_filename = str(self.src.name)
        self.dst = config["dst"]

    def run(self, device_id: str):
        if self.APP_FILES_DIR_TOKEN in self.dst:
            self.copy_to_app_files_dir(
                self.dst.replace(self.APP_FILES_DIR_TOKEN + "/", ""), device_id)

        elif self.OOB_DATA_DIR_TOKEN in self.dst:
            self.copy_to_oob_data_dir(
                self.dst.replace(self.APP_FILES_DIR_TOKEN + "/", ""), device_id)
        else:
            raise CopyTaskError(
                f"Copy task can only copy to {APP_FILES_DIR_TOKEN} or "\
                    f"{OOB_DATA_DIR_TOKEN}")

    def copy_to_app_files_dir(self, dest_subpath, device_id):
        """Copies the src file to the dest location in the app's 
        protected files/ subdir

        The app files dir is protected, so copy requires three steps:
        1: Copy to a temp location
          adb push ./img.jpg /sdcard/img.jpg
        2: Using run-as ensure the dest subpath is available
          adb shell run-as com.google.gamesdk.gamecert.operationrunner "mkdir -p subpath"
        3: Move from temp to dest
          adb shell run-as com.google.gamesdk.gamecert.operationrunner "mv /sdcard/img.jpg files/img.jpg"
        """

        src_file = str(self.src)
        tmp_file = f"/sdcard/{self.src_filename}"
        dst_file = f"files/{dest_subpath}"
        dst_file_parent = str(Path(dst_file).parent)

        push_command = f"adb -s {device_id} push {src_file} {tmp_file}"
        mkdir_command = f"adb -s {device_id} shell run-as {APP_ID} \"mkdir -p {dst_file_parent}\""
        mv_command = f"adb -s {device_id} shell run-as {APP_ID} \"mv {tmp_file} {dst_file}\""

        run_command(push_command)
        run_command(mkdir_command)
        run_command(mv_command)

    def copy_to_oob_data_dir(self, dest_subpath, device_id):
        """Copies the src file to the dest location in the
        app's OOB data dir
        Maps to:
         adb push img.jpg /storage/emulated/0/Android/obb/com.google.gamesdk.gamecert.operationrunner/img.jpg
        """
        src_file = str(self.src)
        dst_file = f"/storage/emulated/0/Android/obb/{APP_ID}/{dest_subpath}"
        dst_path = str(Path(dst_file).parent)
        mkdir_command = f"adb -s {device_id} shell \"mkdir -p {dst_path}\""
        push_command = f"adb -s {device_id} push {src_file} {dst_file}"
        run_command(mkdir_command)
        run_command(push_command)


#-------------------------------------------------------------------------------


def load(preflight: List[Dict]) -> List[Task]:
    """Loads a list of Task to run from a list of task description dictionaries
    Args:
        preflight: list loaded from the recipe's deployment.local.preflight list
        Each entry is a dictionary in form:
        {
            "action": name
            "param0": a param
            "param1": a param
        }

        The action maps to a task class, and the remainder fields are handled
        by the appropriate task subclass

    """
    ctors = {"copy": CopyTask}

    tasks: List[Task] = []
    for pd in preflight:
        action = pd["action"]
        if action in ctors:
            tasks.append(ctors[action](pd))

    return tasks
