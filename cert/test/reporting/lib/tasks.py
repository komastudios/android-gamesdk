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
"""Base classes and enums used for pre- and post-flight task sets
"""

from abc import ABC, abstractmethod
from pathlib import Path
from typing import Dict

class DeviceDirs:
    #pylint: disable=too-few-public-methods
    """Collections of special file location tokens on device
    Values:
        APP_FILES: The app-private files dir
        OOB_DATA: The app's OOB data dir
        DEVICE_ROOT: The root folder of a device
    """
    APP_FILES = "${APP_FILES_DIR}"
    OOB_DATA = "${APP_OOB_DATA_DIR}"
    DEVICE_ROOT = "${DEVICE_ROOT}"

class LocalDirs:
    #pylint: disable=too-few-public-methods
    """Collection of special file location tokens on the computer
    executing the script
    Values:
        WORKSPACE: The location of run.py
    """
    WORKSPACE = "${WORKSPACE_DIR}"

class Environment:
    #pylint: disable=too-few-public-methods
    """Represents environment information for running pre/postflight tasks
    Fields:
        workspace_dir: Path representing where run.py lives
    """

    def __init__(self):
        self.workspace_dir: Path = Path.cwd()


class Task(ABC):
    #pylint: disable=too-few-public-methods
    """Base class for tasks"""

    def __init__(self, config: Dict):
        self.action = config["action"]

    @abstractmethod
    def run(self, device_id: str, env: Environment):
        """Each Task implementation must implement the run() method
        to perform its work.
        Arguments:
            device_id: The ADB device id of the target device
            env: the current operating environment for sourcing paths
        """
        raise NotImplementedError("Task subclasses must implement run()")
