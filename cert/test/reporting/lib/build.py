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
"""Provides helper function build_apk() which builds the AndroidCertTest
with optional custom configuration JSON
"""

import os
import shutil

from pathlib import Path

from lib.common import run_command, Error

APP_ID = "com.google.gamesdk.gamecert.operationrunner"


class BuildError(Error):
    """Exception raised when a build fails

    Attributes:
        message -- explanation of the error
    """

    def __init__(self, message):
        super().__init__()
        self.message = message


def _copy_configuration(configuration: Path) -> Path:
    dst = Path("../AndroidCertTest/app/src/main/res/raw/configuration.json")
    backup = Path(
        "../AndroidCertTest/app/src/main/res/raw/configuration_backup.json")
    shutil.copyfile(
        dst,
        Path("../AndroidCertTest/app/src/main/res/raw/configuration_backup.json"
            ))
    shutil.copyfile(configuration, dst)
    return backup


def _restore_configuration(backup: Path):
    dst = Path("../AndroidCertTest/app/src/main/res/raw/configuration.json")
    shutil.copyfile(backup, dst)
    os.remove(backup)


def get_apk_path(build_task: str) -> Path:
    """Get a path to the APK that will be build by build_apk()"""

    artifact_path = "../AndroidCertTest/app/build/outputs/apk/"
    artifact_path += build_task + "/app-" + build_task + ".apk"

    return Path(artifact_path).resolve()


def build_apk(clean: bool,
              build_task: str,
              custom_configuration: Path = None) -> Path:
    """Builds the AndroidCertTest APK

    Args:
        clean: if true, clean and rebuild
        build_task: gradle task, e.g., "assembleDebug" or "assembleOptmizedNative"
        custom_configuration: if provided and exists, is a custom
            configuration.json to build into the ACT app APK

    Returns:
        path to built APK if successful

    Raises:
        BuildError if something goes wrong
    """

    if custom_configuration and not custom_configuration.exists():
        raise BuildError(f"missing configuration file {custom_configuration}")

    # if a custom config is provided, copy it over
    prev_configuration: Path = _copy_configuration(
        custom_configuration) if custom_configuration else None

    cwd = os.getcwd()
    os.chdir("../AndroidCertTest")

    task = ["./gradlew"]

    if clean:
        task.append("clean")

    gradle_build_task = "app:assemble" + build_task[0].capitalize(
    ) + build_task[1:]
    task.append(gradle_build_task)
    task_cmd = " ".join(task)

    artifact_path = get_apk_path(build_task)

    try:
        run_command(task_cmd)
        return artifact_path
    finally:
        # cleanup
        os.chdir(cwd)
        if prev_configuration is not None:
            _restore_configuration(prev_configuration)
