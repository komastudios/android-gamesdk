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

class DeviceDirs:
    APP_FILES = "${APP_FILES_DIR}"
    OOB_DATA = "${APP_OOB_DATA_DIR}"
    DEVICE_ROOT = "${DEVICE_ROOT}"

class LocalDirs:
    WORKSPACE = "${WORKSPACE_DIR}"

class Environment:
    """Represents environment information for running pre/postflight tasks
    Fields:
        workspace_dir: Path representing where run.py lives
    """

    def __init__(self):
        self.workspace_dir: Path = Path.cwd()


class Task(object):
    """Base class for tasks"""

    def __init__(self, config: Dict):
        self.action = config["action"]

    def run(self, device_id: str, env: Environment):
        pass

