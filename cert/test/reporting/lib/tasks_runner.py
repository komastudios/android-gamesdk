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

from typing import List

from lib.tasks import *
import lib.tasks_preflight
import lib.tasks_postflight

#-------------------------------------------------------------------------------

PREFLIGHT_TASKS = {
    "copy" : lib.tasks_preflight.CopyTask
}

POSTFLIGHT_TASKS = {
    "copy": lib.tasks_postflight.CopyTask,
    "delete": lib.tasks_postflight.DeleteTask
}

#-------------------------------------------------------------------------------

def run(tasks: List[Task], device_id: str, env: Environment):
    """Run a list of preflight tasks with the specified environment
    Args:
        tasks: List of tasks to run
        device_id: The adb device id of a specific device to run tasks on
        env: Environment object
    """
    for task in tasks:
        task.run(device_id, env)


def load(configs: List[Dict], ctors:Dict) -> List[Task]:
    """Loads a list of Task to run from a list of task description dictionaries
    Args:
        configs: list loaded from the recipe's deployment.local.postflight or
        deployment.local.preflight

        Each entry is a dictionary in form:
        {
            "action": name
            "param0": a param
            "param1": a param
        }

        The action maps to a task class, and the remainder fields are handled
        by the appropriate task subclass

    """
    tasks: List[Task] = []
    for pd in configs:
        action = pd["action"]
        if action in ctors:
            tasks.append(ctors[action](pd))

    return tasks
