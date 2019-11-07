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

import os
import shutil
import time

from pathlib import Path

from lib.common import run_command, Error

APP_ID = "com.google.gamesdk.gamecert.operationrunner"


class BuildError(Error):
    """Exception raised when a build fails

    Attributes:
        message -- explanation of the error
    """

    def __init__(self, message):
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


def build_apk(clean: bool, custom_configuration: Path = None) -> Path:
    """Builds the AndroidCertTest APK

    Args:
        clean: if true, clean and rebuild
        custom_configuration: if provided and exists, is a custom 
            configuration.json to build into the ACT app APK

    Returns: 
        path to built APK if successful

    Raises:
        BuildError if something goes wrong
    """

    if custom_configuration and not custom_configuration.exists():
        raise BuildError(
            f"specified custom configuration file {custom_configuration} missing"
        )

    # if a custom config is provided, copy it over
    prev_configuration: Path = _copy_configuration(
        custom_configuration) if custom_configuration else None

    cwd = os.getcwd()
    os.chdir("../AndroidCertTest")

    try:
        if clean:
            run_command("./gradlew clean assembleDebug")
        else:
            run_command("./gradlew assembleDebug")

        return Path(
            "../AndroidCertTest/app/build/outputs/apk/debug/app-debug.apk"
        ).resolve()
    finally:
        # cleanup
        os.chdir(cwd)
        if prev_configuration is not None:
            _restore_configuration(prev_configuration)
