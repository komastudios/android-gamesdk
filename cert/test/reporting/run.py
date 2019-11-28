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
from lib.report import extract_and_export, convert_json_report_to_csv, \
    get_device_product_and_api, merge_systrace_into_csv
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


def unpack(s):
    """Convenience string join function"""
    return " ".join(map(str, s))


def get_systrace_config(recipe: Dict) -> (bool, List[str], List[str]):
    """
    Extracts, from the recipe, systrace related arguments.
    These are:

    * enabled: a boolean telling whether systrace is enabled at all.
    * keywords: a list of string keywords to report.
    * categories: a list of string categories to append to the command.
    """
    enabled = dict_lookup(recipe, "systrace.enabled", fallback=False)
    keywords = dict_lookup(recipe, "systrace.keywords", fallback=[])
    categories = dict_lookup(recipe, "systrace.categories", fallback="")
    categories = [] if categories == "" else categories.split(" ")
    return enabled, keywords, categories


def get_chart_config(recipe: Dict) -> (bool, List[str], List[str], List[str]):
    """
    Extracts, from the recipe, chart related arguments.
    These are:

    * enabled: a boolean telling whether charts are enabled at all.
    * suites: if enabled, what suites are to be displayed.
    * fields: if specified, which fields are considered (all if not).
    * skip: if specified, which fields are excluded (none if not).
    """
    enabled = dict_lookup(recipe, "chart.enabled", fallback=False)
    suites = dict_lookup(recipe, "chart.suites", [])
    fields = dict_lookup(recipe, "chart.fields", [])
    skip = dict_lookup(recipe, "chart.skipping", [])
    return enabled, suites, fields, skip


def push_apk_to_device(apk: Path, device_id: str) -> type(None):
    """
    Gets the app installed in the device.

    Args:
        apk: path to the packaged app.
        device_id: the device serial identifier.
    """
    # install the APK
    run_command(f"adb -s {device_id} install -r {str(apk)}")


def block_for_systrace(device_id: str, tmp_dir: Path,
                       categories: List[str]) -> LocalSystrace:
    """
    Starts systrace and blocks until finished.

    Args:
        device_id: local device serial number.
        tmp_dir: temporary directory where to place the HTML capture.
        categories: systrace categories to capture.

    Returns:
        A LocalSystrace instance.
    """
    # Start systrace - this blocks until systrace is ready to run
    return LocalSystrace(device_id=device_id,
                         dst_file=tmp_dir.joinpath("tracey.html"),
                         categories=categories)


