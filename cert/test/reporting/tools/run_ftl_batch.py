#
# Copyright 2020 The Android Open Source Project
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
'''Runs a folder of recipes on FTL
'''

import argparse
import concurrent.futures
import datetime
import glob
from pathlib import Path
import subprocess
import sys
import time

SLEEP_PERIOD_SECONDS = 15


def __run_command(command, cmd_id: str, stdout_path: Path, stderr_path: Path):
    with open(stdout_path, "w") as stdout, open(stderr_path, "w") as stderr:
        process = subprocess.Popen(command,
                                   stdout=subprocess.PIPE,
                                   stderr=subprocess.PIPE,
                                   shell=True)
        while True:
            line = process.stdout.readline().decode("utf-8")
            if not line:
                break
            stdout.write(line)
            sys.stdout.write(f"stdout[{cmd_id}]: {line}")

            line = process.stderr.readline().decode("utf-8")
            if line:
                timestamp = str(datetime.datetime.now())
                msg = f"{timestamp} :\t{line}\n"
                stderr.write(msg)
                sys.stderr.write(f"stderr[{cmd_id}] : {msg}")

        process.wait()
        process.stdout.close()


def __batch_runner(recipe: Path, batch_id: str, delay: float, dry_run: bool):
    time.sleep(delay)

    output_file_stdout = recipe.parent / (recipe.stem + ".stdout.txt")
    output_file_stderr = recipe.parent / (recipe.stem + ".stderr.txt")

    cmd = f"python run.py --ftl --recipe {str(recipe)}"
    if dry_run:
        print(f"Would run: \"{cmd}\"")
    else:
        print(f"Submitting recipe {str(recipe)}")
        __run_command(cmd, batch_id, output_file_stdout, output_file_stderr)


def __main():
    parser = argparse.ArgumentParser()

    parser.add_argument(
        "--dry_run",
        help="If true, print run commands but don't submit to FTL",
        required=False,
        action="store_true")

    parser.add_argument("recipes",
                        type=str,
                        nargs=1,
                        metavar="path_to_recipes_dir",
                        help="Path to dir full of recipe files")

    args = parser.parse_args()
    recipe_path = Path(args.recipes[0])
    dry_run = args.dry_run

    recipe_paths = [
        Path(f) for f in sorted(glob.glob(f"{str(recipe_path)}/*.yaml"))
    ]

    if dry_run:
        print(f"DRY RUN: Would send {len(recipe_paths)} recipes up to FTL")
    else:
        print(f"Will send {len(recipe_paths)} recipes up to FTL")

    input("Hit Enter to continue...")

    delay = 0.05 if dry_run else SLEEP_PERIOD_SECONDS
    with concurrent.futures.ThreadPoolExecutor() as executor:
        for i, recipe in enumerate(recipe_paths):
            executor.submit(__batch_runner, recipe, f"batch_{i}", i * delay,
                            dry_run)


if __name__ == "__main__":
    __main()
