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
import json
import os
import re as regex
import shutil
import subprocess
import tempfile

from pathlib import Path
from re import match
from typing import Any, List, Tuple

from lib.build import APP_ID
from lib.common import *
from lib.systrace import *

NANOSEC_EXPRESSION = "^(\d+) (nanoseconds|ns|nsec)$"
MILLISEC_EXPRESSION = "^(\d+) (milliseconds|ms|msec)$"
SEC_EXPRESSION = "^(\d+) (seconds|s|sec)$"
TIME_EXPRESSIONS = [NANOSEC_EXPRESSION, MILLISEC_EXPRESSION, SEC_EXPRESSION]


def to_float(v: Any) -> float:
    """Convenience function for converting Datum numeric values to float"""
    try:
        return float(v)
    except ValueError:
        if v == "True":
            return 1
        elif v == "False":
            return 0
        else:
            for e in TIME_EXPRESSIONS:
                match = regex.search(e, v)
                if match is not None and \
                    len(match.groups()) > 1:
                    return float(match.group(1))

    return None


def flatten_dict(d: dict) -> List[Tuple[str, Any]]:
    """Recursively flatten a dict to a list of (pathname,value) tuples
    Args:
        d: the dict to flatten
    Returns:
        List of tuples where the 0th element is the full path name of the leaf
            value, and the 1th element is the value
    Note:
        doesn't attempt to handle cycles so be careful
    """
    result = []
    for k in d:
        v = d[k]
        if type(v) is dict:
            for r in flatten_dict(v):
                p = k + "." + r[0]
                result.append((p, r[1]))
        else:
            result.append((k, v))

    return result


class Datum(object):
    """Datum maps to the datum entries in the report JSON file
    """

    def __init__(self, issue_id:int, suite_id: str, operation_id: str, thread_id: str,
                 cpu_id: int, timestamp: int, custom: dict):
        self.issue_id = issue_id
        self.suite_id = suite_id
        self.operation_id = operation_id
        self.thread_id = thread_id
        self.cpu_id = cpu_id
        self.timestamp = timestamp

        # the raw custom JSON dict from the report JSON entry
        self.custom = custom

        # the custom dict flattened for easier access
        self.custom_fields_flattened = {}
        for field_name, field_value in flatten_dict(custom):
            self.custom_fields_flattened[field_name] = field_value

    @classmethod
    def from_json(cls, data):
        return cls(**data)

    def to_json(self) -> Dict:
        return {
            "issue_id": self.issue_id,
            "suite_id": self.suite_id,
            "operation_id": self.operation_id,
            "thread_id": self.thread_id,
            "cpu_id": self.cpu_id,
            "timestamp": self.timestamp,
            "custom": self.custom
        }

    def get_custom_field(self, name) -> Any:
        if name in self.custom_fields_flattened:
            return self.custom_fields_flattened[name]
        return None

    def get_custom_field_numeric(self, name) -> float:
        if name in self.custom_fields_flattened:
            return to_float(self.custom_fields_flattened[name])
        return None


class BuildInfo(object):
    """BuildInfo maps to the first line of the report JSON files
    """

    def __init__(self, d: Dict[str, str]):
        self._d = d

    def get(self, feature: str) -> str:
        return self._d[feature]

    def __getitem__(self, key):
        return self._d[key]

    @classmethod
    def from_json(cls, data):
        return cls(data)

    def to_json(self) -> Dict:
        return self._d


class Suite(object):
    """Suite is a list of datums from a report file of the same suite_id
    """

    def __init__(self, name: str, build: BuildInfo, data: List[Datum],
                 file: Path):
        self.name = name
        self.build = build
        self.data = data
        self.file = file
        self.handler = None

    def identifier(self):
        return self.build["MANUFACTURER"] + " " + self.build[
            "DEVICE"] + " SDK " + str(self.build["SDK_INT"])

    def description(self):
        return self.name + " (" + self.build["MANUFACTURER"] + " " + self.build[
            "DEVICE"] + " SDK " + str(self.build["SDK_INT"]) + ")"


def load_report(file: Path) -> (BuildInfo, List[Datum]):
    """Loads a report file, parses it, and returns a tuple of BuildInfo,
        List[Datum]
    Args:
        file: Path to a report json file
    Returns:
        tuple of BuildInfo, list of Datum
    """
    build: BuildInfo = None
    data: List[Datum] = []

    with open(file) as f:
        for i, line in enumerate(f):
            jd = json.loads(line)
            # first line is the build info, every other is a report datum
            if i == 0:
                build = BuildInfo.from_json(jd)
            else:
                datum = Datum.from_json(jd)
                data.append(datum)

    if build is None:
        device_patterns = match(r"^(.+)-(\d+)-.+$", file.name)
        build = {
            "PRODUCT": device_patterns.group(1),
            "SDK_INT": device_patterns.group(2)
        } if device_patterns is not None else {
            "PRODUCT": "Unknown",
            "SDK_INT": "00"
        }

    return build, data


