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

    def __init__(self, config: Dict):
        self.action = config["action"]

    def run(self, device_id: str):
        pass


class CopyTask(Task):
    # _internal_data_path: /data/user/0/com.google.gamesdk.gamecert.operationrunner/files
    # to access this we need to run-as com.google.gamesdk.gamecert.operationrunner and then push to ./files

    # _raw_data_path: /data/app/com.google.gamesdk.gamecert.operationrunner-rxbr2eneXm2Z_JpZORYFlw==/base.apk
    # don't think we can writ eto this

    # _obb_path: /storage/emulated/0/Android/obb/com.google.gamesdk.gamecert.operationrunner
    # looks like we can read/write this just fine

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
        # three steps:
        # 1: Copy to a temp location
        #   adb push ./img.jpg /sdcard/img.jpg
        # 2: Using run-as ensure the dest subpath is available
        #   adb shell run-as com.google.gamesdk.gamecert.operationrunner "mkdir -p subpath"
        # 3: Move from temp to dest
        #   adb shell run-as com.google.gamesdk.gamecert.operationrunner "mv /sdcard/img.jpg files/img.jpg"

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
        # no run-as dance:
        #  adb push img.jpg /storage/emulated/0/Android/obb/com.google.gamesdk.gamecert.operationrunner/img.jpg
        src_file = str(self.src)
        dst_file = f"/storage/emulated/0/Android/obb/{APP_ID}/{dest_subpath}"
        dst_path = str(Path(dst_file).parent)
        mkdir_command = f"adb -s {device_id} shell \"mkdir -p {dst_path}\""
        push_command = f"adb -s {device_id} push {src_file} {dst_file}"
        run_command(mkdir_command)
        run_command(push_command)


#-------------------------------------------------------------------------------


def load(preflight: List[Dict]) -> List[Task]:
    ctors = {"copy": CopyTask}

    tasks: List[Task] = []
    for pd in preflight:
        action = pd["action"]
        if action in ctors:
            tasks.append(ctors[action](pd))

    return tasks
