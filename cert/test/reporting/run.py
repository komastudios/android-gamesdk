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
import glob
import shutil
import time
import yaml

from typing import Dict, List, Any
from pathlib import Path

from lib.build import build_apk, APP_ID
from lib.common import *
from lib.report import *
from lib.devicefarm import run_on_farm_and_collect_reports
import lib.graphing
import lib.tasks_runner


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
            "-e",
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


def get_summary_config(recipe: Dict) -> (bool, str):
    """
    Extracts, from the recipe, summary related arguments.
    These are:

    * enabled: a boolean telling whether a summary is enabled at all.
    * html: a boolean to specify if the summary format is html instead of its
            markdown default.
    """
    enabled = dict_lookup(recipe, "summary.enabled", fallback=False)
    fmt = dict_lookup(recipe, "summary.format", fallback="md")
    fmt = lib.graphing.DocumentFormat.from_extension(fmt)

    return enabled, fmt


def load_tasks(tasks_dict: List[Dict], task_ctors: Dict
              ) -> Tuple[List[lib.tasks.Task], lib.tasks.Environment]:
    """Load tasks and create default task environment (internal, called by
    get_preflight_tasks and get_postflight_tasks)
    Args:
        tasks_dict: The dict from a recipe corresponding to pre or postflight
        task_ctors: Dict mapping task action to task constructor
    """
    env = lib.tasks.Environment()
    env.workspace_dir = Path(os.path.realpath(__file__)).parent

    return lib.tasks_runner.load(tasks_dict, task_ctors), env


def get_preflight_tasks(recipe: Dict
                       ) -> Tuple[List[lib.tasks.Task], lib.tasks.Environment]:
    """Load preflight tasks from the local deployment block of the recipe,
    and generate preflight environment
    """
    preflight = dict_lookup(recipe, "deployment.local.preflight", fallback=[])
    return load_tasks(preflight, lib.tasks_runner.PREFLIGHT_TASKS)


def get_postflight_tasks(recipe: Dict
                        ) -> Tuple[List[lib.tasks.Task], lib.tasks.Environment]:
    """Load postflight tasks from the local deployment block of the recipe,
    and generate preflight environment
    """
    postflight = dict_lookup(recipe, "deployment.local.postflight", fallback=[])
    return load_tasks(postflight, lib.tasks_runner.POSTFLIGHT_TASKS)


def uninstall_apk_from_device(device_id: str) -> type(None):
    """
    Uninstalls the APK from the specified device
    """
    try:
        run_command(f"adb -s {device_id} uninstall {APP_ID}")
    except NonZeroSubprocessExitCode as e:
        # uninstall is expected to fail if the APK is not currently installed
        print(
            f"uninstall_apk_from_device[{device_id}] - unable to uninstall"\
                f": {e.message}"
        )


def push_apk_to_device(apk: Path, device_id: str) -> type(None):
    """
    Gets the app installed in the device.

    Args:
        apk: path to the packaged app.
        device_id: the device serial identifier.
    """
    # install the APK
    run_command(f"adb -s {device_id} install -r {str(apk)}")


