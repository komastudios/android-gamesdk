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
from lib.tasks import *

class CopyTaskError(Error):
    """Exception raised when CopyTask fails

    Attributes:
        message -- explanation of the error
    """

    def __init__(self, message):
        self.message = message


class DeleteTaskError(Error):
    """Exception raised when DeleteTask fails

    Attributes:
        message -- explanation of the error
    """

    def __init__(self, message):
        self.message = message


#-------------------------------------------------------------------------------



class DeleteTask(Task):
    ENABLED = False

    def __init__(self, config: Dict):
        super().__init__(config)
        self.files = config["files"]

    def run(self, device_id: str, env: Environment):
        for file in self.files:
            if LocalDirs.WORKSPACE in file:
                self.delete_local_file(file, device_id, env)
            elif DeviceDirs.APP_FILES in file:
                self.delete_device_app_files_file(file, device_id, env)
            elif DeviceDirs.OOB_DATA in file:
                self.delete_oob_file(file, device_id, env)
            elif DeviceDirs.DEVICE_ROOT in file:
                self.delete_device_file(file, device_id, env)
            elif Path(file).expanduser().resolve().exists():
                self.delete_local_file(file, device_id, env)
            else:
                raise DeleteTask(f"Can't delete unknown file \"{file}\"")

    def delete_local_file(self, file:str, device_id:str, env:Environment):
        if LocalDirs.WORKSPACE in file:
            file = file.replace(LocalDirs.WORKSPACE, str(env.workspace_dir))

        file = Path(file).expanduser().resolve()
        print(f"[DeleteTask] - Deleting {str(file)}")
        if self.ENABLED and file.exists():
            if file.is_dir():
                file.rmdir()
            else:
                file.unlink()

    def delete_device_app_files_file(self, file: str, device_id: str,
                                     env: Environment):
        sub_path = file.replace(DeviceDirs.APP_FILES + "/", "")
        device_path = f"files/{sub_path}"
        delete_command = f"adb -s {device_id} shell run-as {APP_ID} \"rm -r {device_path}\""
        try:
            run_command(delete_command)
        except NonZeroSubprocessExitCode as e:
            print(f"Unable to delete file {device_path} from device {device_id} error: {e.message}")


    def delete_oob_file(self, file: str, device_id: str, env: Environment):
        sub_path = file.replace(DeviceDirs.OOB_DATA + "/", "")
        device_path = f"/storage/emulated/0/Android/obb/{APP_ID}/{sub_path}"
        delete_command = f"adb -s {device_id} shell \"rm -r '{device_path}'\""
        try:
            run_command(delete_command)
        except NonZeroSubprocessExitCode as e:
            print(
                f"Unable to delete file {device_path} from device {device_id} error: {e.message}"
            )

    def delete_device_file(self, file: str, device_id: str, env: Environment):
        sub_path = file.replace(DeviceDirs.DEVICE_ROOT, "")
        device_path = f"{sub_path}"
        delete_command = f"adb -s {device_id} shell \"rm -r '{device_path}'\""
        try:
            run_command(delete_command)
        except NonZeroSubprocessExitCode as e:
            print(
                f"Unable to delete file {device_path} from device {device_id} error: {e.message}"
            )


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

    def copy_from_app_files_dir(self, device_id:str, src:str, dst:Path):
        src = "files/" + src.replace(DeviceDirs.APP_FILES + "/", "")
        copy_cmd = f"adb -s {device_id} shell \"run-as {APP_ID} cat '{src}'\" > \"{str(dst)}\""
        try:
            run_command(copy_cmd)
        except NonZeroSubprocessExitCode as e:
            print(f"Error copying {src} from device; error: {e.message}")

    def copy_from_app_oob_dir(self, device_id: str, src: str, dst: Path):
        src = src.replace(DeviceDirs.OOB_DATA + "/", "")
        src = f"/storage/emulated/0/Android/obb/{APP_ID}/{src}"
        copy_cmd = f"adb -s {device_id} shell \"cat '{src}'\" > \"{str(dst)}\""

        try:
            run_command(copy_cmd)
        except NonZeroSubprocessExitCode as e:
            print(f"Error copying {src} from device; error: {e.message}")

    def copy_from_other_dir(self, device_id: str, src: str, dst: Path):
        copy_cmd = f"adb -s {device_id} shell \"cat '{src}'\" > \"{str(dst)}\""

        try:
            run_command(copy_cmd)
        except NonZeroSubprocessExitCode as e:
            print(f"Error copying {src} from device; error: {e.message}")

    def resolve_dest_path(self, dst:str, env:Environment)->Path:
        if LocalDirs.WORKSPACE in dst:
            dst = dst.replace(LocalDirs.WORKSPACE, str(env.workspace_dir))

        dst = Path(dst).expanduser().resolve()
        ensure_dir(str(dst))
        return dst