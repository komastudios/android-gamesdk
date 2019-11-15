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

from pathlib import Path
from typing import List, Tuple


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


# -----------------------------------------------------------------------------------


def ensure_dir(file_path):
    """Ensures the path leading to a file exists
    This helps ensure making a temp file in a deeply nested dir is successful
    Args:
        file_path: The path to the file in question
    """
    directory = os.path.dirname(file_path)
    if not os.path.exists(directory):
        os.makedirs(directory)


def create_output_dir(postfix: str = None):
    """ensure ./out/ exists
    """
    out_dir = "./out"
    if postfix:
        out_dir = "./out/" + postfix

    if not os.path.exists(out_dir):
        os.makedirs(out_dir)

    return Path(out_dir)


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


def upload_to_gcloud(file: Path, bucket: str):
    """Upload a file to a gcloud bucket
    Args:
        file: The file to upload
        bucket: The gcloud bucket to receive the file
    """
    cmdline = ["gsutil", "cp", str(file), bucket + "/"]

    proc = subprocess.run(
        cmdline,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        encoding='utf-8',
    )

    if proc.returncode != 0:
        print(proc.stderr)
        exit(proc.returncode)


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