def block_for_systrace(device_id: str, out_dir: Path,
                       categories: List[str]) -> LocalSystrace:
    """
    Starts systrace and blocks until finished.

    Args:
        device_id: local device serial number.
        out_dir: output directory where to place the HTML capture.
        categories: systrace categories to capture.

    Returns:
        A LocalSystrace instance.
    """
    # Start systrace - this blocks until systrace is ready to run
    return LocalSystrace(device_id=device_id,
                         dst_file=out_dir.joinpath("tracey.html"),
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


def process_ftl_reports(
        out_dir: Path,
        report_files: List[Path],
        systrace_files: List[Path],
        systrace_keywords: List[str],
) -> List[Path]:
    """
    Given report files (and optional systrace files) copy the files into the
    designated output dir and normalize file names to convention, merging
    systrace data if available

    Args:
        out_dir: directory where gCloud test results got downloaded.
        report_files: list of report file names (one per device).
        systrace_files: list of associated systrace collections. May be empty
                        if systrace_enabled is False.
        systrace_keywords: systrace keywords to filter.

    Returns:
        A list paths to report files (per device) with input data consolidated
        and transformed.
    """
    out_files = []
    report_pairs = zip(report_files, systrace_files) if systrace_files else map(
        lambda json_file: (json_file, None), report_files)
    for report_file, systrace_file in report_pairs:
        # copy from the ftl dest folder into tmp
        dst_report_file = out_dir.joinpath(report_file.name)
        shutil.copy(report_file, dst_report_file)

        try:
            # normalize the file name
            report_file = normalize_report_name(dst_report_file)

            if systrace_file:
                # copy the trace.html file to basename_trace.html
                dst_trace_file = out_dir.joinpath(
                    f"{report_file.stem}_trace.html")
                shutil.copy(systrace_file, dst_trace_file)

                # merge the systrace data
                report_file = merge_systrace(report_file, dst_trace_file,
                                             systrace_keywords)

            out_files.append(report_file)

        except:
            print(f"Unable to process file {report_file}")

    return out_files


def generate_report_summary(report_files_dir: Path,
                            doc_fmt: str,
                            figure_dpi=600) -> Path:
    report_files = [
        Path(f) for f in glob.glob(str(report_files_dir) + '/*.json')
    ]

    report_summary_file_name = f"summary_{str(report_files_dir.stem)}"
    report_summary_file = report_files_dir.joinpath(report_summary_file_name)

    lib.graphing.render_report_document(report_files, report_summary_file,
                                        doc_fmt, figure_dpi)


# ------------------------------------------------------------------------------


def run_local_deployment(recipe: Dict, apk: Path, out_dir: Path):
    if not dict_lookup(recipe, "deployment.local.all_attached_devices", False):
        devices = dict_lookup(recipe, "deployment.local.device_ids", None)
        devices = list(set(devices) & set(get_attached_devices()))
    else:
        devices = get_attached_devices()

    systrace_enabled, systrace_keywords, systrace_categories = \
        get_systrace_config(recipe)

    summary_enabled, summary_fmt = get_summary_config(recipe)

    # at present, preflight_tasks are only available for local deployment
    preflight_tasks, preflight_env = get_preflight_tasks(recipe)
    postflight_tasks, postflight_env = get_postflight_tasks(recipe)

    print("Will run local deployment on the following ADB device IDs:" +
          unpack(devices))

    print(f"\tSystrace: {systrace_enabled} systrace_keywords: " +
          unpack(systrace_keywords) + "\n\n")

    for device_id in devices:
        uninstall_apk_from_device(device_id)
        push_apk_to_device(apk, device_id)

        # run preflight tasks
        lib.tasks_runner.run(preflight_tasks, device_id, preflight_env)

        # if enabled, start systrace and wait for it
        systracer = block_for_systrace(
            device_id, out_dir,
            systrace_categories) if systrace_enabled else None

        # run test!
        start_test_and_wait_for_completion(device_id)

        # extract report
        log_file, sys_file = extract_and_export(
            device_id,
            dst_dir=out_dir,
            systrace_file=systracer.finish() if systracer else None,
            systrace_keywords=systrace_keywords)

        # run postflight tasks
        lib.tasks_runner.run(postflight_tasks, device_id, postflight_env)

    if summary_enabled:
        generate_report_summary(out_dir, summary_fmt)


# ------------------------------------------------------------------------------


def run_ftl_deployment(recipe: Dict, apk: Path, out_dir: Path):
    # first step is to save out an args.yaml file to pass on to gsutil API
    args_dict = dict_lookup(recipe, "deployment.ftl.args", fallback=None)
    for test in args_dict:
        args_dict[test]["app"] = str(apk)

    tests = dict_lookup(recipe, "deployment.ftl.args", fallback=None)
    fallback_test = list(tests.keys())[0]
    active_test = dict_lookup(recipe,
                              "deployment.ftl.test",
                              fallback=fallback_test)
    flags_dict = dict_lookup(recipe, "deployment.ftl.flags", {})

    all_physical_devices = dict_lookup(recipe,
                                       "deployment.ftl.all-physical-devices",
                                       fallback=False)

    systrace_enabled, systrace_keywords, systrace_categories = \
        get_systrace_config(recipe)

    summary_enabled, summary_fmt = get_summary_config(recipe)

    report_files, systrace_files = run_on_farm_and_collect_reports(
        args_dict=args_dict,
        flags_dict=flags_dict,
        test=active_test,
        enable_systrace=systrace_enabled,
        enable_all_physical=all_physical_devices,
        dst_dir=out_dir)

    report_files = process_ftl_reports(out_dir, report_files, systrace_files,
                                       systrace_keywords)

    if summary_enabled:
        generate_report_summary(out_dir, summary_fmt)


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

    # ensure the out/ dir exists for storing reports/systraces/etc
    # use the name of the provided configuration, or if none, the yaml file
    # to specialize the output dir
    custom_config = dict_lookup(recipe, "build.configuration", fallback=None)
    if custom_config:
        prefix = Path(custom_config).stem
    else:
        prefix = Path(recipe_path).stem

    out_dir = create_output_dir(prefix)

    # step one: build the APK

    apk_path = build_apk(
        clean=dict_lookup(recipe, "build.clean", fallback=False),
        release=dict_lookup(recipe, "build.release", fallback=False),
        custom_configuration=Path(custom_config) if custom_config else None)

    if "local" in recipe["deployment"] and dict_lookup(
            recipe, "deployment.local.enabled", fallback=True):
        run_local_deployment(recipe, apk_path, out_dir)

    if "ftl" in recipe["deployment"] and dict_lookup(
            recipe, "deployment.ftl.enabled", fallback=True):
        run_ftl_deployment(recipe, apk_path, out_dir)
