#!/usr/bin/env python3

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

import argparse
import shutil
import time
import yaml

from typing import Dict, List, Any
from pathlib import Path

from lib.build import build_apk, APP_ID
from lib.common import *
from lib.report import extract_and_export, convert_json_report_to_csv, merge_systrace_into_csv
from lib.devicefarm import run_on_farm_and_collect_reports
import lib.charting


class Error(Exception):
    """Base class for exceptions in this module."""
    pass


class MissingPropertyError(Error):
    """Exception raised when a required property in the recipe is missing

    Attributes:
        message -- explanation of the error
    """

    def __init__(self, message):
        self.message = message


# ------------------------------------------------------------------------------


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


def poll_app_active(device_id: str, app_id: str):
    """Check if the given app is running and frontmost on the activity stack

    Args:
        device_id: The USB device to query
        app_id: The id of the app in question

    Returns:
        True iff the app is running and at top of activity stack
    """

    cmdline = [
        "adb", "-s", device_id, "shell", "dumpsys", "activity", "activities"
    ]

    # * TaskRecord{e5f38d5d0 #12 I=com.sec.android.app.launcher/com.android.launcher3.Launcher U=0 StackId=0 sz=1}
    # if the app id in that line is ours, we are running and frontmost. If
    # we're not in that line we probably called finish() and are done

    output: str = subprocess.check_output(
        cmdline, stderr=subprocess.PIPE).decode('utf-8')

    for line in output.splitlines():
        line = line.strip()
        if line.find("* TaskRecord") == 0:
            if line.find(app_id) >= 0:
                return True
            return False

    return False


# ------------------------------------------------------------------------------


class LocalSystrace(object):

    def __init__(self, device_id: str, dst_file: Path, categories: List[str]):
        """Start systrace on a device, blocks until systrace is ready"""
        self._trace_file = dst_file

        cmd = [
            "python2",
            os.path.expandvars(
                "$ANDROID_HOME/platform-tools/systrace/systrace.py"),
            "--serial",
            device_id,
            "-a",
            APP_ID,
            "-o",
            str(dst_file),
        ] + categories

        self.process = subprocess.Popen(cmd,
                                        stdout=subprocess.PIPE,
                                        stdin=subprocess.PIPE,
                                        shell=False)

        # TODO(shamyl@gmail.com): Find a way to capture "Starting
        # tracing (stop with enter)"
        # attempting to run the loop below blocks on the first call to
        # readline()
        # while True:
        #     line = self.process.stdout.readline().decode("utf-8")
        #     if "stop with enter" in line:
        #         break

        time.sleep(4)

    def finish(self):
        # systrace ends when it receives an "enter" input
        print("LocalSystrace::finish - Stopping systrace.py...")
        self.process.stdin.write(b"\n")
        self.process.stdin.close()
        self.process.wait()
        print("LocalSystrace::finish - done")
        return self._trace_file


def unpack(s):
    """Convenience string join function"""
    return " ".join(map(str, s))


def run_local_deployment(recipe: Dict, apk: Path, tmp_dir: Path):
    if not dict_lookup(recipe, "deployment.local.all_attached_devices", False):
        devices = dict_lookup(recipe, "deployment.local.device_ids", None)
        devices = list(set(devices) & set(get_attached_devices()))
    else:
        devices = get_attached_devices()

    collect_systrace = dict_lookup(recipe, "systrace.enabled", fallback=False)
    systrace_keywords = dict_lookup(recipe, "systrace.keywords", fallback=[])
    systrace_categories = dict_lookup(recipe,
                                      "systrace.categories",
                                      fallback="")
    render_chart = dict_lookup(recipe, "chart.enabled", fallback=False)
    render_chart_suites = dict_lookup(recipe, "chart.suites", [])

    fields = dict_lookup(recipe, "chart.fields", [])
    skipping_fields = dict_lookup(recipe, "chart.skipping", [])

    if systrace_categories:
        systrace_categories = systrace_categories.split(" ")

    print("Will run local deployment on the following ADB device IDs:" +
          unpack(devices))
    print(f"\tSystrace: {collect_systrace} systrace_keywords: " +
          unpack(systrace_keywords) + "\n\n")

    for device_id in devices:

        # install the APK
        command = f"adb -s {device_id} install -r {str(apk)}"
        run_command(command)

        # Start systrace - this blocks until systrace is ready to run
        if collect_systrace:
            systracer = LocalSystrace(device_id=device_id,
                                      dst_file=tmp_dir.joinpath("tracey.html"),
                                      categories=systrace_categories)

        # launch the activity in GAME_LOOP mode
        command = (f"adb -s {device_id} shell am start" +
                   f" -n \"{APP_ID}/.MainActivity\"" +
                   f" -a \"com.google.intent.action.TEST_LOOP\"")

        run_command(command)

        # wait for the GAME_LOOP to complete
        tick = 0
        while poll_app_active(device_id, APP_ID):
            c = tick % 4
            ellipsis = "." * c
            ellipsis += " " * (4 - c)
            print(f"Waiting on app to finish tests{ellipsis}", end='\r')
            tick += 1
            time.sleep(0.25)

        print("\n")
        time.sleep(2)

        trace_file = None
        if collect_systrace:
            trace_file = systracer.finish()

        # extract report (for now we're skipping systrace operations)
        log_file, csv_file, sys_file = extract_and_export(
            device_id,
            dst_dir=tmp_dir,
            systrace_file=trace_file,
            systrace_keywords=systrace_keywords,
            bucket=None)

        if render_chart:
            suites = lib.charting.load_suites(csv_file.resolve())
            for suite in suites:
                if (suite.suite_name in render_chart_suites or
                        not render_chart_suites):
                    title = suite.suite_name
                    suite.plot(title, fields, skipping_fields)


