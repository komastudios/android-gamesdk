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

from contextlib import contextmanager
import os
import shutil
import time
from pathlib import Path

from lib.common import run_command, ensure_dir

APP_ID = "com.google.gamesdk.gamecert.operationrunner"


class BuildError(Exception):
    """Exception raised when a build fails.
    """


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


def get_apk_path(build_type: str) -> Path:
    """Get a path to the APK that will be build by build_apk()"""

    artifact_path = "../AndroidCertTest/app/build/outputs/apk/"
    artifact_path += build_type + "/app-" + build_type + ".apk"

    return Path(artifact_path).resolve()


def _wait_for_sentinel(sentinel_file: Path):
    tick = 0
    while sentinel_file.exists():
        count = tick % 4
        ellipsis = "." * count
        ellipsis += " " * (4 - count)
        print(f"Waiting for existing build to finish{ellipsis}", end='\r')
        tick += 1
        time.sleep(0.25)

#-------------------------------------------------------------------------------


@contextmanager
def _managed_build(configuration: Path):

    # the sentinel file marks that *somebody* is building the
    # project; first, check if it exists and wait for it to
    # go away if it does
    sentinel = Path("./tmp/build_active")
    _wait_for_sentinel(sentinel)

    # now, create the sentinal to mark *our* ownership
    ensure_dir(sentinel)
    sentinel.touch()

    # if a custom config is provided, copy it over
    prev_configuration: Path = _copy_configuration(
        configuration) if configuration else None

    cwd = os.getcwd()
    os.chdir("../AndroidCertTest")

    try:
        yield

    finally:
        # putting the cleanup in finally handles ctrl-c and other
        # unexpected termination events
        os.chdir(cwd)
        if prev_configuration is not None:
            _restore_configuration(prev_configuration)

        sentinel.unlink()

#-------------------------------------------------------------------------------


def build_apk(clean: bool,
              build_type: str,
              custom_configuration: Path = None) -> Path:
    """Builds the AndroidCertTest APK

    Args:
        clean: if true, clean and rebuild
        build_type: build type defined in app/build.gradle e.g.,
            "debug" or "optimizedNative"
        custom_configuration: if provided and exists, is a custom
            configuration.json to build into the ACT app APK

    Returns:
        path to built APK if successful

    Raises:
        BuildError if something goes wrong
    """

    if custom_configuration and not custom_configuration.exists():
        raise BuildError(f"missing configuration file {custom_configuration}")

    with _managed_build(custom_configuration):
        task = ["./gradlew"]

        if clean:
            task.append("clean")

        gradle_build_task = "app:assemble" + build_type[0].capitalize(
        ) + build_type[1:]
        task.append(gradle_build_task)
        task_cmd = " ".join(task)

        artifact_path = get_apk_path(build_type)

        run_command(task_cmd, display_output=True)
        return artifact_path