def poll_app_active(device_id: str, app_id: str):
    """Check if the given app is running and frontmost on the activity stack

    Args:
        device_id: The USB device to query
        app_id: The id of the app in question

    Returns:
        True if the app is running and at top of activity stack
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


def start_test_and_wait_for_completion(local_device_id: str) -> type(None):
    """
    Launches the test on the device. Doesn't return control until execution
    has finished.

    Args:
        local_device_id: local device serial identifier.
    """
    # launch the activity in GAME_LOOP mode
    command = (f"adb -s {local_device_id} shell am start"
               f" -n \"{APP_ID}/.MainActivity\""
               f" -a \"com.google.intent.action.TEST_LOOP\"")
    run_command(command)

    # wait for the GAME_LOOP to complete
    tick = 0
    while poll_app_active(local_device_id, APP_ID):
        c = tick % 4
        ellipsis = "." * c
        ellipsis += " " * (4 - c)
        print(f"Waiting on app to finish tests{ellipsis}", end='\r')
        tick += 1
        time.sleep(0.25)

    print("\n")
    time.sleep(2)


def map_reports_to_csv(
        tmp_dir: Path,
        json_files: List[Path],
        systrace_files: List[Path],
        systrace_keywords: List[str],
) -> List[Path]:
    """
    Given a series of test results and associated systrace collections, this
    function consolidates these into a list of comma-separated value files (one
    per device).

    Args:
        tmp_dir: directory where gCloud test results got downloaded.
        json_files: list of file names (one per device).
        systrace_files: list of associated systrace collections. May be empty
                        if systrace_enabled is False.
        systrace_keywords: systrace keywords to filter.

    Returns:
        A list of CSV files per device with input data consolidated and
        transformed.
    """
    csv_files = []
    report_pairs = zip(json_files, systrace_files) if systrace_files else map(
        lambda json_file: (json_file, None), json_files)
    for json_file, systrace_file in report_pairs:
        # copy from the ftl dest folder into tmp
        dst_json_file = tmp_dir.joinpath(json_file.name)
        shutil.copy(json_file, dst_json_file)

        try:
            # convert to csv
            json_file, csv_file = convert_json_report_to_csv(dst_json_file)

            if systrace_file:
                # copy the trace.html file to basename_trace.html
                dst_trace_file = tmp_dir.joinpath(
                    f"{json_file.stem}_trace.html")
                shutil.copy(systrace_file, dst_trace_file)

                # merge the systrace data into our csv
                csv_file = merge_systrace_into_csv(csv_file, dst_trace_file,
                                                   systrace_keywords)

            csv_files.append(csv_file)

        except:
            print(f"Unable to convert file {json_file} to CSV")

    return csv_files


def plot_suites(suites: List[str], csv_files: List[Path], fields: List[str],
                skipping: List[str]) -> type(None):
    """
    Plots a sequence of charts.

    Args:
        suites: a list of suite names to plot. If empty, all existing
                suites are plotted.
        csv_files: a list of paths to comma-separated value files (one per
                   tested device) containing sampled data points.
        fields: list of data points to include in the graph (all if empty).
        skipping: list of data points to exclude (none if empty).
    """
    for csv_file in csv_files:
        device, api_level = get_device_product_and_api(csv_file)
        all_suites = lib.charting.load_suites(csv_file.resolve())
        for suite in all_suites:
            if (suite.suite_name in suites or not suites):
                title = f"{suite.suite_name}-{device} API {api_level}"
                suite.plot(title, fields, skipping)


# ------------------------------------------------------------------------------


def run_local_deployment(recipe: Dict, apk: Path, tmp_dir: Path):
    if not dict_lookup(recipe, "deployment.local.all_attached_devices", False):
        devices = dict_lookup(recipe, "deployment.local.device_ids", None)
        devices = list(set(devices) & set(get_attached_devices()))
    else:
        devices = get_attached_devices()

    systrace_enabled, systrace_keywords, systrace_categories = \
        get_systrace_config(recipe)
    chart_enabled, chart_suites, chart_fields, chart_skip = \
        get_chart_config(recipe)

    print("Will run local deployment on the following ADB device IDs:" +
          unpack(devices))
    print(f"\tSystrace: {systrace_enabled} systrace_keywords: " +
          unpack(systrace_keywords) + "\n\n")

    for device_id in devices:
        push_apk_to_device(apk, device_id)

        systracer = block_for_systrace(
            device_id, tmp_dir,
            systrace_categories) if systrace_enabled else None

        start_test_and_wait_for_completion(device_id)

        # extract report (for now we're skipping systrace operations)
        log_file, csv_file, sys_file = extract_and_export(
            device_id,
            dst_dir=tmp_dir,
            systrace_file=systracer.finish() if systracer else None,
            systrace_keywords=systrace_keywords,
            bucket=None)

        if chart_enabled:
            plot_suites(chart_suites, [csv_file], chart_fields, chart_skip)


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

    systrace_enabled, systrace_keywords, systrace_categories = \
        get_systrace_config(recipe)
    chart_enabled, chart_suites, chart_fields, chart_skip = \
        get_chart_config(recipe)

    json_files, systrace_files = run_on_farm_and_collect_reports(
        args_dict=args_dict,
        flags_dict=flags_dict,
        test=active_test,
        enable_systrace=systrace_enabled,
        enable_all_physical=all_physical_devices,
        dst_dir=tmp_dir)

    csv_files = map_reports_to_csv(tmp_dir, json_files, systrace_files,
                                   systrace_keywords)

    if chart_enabled:
        plot_suites(chart_suites, csv_files, chart_fields, chart_skip)


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
