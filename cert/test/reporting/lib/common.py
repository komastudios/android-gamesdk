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
import shlex
import subprocess
import sys

from datetime import datetime
from pathlib import Path
from typing import Any, Dict, List, Tuple

# ------------------------------------------------------------------------------

def nanoseconds_to_seconds(ns):
    return ns / 1e9

def seconds_to_nanoseconds(s):
    return s * 1e9

# ------------------------------------------------------------------------------


class Error(Exception):
    """Base class for exceptions in this module."""
    pass


class NonZeroSubprocessExitCode(Error):
    """Exception raised when a subprocess returns other than 0

    Attributes:
        message -- explanation of the error
    """

    def __init__(self, message):
        self.message = message


# ------------------------------------------------------------------------------


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
    """
    Deletes all files at a directory with some specific extensions.

    Args:
        directory: where at.
        extensions: list of file extensions to remove.
    """
    if (directory.is_dir()):
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
                          encoding='utf-8')

    if proc.returncode != 0:
        print(proc.stderr)
        exit(proc.returncode)

    device_ids = []
    for device in proc.stdout.splitlines()[1:]:
        t = device.split()
        if t is not None and len(t) == 2:
            device_ids.append(t[0])
    return device_ids


def run_command(command):
    """Run a shell command displaying output in real time
    Args:
        command: The shell command to execute
    Raises:
        NonZeroSubprocessExitCode if the shell command exited with non-zero
            result
    """
    print(f"run_command: {command}")
    process = subprocess.Popen(command, stdout=subprocess.PIPE, shell=True)
    while True:
        line = process.stdout.readline().decode("utf-8")
        if not line:
            break
        sys.stdout.write(line)

    rc = process.wait()
    if rc is not 0:
        raise NonZeroSubprocessExitCode(
            f"Command {command} exited with non-zero {rc} return code")


# -----------------------------------------------------------------------------


def get_readable_utc() -> str:
    """
    Returns the current timestamp in a time zone-agnostic, readable format.
    """
    return datetime.utcnow().strftime("%A, %B %d, %Y %H:%M UTC")


def get_indexable_utc() -> str:
    """
    Returns the current timestamp as an alphanumeric index such that
    if "right now" is earlier than "a couple more minutes", then
    get_indexable_utc() at "right now" is order before get_indexable_utc() at
    "a couple more minutes".
    """
    return datetime.utcnow().strftime("%Y%m%d-%H%M%S")


# -----------------------------------------------------------------------------


def dict_lookup(d: Dict, key_path: str, fallback: Any) -> Any:
    """Look up a value (possibly deep) in a dict, safely, with fallback

    Performs deep lookup of a value in a dict, e.g. "foo.bar.baz" would
    return the value d["foo"]["bar"]["baz"] or the fallback if that path
    isn't traversable.

    Args:
        d: The dict to lookup values from
        key_path: A string in format "foo.bar.baz"
        fallback: If not none, the value to return if the dict lookup fails

    Raises:
        MissingPropertyError if fallback is None and the key_path wasn't
            resolvable

    """
    keys = key_path.split(".")
    for i, kp in enumerate(keys):
        if kp in d:
            if i < len(keys) - 1:
                d = d[kp]
            else:
                return d[kp]

    if fallback is None:
        raise MissingPropertyError(f"Key path \"{key_path}\" is required")

    return fallback