def run_ftl_deployment(recipe: Dict, apk: Path, tmp_dir: Path):
    # first step is to save out an args.yaml file to pass on to gsutil API
    args_dict = dict_lookup(recipe, "deployment.ftl.args", fallback=None)
    for test in args_dict:
        args_dict[test]["app"] = str(apk)

    active_test = dict_lookup(recipe, "deployment.ftl.test", fallback=None)
    flags_dict = dict_lookup(recipe, "deployment.ftl.flags", {})

    all_physical_devices = dict_lookup(recipe,
                                       "deployment.ftl.all-physical-devices",
                                       fallback=False)
    enable_systrace = dict_lookup(recipe, "systrace.enabled", fallback=False)
    systrace_keywords = dict_lookup(recipe, "systrace.keywords", fallback=[])
    render_chart = dict_lookup(recipe, "chart.enabled", fallback=False)
    render_chart_suites = dict_lookup(recipe, "chart.suites", [])

    fields = dict_lookup(recipe, "chart.fields", [])
    skipping_fields = dict_lookup(recipe, "chart.skipping", [])

    json_files, systrace_files = run_on_farm_and_collect_reports(
        args_dict=args_dict,
        flags_dict=flags_dict,
        test=active_test,
        enable_systrace=enable_systrace,
        enable_all_physical=all_physical_devices,
        dst_dir=tmp_dir)

    csv_files = []

    if enable_systrace:
        for json_file, systrace_file in zip(json_files, systrace_files):
            # copy from the ftl dest folder into tmp
            dst_json_file = tmp_dir.joinpath(json_file.name)
            shutil.copy(json_file, dst_json_file)

            # convert to csv
            json_file, csv_file = convert_json_report_to_csv(dst_json_file)

            # copy the trace.html file to basename_trace.html
            dst_trace_file = tmp_dir.joinpath(json_file.stem + "_trace.html")
            shutil.copy(systrace_file, dst_trace_file)

            # merge the systrace data into our csv
            csv_file = merge_systrace_into_csv(csv_file, dst_trace_file,
                                               systrace_keywords)

            csv_files.append(csv_file)

    else:
        for json_file in json_files:
            # copy from the ftl dest folder into tmp
            dst_json_file = tmp_dir.joinpath(json_file.name)
            shutil.copy(json_file, dst_json_file)

            # convert to csv
            try:
                json_file, csv_file = convert_json_report_to_csv(dst_json_file)
                csv_files.append(csv_file)
            except:
                print(f"Unable to convert file {json_file} to CSV")

    if render_chart:
        for csv_file in csv_files:
            suites = lib.charting.load_suites(csv_file.resolve())
            for suite in suites:
                if ( suite.suite_name in render_chart_suites \
                    or not render_chart_suites):
                    title = suite.suite_name
                    suite.plot(title, fields, skipping_fields)


# ------------------------------------------------------------------------------

if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description=
        "Run ACT on local or remote devices, and process their results")

    parser.add_argument("--recipe",
                        help="Path to yaml file describing the run task",
                        required=True)

    args = parser.parse_args()

    recipe_path = Path(args.recipe)
    with open(recipe_path) as recipe_file:
        recipe = yaml.load(recipe_file, Loader=yaml.FullLoader)

    # ensure the out/ dir exists for storing reports/csvs/systraces
    # use the name of the provided configuration, or if none, the yaml file
    # to specialize the output dir
    custom_config = dict_lookup(recipe, "build.configuration", fallback=None)
    if custom_config:
        prefix = Path(custom_config).stem
    else:
        prefix = Path(recipe_path).stem

    tmp_dir = create_output_dir(prefix)

    # step one: build the APK

    apk_path = build_apk(
        clean=dict_lookup(recipe, "build.clean", fallback=False),
        custom_configuration=Path(custom_config) if custom_config else None)

    if "local" in recipe["deployment"]:
        run_local_deployment(recipe, apk_path, tmp_dir)

    if "ftl" in recipe["deployment"]:
        run_ftl_deployment(recipe, apk_path, tmp_dir)
