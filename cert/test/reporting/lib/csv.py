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

import csv
import json
import argparse
from pathlib import Path
from re import match
from typing import Dict, List, Any, Tuple

# ----------------------------------------------------------------------
# helpers


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


def try_parse_int(s, base=10, val=None):
    try:
        return int(s, base)
    except ValueError:
        return val


time_units_per_ns = {
    "nanoseconds": 1,
    "ns": 1,
    "milliseconds": 1e-6,
    "ms": 1e-6,
    "seconds": 1e-9,
    "s": 1e-9
}


def timestamp_to_ns(timestamp):
    if isinstance(timestamp, str):
        if " " in timestamp:
            tokens = timestamp.split()
            num = float(tokens[0]) if "." in tokens[0] else int(tokens[0])
            unit = tokens[1]
            if unit in time_units_per_ns:
                num = num / time_units_per_ns[unit]
            return num
        else:
            return float(timestamp)
    elif isinstance(timestamp, float) or isinstance(timestamp, int):
        return timestamp
    else:
        raise ValueError(f"timestamp {timestamp} is not a parseable number")


# ----------------------------------------------------------------------
# Models


class CSVEntry:

    def __init__(self, timestamp, suite_id, operation_id, thread_id, cpu_id,
                 token, value: Any):
        self.timestamp = timestamp_to_ns(timestamp)
        self.suite_id = suite_id
        self.operation_id = operation_id
        self.cpu_id = int(cpu_id)
        self.thread_id = thread_id
        self.token = token
        self.value = value

    def __str__(self):
        return f"{self.timestamp}, \"{self.suite_id}\", \"{self.operation_id}\", {self.thread_id}, {self.cpu_id}, \"{self.token}\", \"{self.value}\""

    @classmethod
    def header(cls) -> str:
        return "timestamp, suite_id, operation_id, thread_id, cpu_id, datum_id, value"


class Datum(object):

    def __init__(self, suite_id: str, operation_id: str, thread_id: str,
                 cpu_id: int, timestamp: int, custom: dict):
        self.suite_id = suite_id
        self.operation_id = operation_id
        self.thread_id = thread_id
        self.cpu_id = cpu_id
        self.timestamp = timestamp
        self.custom = custom

    def to_csvs(self) -> List[CSVEntry]:
        return list(
            map(
                lambda r: CSVEntry(self.timestamp, self.suite_id, self.
                                   operation_id, self.thread_id, self.cpu_id, r[
                                       0], r[1]), flatten_dict(self.custom)))

    @classmethod
    def from_json(cls, data):
        return cls(**data)


class BuildInfo(object):

    def __init__(self, d: Dict[str, str]):
        self._d = d

    def get(self, feature: str) -> str:
        return self._d[feature]

    def __getitem__(self, key):
        return self._d[key]

    @classmethod
    def from_json(cls, data):
        return cls(data)


# ----------------------------------------------------------------------
# Helpers


def read_csv(csv_file: Path) -> List[CSVEntry]:
    """Opens our CSV format
    Args:
        csv_file: Path to a file in our CSV format
    Returns:
        List of CSVEntry
    """
    with open(csv_file, newline='') as csvfile:
        dialect = csv.Sniffer().sniff(csvfile.read(1024))
        csvfile.seek(0)
        reader = csv.reader(csvfile, dialect)

        entries_by_suite: Dict[str, List[CSVEntry]] = {}

        next(reader)  # skip the header line

        csvs = [CSVEntry(*row) for row in reader]
        return csvs


def write_csv(out_csv_file: Path, csvs: List[CSVEntry]):
    """Saves a CSV file in our format
    Args:
        out_csv_file: Path to a file to save to
        csvs: Array of CSVEntry
    """
    csv_data = to_csv(csvs)
    with open(out_csv_file, "w") as f:
        f.write(CSVEntry.header() + "\n")
        f.write(csv_data)


def merge_csvs(input: List[List[CSVEntry]]) -> List[CSVEntry]:
    """Flattens lists of lists of CSVEntry into one sorted list
    Args:
        input: List of list of CSVEntry
    Returns:
        List of CSVEntry, sorted by timestamp
    """
    flat = []
    for csvs in input:
        flat.extend(csvs)

    flat = sorted(flat, key=lambda c: c.timestamp)
    return flat


# ----------------------------------------------------------------------
# Bootstrap


def load_report(file: Path) -> (BuildInfo, List[Datum], List[CSVEntry]):
    """Loads a report file, parses it, and returns a tuple of BuildInfo,
        List[Datum], and List[CSVEntry]
    Args:
        file: Path to a report json file
    Returns:
        tuple of BuildInfo, list of Datum, and list of CSVEntry
    """
    build: BuildInfo = None
    data: List[Datum] = []
    csvs: List[CSVEntry] = []

    with open(file) as f:
        for i, line in enumerate(f):
            jd = json.loads(line)
            # first line is the build info, every other is a report datum
            if i == 0:
                build = BuildInfo.from_json(jd)
            else:
                datum = Datum.from_json(jd)
                data.append(datum)
                csvs.extend(datum.to_csvs())

    if build is None:
        device_patterns = match(r"^(.+)-(\d+)-.+$", file.name)
        build = {
            "PRODUCT": device_patterns.group(1),
            "SDK_INT": device_patterns.group(2)
        } if device_patterns is not None else {
            "PRODUCT": "Unknown",
            "SDK_INT": "00"
        }

    return build, data, csvs


def to_csv(csvs: List[CSVEntry]) -> str:
    """builds a string for saving to storage of a list of CSVEntry
    Args:
        csvs: list of CSVEntry
    Returns:
        a string suitable for saving to a csv file
    """
    csvs = sorted(csvs, key=lambda c: c.timestamp)

    csv: str = ""
    for c in csvs:
        csv += str(c) + "\n"

    return csv


def json_report_to_csv(in_json_file: Path, out_csv_file: Path) -> BuildInfo:
    """Reads a report JSON file and saves it to a CSV file
    Args:
        in_json_file: Path to a json report
        out_csv_file: Path to where the CSV should be saved
    Returns:
        BuildInfo from the report suitable for IDing the device
    """
    build_info, data, csvs = load_report(in_json_file)
    csv_data = to_csv(csvs)

    with open(out_csv_file, "w") as f:
        f.write(CSVEntry.header() + "\n")
        f.write(csv_data)

    return build_info