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
"""Helper functions and classes for loading and parsing report JSON files
"""

import json
import os
import re
import shutil
import subprocess
import sys

from pathlib import Path
from typing import Any, Dict, List, Tuple

from lib.build import APP_ID
from lib.common import ensure_dir
from lib.systrace import filter_systrace_to_interested_lines,\
    convert_systrace_line_to_datum
from lib.device import DeviceCatalog, DeviceInfo

NANOSEC_EXPRESSION = r"^(\d+) (nanoseconds|ns|nsec)$"
MILLISEC_EXPRESSION = r"^(\d+) (milliseconds|ms|msec)$"
SEC_EXPRESSION = r"^(\d+) (seconds|s|sec)$"
TIME_EXPRESSIONS = [NANOSEC_EXPRESSION, MILLISEC_EXPRESSION, SEC_EXPRESSION]


def to_float(val: Any) -> float:
    """Convenience function for converting Datum numeric values to float"""
    try:
        return float(val)
    except ValueError:
        if val == "True":
            return 1

        if val == "False":
            return 0

        for expr in TIME_EXPRESSIONS:
            result = re.search(expr, val)
            if result is not None and \
                len(result.groups()) > 1:
                return float(result.group(1))

    return None


def flatten_dict(src_dict: dict) -> List[Tuple[str, Any]]:
    """Recursively flatten a dict to a list of (pathname,value) tuples
    Args:
        src_dict: the dict to flatten
    Returns:
        List of tuples where the 0th element is the full path name of the leaf
            value, and the 1th element is the value
    Note:
        doesn't attempt to handle cycles so be careful
    """
    result = []
    for k in src_dict:
        value = src_dict[k]
        if isinstance(value, dict):
            for sub_path in flatten_dict(value):
                key_path = k + "." + sub_path[0]
                result.append((key_path, sub_path[1]))
        else:
            result.append((k, value))

    return result


class Datum:
    """Datum maps to the datum entries in the report JSON file
    """

    def __init__(self, issue_id: int, suite_id: str, operation_id: str,
                 thread_id: str, cpu_id: int, timestamp: int, custom: dict):
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

    def __eq__(self, other):
        if isinstance(other, Datum):
            return self.issue_id == other.issue_id and \
                self.suite_id == other.suite_id and \
                self.operation_id == other.operation_id and \
                self.thread_id == other.thread_id and \
                self.cpu_id == other.cpu_id and \
                self.timestamp == other.timestamp and \
                self.custom == other.custom
        return False

    def __ne__(self, other):
        return not self.__eq__(other)

    @classmethod
    def from_json(cls, data):
        """Inflate a Datum from a dict"""
        if not "issue_id" in data:
            data["issue_id"] = -1
        return cls(**data)

    def to_json(self) -> Dict:
        """Convert a Datum to a dict suitable for serializing to JSON"""
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
        """Get the value from a datum's `custom` field; for example,
        the Datum may have a custom field like so:
        custom {
            "foo": {
                "bar": 1,
                "baz": {
                    "qux": 2
                }
            }
        }
        Calling get_custom_field("foo.bar") returns 1,
        and calling get_custom_field("foo.baz.qux") returns 2
        """
        if name in self.custom_fields_flattened:
            return self.custom_fields_flattened[name]
        return None

    def get_custom_field_numeric(self, name) -> float:
        """Get the value from a datum's `custom` field, parsed as
        a number. See get_custom_field(). Returns None if not parsable
        to a number.
        """
        if name in self.custom_fields_flattened:
            return to_float(self.custom_fields_flattened[name])
        return None


class BuildInfo:
    """BuildInfo maps to the first line of the report JSON files; just
    a dictionary of build info key/pairs for device/os identification
    """

    def __init__(self, d: Dict[str, str]):
        self._d = d

    def get(self, feature: str) -> str:
        """Simple getter
        Example:
            build_info.get("MANUFACTURER")
        """
        return self._d[feature]

    def __getitem__(self, key):
        return self._d[key]

    def __contains__(self, key):
        return key in self._d

    def __eq__(self, other):
        if isinstance(other, BuildInfo):
            return self.contents() == other.contents()
        return False

    @classmethod
    def from_json(cls, data):
        """Load a BuildInfo from a dict"""
        return cls(data)

    def to_json(self) -> Dict:
        """Convert a BuildInfo instance to a dict suitable for
        passing to json encoding"""
        return self._d

    def contents(self) -> Dict:
        """Get underlying dict"""
        return self._d


class Suite:
    """Suite is a list of datums from a report file of the same suite_id
    """

    def __init__(self, name: str, build: BuildInfo, data: List[Datum],
                 file: Path):
        self.name = name
        self.build = build
        self.data = data
        self.file = file
        self.handler = None

        self.data_by_operation_id = {}
        for datum in data:
            self.data_by_operation_id.setdefault(datum.operation_id,
                                                 []).append(datum)

        if DeviceCatalog()[self.build["DEVICE"]] is None:
            homologate_build_info(self.build["DEVICE"], self.build["SDK_INT"],
                                  self.build)

    def identifier(self) -> str:
        """Composes a suitable identifier for the dataset."""
        return f'{DeviceCatalog()[self.build["DEVICE"]]}'

    def description(self) -> str:
        """Composes a human-readable description of the dataset."""
        return f"""{self.name}
{self.identifier()}"""


