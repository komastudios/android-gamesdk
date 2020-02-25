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
"""Common helper functions and classes"""

import os
import subprocess
import sys
from datetime import datetime
from pathlib import Path
from typing import Any, Dict, List
import yaml

# ------------------------------------------------------------------------------


def nanoseconds_to_seconds(nano_seconds):
    """Convert nano seconds to seconds"""
    return nano_seconds / 1e9


def seconds_to_nanoseconds(seconds):
    """Convert seconds to nanoseconds"""
    return seconds * 1e9


def nanoseconds_to_milliseconds(nano_seconds):
    """Convert nano seconds to milliseconds"""
    return nano_seconds / 1e6


def milliseconds_to_nanoseconds(milliseconds):
    """Convert milliseconds to nanoseconds"""
    return milliseconds * 1e6


# ------------------------------------------------------------------------------


class NonZeroSubprocessExitCode(Exception):
    """Exception raised when a subprocess returns other than 0.
    """


class MissingPropertyError(Exception):
    """Exception raised when a required property in the recipe is missing.
    """


# ------------------------------------------------------------------------------


def dict_lookup(src: Dict, key_path: str, fallback: Any) -> Any:
    """Look up a value (possibly deep) in a dict, safely, with fallback

    Performs deep lookup of a value in a dict, e.g. "foo.bar.baz" would
    return the value src["foo"]["bar"]["baz"] or the fallback if that path
    isn't traversable.

    Args:
        dict: The dict to lookup values from
        key_path: A string in format "foo.bar.baz"
        fallback: If not none, the value to return if the dict lookup fails

    Raises:
        MissingPropertyError if fallback is None and the key_path wasn't
            resolvable

    """
    keys = key_path.split(".")
    for i, sub_key_path in enumerate(keys):
        if sub_key_path in src:
            if i < len(keys) - 1:
                src = src[sub_key_path]
            else:
                return src[sub_key_path]
        else:
            break

    if fallback is None:
        raise MissingPropertyError(f"Key path \"{key_path}\" is required")

    return fallback() if callable(fallback) else fallback


def ensure_dir(file: Path):
    """Ensures the path leading to a file exists
    This helps ensure making a temp file in a deeply nested dir is successful
    Args:
        file_path: The path to the file in question
    """
    file_dir = file.parent
    if not os.path.exists(file_dir):
        os.makedirs(file_dir)


def create_output_dir(postfix: str = None):
    """ensure ./out/ exists
    """
    out_dir = "./out"
    if postfix:
        out_dir = "./out/" + postfix

    if not os.path.exists(out_dir):
        os.makedirs(out_dir)

    return Path(out_dir)


def remove_files_with_extensions(directory: Path, extensions: List[str]):
    """Deletes all files at a directory with some specific extensions.

    Args:
        directory: where at.
        extensions: list of file extensions to remove.
    """
    if directory.is_dir():
        for file_name in os.listdir(directory):
            if Path(file_name).suffix in extensions:
                os.unlink(directory.joinpath(file_name))


# -----------------------------------------------------------------------------


def get_attached_devices() -> List[str]:
    """Get a list of device ids of attached android devices
    Returns:
        List of device ids of attached android devices
    """
    cmdline = ['adb', 'devices']

    proc = subprocess.run(cmdline,
                          stdout=subprocess.PIPE,
                          stderr=subprocess.PIPE,
                          check=False,
                          encoding='utf-8')

    if proc.returncode != 0:
        print(proc.stderr)
        sys.exit(proc.returncode)

    device_ids = []
    for device in proc.stdout.splitlines()[1:]:
        tokens = device.split()
        if tokens is not None and len(tokens) == 2:
            device_ids.append(tokens[0])
    return device_ids


RET_CODE_SUCCESS = 0


def run_command(command, display_output: bool = False):
    """Run a shell command displaying output in real time
    Args:
        command: The shell command to execute
        display_output: If true, the executed command and its stdout will
            be displayed in stdout
    Raises:
        NonZeroSubprocessExitCode if the shell command exited with non-zero
            result
    """
    if display_output:
        print(f"run_command: {command}")

    process = subprocess.Popen(command, stdout=subprocess.PIPE, shell=True)
    while True:
        line = process.stdout.readline().decode("utf-8")
        if not line:
            break
        if display_output:
            sys.stdout.write(line)

    ret_code = process.wait()
    process.stdout.close()

    if ret_code is not RET_CODE_SUCCESS:
        raise NonZeroSubprocessExitCode(
            f"Command {command} exited with non-zero {ret_code} return code")


# -----------------------------------------------------------------------------


def get_readable_utc() -> str:
    """Returns the current timestamp in a time zone-agnostic, readable format.
    """
    return datetime.utcnow().strftime("%A, %B %d, %Y %H:%M UTC")


def get_indexable_utc() -> str:
    """Returns the current timestamp as an alphanumeric index such that if
    "right now" is earlier than "a couple more minutes", then
    get_indexable_utc() at "right now" is order before get_indexable_utc() at
    "a couple more minutes".
    """
    return datetime.utcnow().strftime("%Y%m%d-%H%M%S")


# -----------------------------------------------------------------------------


class Recipe():
    """Represents a union of two recipe files, the primary and the fallback
    """

    def __init__(self, recipe: Path, default_recipe: Path):
        self._recipe_path = recipe
        self._default_recipe_path = default_recipe
        self._name = recipe.stem

        with open(recipe) as recipe_file:
            self._recipe = yaml.load(recipe_file, Loader=yaml.FullLoader)

        with open(default_recipe) as recipe_file:
            self._default_recipe = yaml.load(recipe_file,
                                             Loader=yaml.FullLoader)

    def get_recipe_path(self) -> Path:
        """Get the path to the primary recipe"""
        return self._recipe_path

    def get_default_recipe_path(self) -> Path:
        """Get the path to the default/fallback recipe"""
        return self._default_recipe_path

    def get_recipe(self) -> Dict:
        """Get the primary recipe"""
        return self._recipe

    def get_default_recipe(self) -> Dict:
        """Get the default/fallback recipe"""
        return self._recipe

    def get_name(self) -> str:
        """Returns the recipe file name (without extension)."""
        return self._name

    def lookup(self, key_path: str, fallback: Any = None) -> Any:
        """Looks up a value in a the recipe file, using the value from
        default_recipe as a fallback if not present in src
        Args:
            key_path: A path to lookup, e.g.,
                "deployment.local.all_attached_devices"
        """

        def fallback_default():
            return dict_lookup(self._default_recipe,
                               key_path,
                               fallback=fallback)

        return dict_lookup(self._recipe, key_path, fallback=fallback_default)