def save_report(file: Path, build_info: BuildInfo, data: List[Datum]):
    """Write a report file
    Args:
        file: The file to save the report to
        build_info: The BuildInfo object which will be the first line of the report
        data: List of datum objects to fill out the report
    """
    with open(file, "w") as f:
        # write build info
        f.write(json.dumps(build_info.to_json()))
        f.write("\n")

        # write each datum
        for d in data:
            f.write(json.dumps(d.to_json()))
            f.write("\n")


def merge_datums(input: List[List[Datum]]) -> List[Datum]:
    """Flattens lists of lists of Datum into one sorted list
    Args:
        input: List of list of Datum
    Returns:
        List of Datum, sorted by timestamp
    """
    flat = []
    for d in input:
        flat.extend(d)

    flat = sorted(flat, key=lambda c: c.timestamp)
    return flat


def normalize_report_name(report_file: Path) -> Path:
    """Renames a report JSON file to our naming convention:
    report_DEVICE_SDK.json
    Args:
        json_file: Path to the JSON report file
    Returns:
        Path of renamed report file
    """
    device_info, data = load_report(report_file)
    device_identifier = device_info["PRODUCT"] + \
        "_" + str(device_info["SDK_INT"])

    renamed_report_file: Path = report_file.with_name("report_" +
                                                      device_identifier +
                                                      ".json")
    shutil.move(report_file, renamed_report_file)
    return renamed_report_file


def extract_log_from_device(device_id: str, dst_dir: Path) -> Path:
    """Extract a JSON report log from a USB-attached device
    Args:
        device_id: The device from which to extract the report log
        dst_dir: The destination directory into which to save the report
    Returns:
        Path to the saved report file
    """
    dst_file = dst_dir.joinpath(f"report_device_{device_id}.json")
    ensure_dir(dst_file)

    cmdline = f"adb -s {device_id} shell \"run-as {APP_ID} cat /data/data/{APP_ID}/files/report.json\" > {dst_file}"

    proc = subprocess.run(cmdline,
                          stdout=subprocess.PIPE,
                          stderr=subprocess.PIPE,
                          encoding='utf-8',
                          shell=True)

    if proc.returncode != 0:
        print(proc.stderr)
        exit(proc.returncode)

    if dst_file.exists and os.path.getsize(dst_file) > 0:
        return dst_file
    else:
        print(f"Unable to extract report from device {device_id}")
        exit()


def merge_systrace(report_file: Path, systrace_file: Path,
                   systrace_keywords: List[str]) -> Path:
    """Merges select lines from `systrace_file` into `report_file` by converting
    the systrace entries into Datum instances, and inserting them at the correct
    time position
    Args:
        report_file: file containing the original report data
        systrace_file: systrace html file
        systrace_keywords: Each systrace data line which contains one or more
            of the keywords will be mapped into the csv
    """
    offset_ns, useful_lines = filter_systrace_to_interested_lines(
        systrace_file, keywords=systrace_keywords)

    systrace_datums = []
    for line in useful_lines:
        datum_dict = convert_systrace_line_to_datum(line, offset_ns)
        datum = Datum.from_json(datum_dict)
        systrace_datums.append(datum)

    # now we need to merge our systrace output
    build_info, datums = load_report(report_file)
    merged = merge_datums([datums, systrace_datums])
    save_report(report_file, build_info, merged)
    return report_file


def extract_and_export(device_id: str, dst_dir: Path, systrace_file: Path,
                       systrace_keywords: List[str]) -> Tuple[Path, Path]:
    """Extracts a report file from the specified attached device,
    normalizes its name, and if systrace info is provided, will merge the
    systrace data into the report file
    Args:
        device_id: ID of attached device
        dst_dir: Place to store output files
        systrace_file: If !None, the contents filtered against systrace_keywords
            will be merged into the datum stream in the report
        systrace_keywords: Keywords used to filter systrace lines
        bucket: If !None, result file(s) will be uploaded to specified bucket/path
    """
    log_file = extract_log_from_device(device_id, dst_dir)
    log_file = normalize_report_name(log_file)

    if systrace_file and systrace_file.exists():
        new_systrace_file = systrace_file.parent.joinpath(log_file.stem +
                                                          "_trace.html")
        systrace_file.rename(new_systrace_file)
        systrace_file = new_systrace_file

    print(
        f"Saved log file to: {log_file} (systrace: {systrace_file if systrace_file else None})"
    )

    if systrace_file and systrace_file.exists():
        log_file = merge_systrace(log_file, systrace_file, systrace_keywords)

    return log_file, systrace_file


def get_device_product_and_api(report_path: Path) -> (str, int):
    """
    Given a report path, infers from it its device and api level.
    """
    device_patterns = match(r"^report_(.+)_(\d+)\.(csv|json)$",
                            report_path.name)
    result = ("UNKNOWN", 0) if device_patterns is None \
    else device_patterns.group(1), int(device_patterns.group(2))

    return result