def homologate_build_info(codename: str, api_level: str,
                          build_info: BuildInfo) -> BuildInfo:
    """BuildInfo is expected to come from the first line of a device report.
    However, before a crash, that info could be missing. Happily, there's a
    chance that such info is available in the device catalog. If so, it's
    leveraged.
    Conversely, there are occasions where the device info is not available. If
    the build info is available, then the device info is populated from the
    build info.

    Args:
        codename: the device codename.
        api_level: the SDK available.
        build_info: a BuildInfo structure.

    Returns: a BuildInfo homologated with the device info.
    """
    device_info = DeviceCatalog()[codename]
    if build_info is None:
        build_info = {}
        build_info["DEVICE"] = codename
        build_info["SDK_INT"] = api_level
        if device_info is None:
            build_info["BRAND"] = "Unknown"
            build_info["MODEL"] = "Unknown"
        else:
            build_info["BRAND"] = device_info.brand
            build_info["MODEL"] = device_info.model

    if device_info is None:  # build_info is not None
        DeviceCatalog().push(
            DeviceInfo(build_info["DEVICE"], build_info["BRAND"],
                       build_info["MODEL"], build_info["SDK_INT"]))

    return build_info


def load_report(report_file: Path) -> (BuildInfo, List[Datum]):
    """Loads a report file, parses it, and returns a tuple of BuildInfo,
        List[Datum]
    Args:
        report_file: Path to a report json report_file
    Returns:
        tuple of BuildInfo, list of Datum
    """
    build: BuildInfo = None
    data: List[Datum] = []
    device_patterns = re.match(r"^(.+)-(\d+)-.+$", report_file.name)
    codename = device_patterns.group(1)
    api_level = device_patterns.group(2)

    with open(report_file) as file:
        for i, line in enumerate(file):
            json_dict = json.loads(line)
            # first line is the build info, every other is a report datum
            if i == 0:
                build = BuildInfo.from_json(json_dict)
            else:
                datum = Datum.from_json(json_dict)
                data.append(datum)

    build = homologate_build_info(codename, api_level, build)

    return build, data


def save_report(report_file: Path, build_info: BuildInfo, data: List[Datum]):
    """Write a report file
    Args:
        report_file: The file path to save the report to
        build_info: The BuildInfo object which will be the first line
            of the report
        data: List of datum objects to fill out the report
    """
    with open(report_file, "w") as file:
        # write build info
        file.write(json.dumps(build_info.to_json()))
        file.write("\n")

        # write each datum
        for datum in data:
            file.write(json.dumps(datum.to_json()))
            file.write("\n")


def merge_datums(lines: List[List[Datum]]) -> List[Datum]:
    """Flattens lists of lists of Datum into one sorted list
    Args:
        lines: List of list of Datum
    Returns:
        List of Datum, sorted by timestamp
    """
    flat = []
    for datum in lines:
        flat.extend(datum)

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
    build_info, _ = load_report(report_file)
    device_info = DeviceCatalog()[build_info["DEVICE"]]
    renamed_report_file: Path = \
        report_file.with_name(f"{device_info.brand}_{device_info.model}"\
            f"_{device_info.sdk_version}_report.json".replace(" ", "_"))

    # it' spossible the test ran on multiple devices of same type,
    # so assign an incrementing index to subsequent names
    idx = 2
    while renamed_report_file.exists():
        renamed_report_file = report_file.with_name(f"{device_info.brand}_{device_info.model}"\
            f"_{device_info.sdk_version}_report({idx}).json".replace(" ", "_"))
        idx += 1

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
    dst_file = dst_dir.joinpath(f"{device_id}-00-local_report.json")
    ensure_dir(dst_file)

    cmdline = f"adb -s {device_id} shell \"run-as {APP_ID}"
    cmdline += f" cat /data/data/{APP_ID}/files/report.json\" > {dst_file}"

    proc = subprocess.run(cmdline,
                          stdout=subprocess.PIPE,
                          stderr=subprocess.PIPE,
                          encoding='utf-8',
                          check=False,
                          shell=True)

    if proc.returncode != 0:
        print(proc.stderr)
        sys.exit(proc.returncode)
        return None

    if dst_file.exists and os.path.getsize(dst_file) > 0:
        return dst_file

    print(f"Unable to extract report from device {device_id}")
    sys.exit()
    return None


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
        systrace_file, keywords=systrace_keywords, pattern=None)

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
        bucket: If !None, result file(s) will be uploaded to specified
            bucket/path
    """
    log_file = extract_log_from_device(device_id, dst_dir)
    log_file = normalize_report_name(log_file)

    if systrace_file and systrace_file.exists():
        new_systrace_file = systrace_file.parent.joinpath(log_file.stem +
                                                          "_trace.html")
        systrace_file.rename(new_systrace_file)
        systrace_file = new_systrace_file

    print(
        f"Saved log file to: {log_file}" \
             + f" (systrace: {systrace_file if systrace_file else None})"
    )

    if systrace_file and systrace_file.exists():
        log_file = merge_systrace(log_file, systrace_file, systrace_keywords)

    return log_file, systrace_file
