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
"""Provides post-flight task implementations:
    CopyTask: Copies files to local computer from a device after tests are run
    DeleteTask: Deletes local files or files on device after tests are run
"""
from pathlib import Path
from typing import Dict

from lib.build import APP_ID
from lib.common import run_command, NonZeroSubprocessExitCode, ensure_dir
from lib.tasks import Task, Environment, LocalDirs, DeviceDirs


class CopyTaskError(Exception):
    """Exception raised when CopyTask fails.
    """


class DeleteTaskError(Exception):
    """Exception raised when DeleteTask fails.
    """


#-------------------------------------------------------------------------------


class DeleteTask(Task):
    """DeleteTask

    A Task implementation which deletes files locally, and on devices
    attached via USB
    """
    ENABLED = True

    def __init__(self, config: Dict):
        super().__init__(config)
        self.files = config["files"]

    def run(self, device_id: str, env: Environment):
        for file in self.files:
            if LocalDirs.WORKSPACE in file:
                self.delete_local_file(file, env)
            elif DeviceDirs.APP_FILES in file:
                self.delete_device_app_files_file(file, device_id, env)
            elif DeviceDirs.OOB_DATA in file:
                self.delete_oob_file(file, device_id, env)
            elif DeviceDirs.DEVICE_ROOT in file:
                self.delete_device_file(file, device_id, env)
            elif Path(file).expanduser().resolve().exists():
                self.delete_local_file(file, env)
            else:
                raise DeleteTaskError(f"Can't delete unknown file \"{file}\"")

    def delete_local_file(self, file: str, env: Environment):
        """Deletes a local file (on the computer executing script)
        Args:
            file: The local file to delete
            device_id: Unused
            env: The operating environment info
        """
        if LocalDirs.WORKSPACE in file:
            file = file.replace(LocalDirs.WORKSPACE, str(env.workspace_dir))

        file = Path(file).expanduser().resolve()
        print(f"[DeleteTask] - Deleting {file}")
        if self.ENABLED and file.exists():
            if file.is_dir():
                file.rmdir()
            else:
                file.unlink()

    def delete_device_app_files_file(self, file: str, device_id: str,
                                     env: Environment):
        """Deletes a file from the app's private file storage
        Args:
            file: The subpath relative to app-local storage of a file to delete
            device_id: The adb device id of the target device
            env: The operating environment
        """
        sub_path = file.replace(DeviceDirs.APP_FILES + "/", "")
        device_path = f"files/{sub_path}"
        delete_command = \
            f"adb -s {device_id} shell run-as {APP_ID} \"rm -r {device_path}\""
        try:
            run_command(delete_command)
        except NonZeroSubprocessExitCode as ex:
            raise DeleteTaskError(f"Unable to delete file {device_path} from "\
                f"device {device_id} error: {repr(ex)}")

    def delete_oob_file(self, file: str, device_id: str, env: Environment):
        """Deletes a file from the app's OOB file storage
        Args:
            file: The subpath relative to app OOB storage of a file to delete
            device_id: The adb device id of the target device
            env: The operating environment
        """
        sub_path = file.replace(DeviceDirs.OOB_DATA + "/", "")
        device_path = f"/storage/emulated/0/Android/obb/{APP_ID}/{sub_path}"
        delete_command = f"adb -s {device_id} shell \"rm -r '{device_path}'\""
        try:
            run_command(delete_command)
        except NonZeroSubprocessExitCode as ex:
            raise DeleteTaskError(f"Unable to delete file {device_path} from "\
                f"device {device_id} error: {repr(ex)}")

    def delete_device_file(self, file: str, device_id: str, env: Environment):
        """Deletes a file from the device's public file storage, e.g., /sdcard
        Args:
            file: The subpath relative to device root of a file to delete
            device_id: The adb device id of the target device
            env: The operating environment
        """
        sub_path = file.replace(DeviceDirs.DEVICE_ROOT, "")
        device_path = f"{sub_path}"
        delete_command = f"adb -s {device_id} shell \"rm -r '{device_path}'\""
        try:
            run_command(delete_command)
        except NonZeroSubprocessExitCode as ex:
            raise DeleteTaskError(f"Unable to delete file {device_path} from "\
                f"device {device_id} error: {repr(ex)}")


