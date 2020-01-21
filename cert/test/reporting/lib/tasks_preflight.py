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
"""Provides pre-flight task implementations:
    CopyTask: Copies files from the local computer to device before tests
"""

from pathlib import Path
from typing import Dict

from lib.build import APP_ID
from lib.common import run_command
from lib.tasks import Task, Environment, LocalDirs, DeviceDirs

#-------------------------------------------------------------------------------

class CopyTaskError(Exception):
    """Exception raised when CopyTask can't do it's thing.
    """


#-------------------------------------------------------------------------------

class CopyTask(Task):
    """Represents a copy action copying files from local filesystem to a
    locally-attached device.
    Tokens:
        ${APP_FILES_DIR} refers to the app's files directory on device
        ${APP_OOB_DATA_DIR} refers to the OOB file location on device
        ${WORKSPACE_DIR} refers to the location of run.py
    """

    def __init__(self, config: Dict):
        super().__init__(config)
        self.src = config["src"]
        self.dst = config["dst"]
        self.src_filename = None

    def run(self, device_id: str, env: Environment):
        self.src = self.resolve_src_path(env)

        if not self.src.exists():
            raise CopyTaskError(f"Missing src file {self.src}")

        self.src_filename = str(self.src.name)

        if DeviceDirs.APP_FILES in self.dst:
            self.copy_to_app_files_dir(
                self.dst.replace(DeviceDirs.APP_FILES + "/", ""), device_id)

        elif DeviceDirs.OOB_DATA in self.dst:
            self.copy_to_oob_data_dir(
                self.dst.replace(DeviceDirs.OOB_DATA + "/", ""), device_id)

        elif DeviceDirs.DEVICE_ROOT in self.dst:
            self.copy_to_device_dir(
                self.dst.replace(DeviceDirs.DEVICE_ROOT + "/", ""), device_id)

        else:
            raise CopyTaskError(
                f"Copy task can only copy to {DeviceDirs.APP_FILES} or "\
                    f"{DeviceDirs.OOB_DATA}")

    def resolve_src_path(self, env: Environment) -> Path:
        """Replace placeholder tokens in self.src with real values
        from Environment
        Args:
            env: The operating environment
        Returns:
            self.src as a path with tokens replaced for real subpaths,
            converted to a Path() object
        """
        src = self.src
        if LocalDirs.WORKSPACE in src:
            src = Path(
                src.replace(LocalDirs.WORKSPACE, str(env.workspace_dir)))

        return Path(src).expanduser().resolve()

    def copy_to_app_files_dir(self, dest_subpath, device_id):
        """Copies the src file to the dest location in the app's
        protected files/ subdir

        The app files dir is protected, so copy requires three steps:
        1: Copy to a temp location
          adb push ./img.jpg /sdcard/img.jpg
        2: Using run-as ensure the dest subpath is available
          adb shell run-as com.google.gamesdk.gamecert.operationrunner \
            "mkdir -p subpath"
        3: Move from temp to dest
          adb shell run-as com.google.gamesdk.gamecert.operationrunner \
            "mv /sdcard/img.jpg files/img.jpg"
        """

        src_file = str(self.src)
        tmp_file = f"/sdcard/{self.src_filename}"
        dst_file = f"files/{dest_subpath}"
        dst_file_parent = str(Path(dst_file).parent)

        push_command = f"adb -s {device_id} push {src_file} {tmp_file}"

        mkdir_command = f"adb -s {device_id} shell run-as {APP_ID}" \
            + f" \"mkdir -p {dst_file_parent}\""

        mv_command = f"adb -s {device_id} shell run-as {APP_ID}" \
            + f" \"mv {tmp_file} {dst_file}\""

        run_command(push_command)
        run_command(mkdir_command)
        run_command(mv_command)

    def copy_to_oob_data_dir(self, dest_subpath, device_id):
        """Copies the src file to the dest location in the
        app's OOB data dir
        Maps to:
         adb push img.jpg /storage/emulated/0/Android/obb/APP_ID/img.jpg
        """
        src_file = str(self.src)
        dst_file = f"/storage/emulated/0/Android/obb/{APP_ID}/{dest_subpath}"
        dst_path = str(Path(dst_file).parent)
        mkdir_command = f"adb -s {device_id} shell \"mkdir -p {dst_path}\""
        push_command = f"adb -s {device_id} push {src_file} {dst_file}"
        run_command(mkdir_command)
        run_command(push_command)

    def copy_to_device_dir(self, dest_subpath, device_id):
        """Copies the src file to the dest location in the filsystem
        Maps to:
         adb push img.jpg /path/to/img.jpg
        """
        src_file = str(self.src)
        dst_file = f"/{dest_subpath}"
        dst_path = str(Path(dst_file).parent)
        mkdir_command = f"adb -s {device_id} shell \"mkdir -p {dst_path}\""
        push_command = f"adb -s {device_id} push {src_file} {dst_file}"
        run_command(mkdir_command)
        run_command(push_command)