class CopyTask(Task):
    """Represents a copy action copying files from an attached
    device to the local filesystem
    Tokens:
        ${APP_FILES_DIR} refers to the app's files directory on device
        ${APP_OOB_DATA_DIR} refers to the OOB file location on device
        ${WORKSPACE_DIR} refers to the location of run.py
    """

    def __init__(self, config: Dict):
        super().__init__(config)
        self.src = config["src"]
        self.dst = config["dst"]

    def run(self, device_id: str, env: Environment):
        self.dst = self.resolve_dest_path(self.dst, env)

        if DeviceDirs.APP_FILES in self.src:
            self.copy_from_app_files_dir(device_id, self.src, self.dst)
        elif DeviceDirs.OOB_DATA in self.src:
            self.copy_from_app_oob_dir(device_id, self.src, self.dst)
        else:
            self.copy_from_other_dir(device_id, self.src, self.dst)

    def copy_from_app_files_dir(self, device_id: str, src: str, dst: Path):
        """Copies a file from the app's private storage location
        Args:
            device_id: The ADB device id of the target device
            src: The subpath relative to app-local file storage of the file
                to copy
            dst: The location on the local filesystem to copy the file to
        """
        src = "files/" + src.replace(DeviceDirs.APP_FILES + "/", "")
        copy_cmd = f"adb -s {device_id} shell \"run-as {APP_ID}" \
            + f" cat '{src}'\" > \"{dst}\""

        try:
            run_command(copy_cmd)
        except NonZeroSubprocessExitCode as ex:
            raise CopyTaskError(
                f"Error copying {src} from device; error: {repr(ex)}")

    def copy_from_app_oob_dir(self, device_id: str, src: str, dst: Path):
        """Copies a file from the app's OOB storage location
        Args:
            device_id: The ADB device id of the target device
            src: The subpath relative to the app's OOB file storage of the
                file to copy
            dst: The location on the local filesystem to copy the file to
        """
        src = src.replace(DeviceDirs.OOB_DATA + "/", "")
        src = f"/storage/emulated/0/Android/obb/{APP_ID}/{src}"
        copy_cmd = f"adb -s {device_id} shell \"cat '{src}'\" > \"{dst}\""

        try:
            run_command(copy_cmd)
        except NonZeroSubprocessExitCode as ex:
            raise CopyTaskError(
                f"Error copying {src} from device; error: {repr(ex)}")

    def copy_from_other_dir(self, device_id: str, src: str, dst: Path):
        """Copies a file from an arbitrary location on device to local storage
        Args:
            device_id: The ADB device id of the target device
            src: device path relative to root, e.g., /sdcard/foo.txt
            dst: The location on the local filesystem to copy the file to
        """
        copy_cmd = f"adb -s {device_id} shell \"cat '{src}'\" > \"{dst}\""

        try:
            run_command(copy_cmd)
        except NonZeroSubprocessExitCode as ex:
            raise CopyTaskError(
                f"Error copying {src} from device; error: {repr(ex)}")

    def resolve_dest_path(self, dst: str, env: Environment) -> Path:
        """Replace placeholder tokens in path with real values from Environment
        Args:
            dst: The destination path with possible tokens in it such as
                ${WORKSPACE_DIR}
            env: The operating environment
        Returns:
            the proposed destination path with tokens replaced for
            real subpaths, converted to a Path() object
        """
        if LocalDirs.WORKSPACE in dst:
            dst = dst.replace(LocalDirs.WORKSPACE, str(env.workspace_dir))

        dst = Path(dst).expanduser().resolve()
        ensure_dir(dst)
        return dst
