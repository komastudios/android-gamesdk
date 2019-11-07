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
import subprocess
import tempfile
import shutil
import os
import re
import csv

from pathlib import Path
from typing import List, Tuple
from bs4 import BeautifulSoup

from lib.build import APP_ID
from lib.common import *
from lib.csv import *
from lib.systrace import *


def convert_json_report_to_csv(json_file: Path) -> Tuple[Path, Path]:
    """Converts a JSON report to CSV, renaming the input and output
    in a manner which identifies the device/sdk of the device which ran
    the test
    Args:
        json_file: Path to the JSON report file
    Returns:
        Tuple of the new JSON file path, and the generated CSV file path
    """

    csv_file: Path = json_file.with_name(json_file.stem + ".csv")
    device_info = json_report_to_csv(json_file, csv_file)
    device_identifier = device_info["PRODUCT"] + \
        "_" + str(device_info["SDK_INT"])

    renamed_json_file: Path = json_file.with_name("report_" +
                                                  device_identifier + ".json")
    shutil.move(json_file, renamed_json_file)

    renamed_csv_file: Path = csv_file.with_name("report_" + device_identifier +
                                                ".csv")
    shutil.move(csv_file, renamed_csv_file)

    return renamed_json_file, renamed_csv_file


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


def merge_systrace_into_csv(csv_file: Path, systrace_file: Path,
                            systrace_keywords: List[str]) -> Path:
    """Merges select lines from `systrace_file` into `csv_file`
    Args:
        csv_file: file containing the source csv data
        systrace_file: systrace html file
        systrace_keywords: Each systrace data line which contains one or more 
            of the keywords will be mapped into the csv
    Returns:
        A new file sibling to csv_file named <csv_file_name>_merged.csv 
            containing the systrace data interlieved with the original
    """
    offset_ns, useful_lines = filter_systrace_to_interested_lines(
        systrace_file, keywords=systrace_keywords)

    systrace_csvs = []
    for line in useful_lines:
        systrace_csvs.append(convert_systrace_line_to_csv(line, offset_ns))

    # now we need to merge our systrace output into the csv
    report_csv = read_csv(csv_file)
    merged = merge_csvs([report_csv, systrace_csvs])
    merged_csv_file = csv_file.with_name(csv_file.stem + "_merged.csv")
    write_csv(merged_csv_file, merged)
    return merged_csv_file


def extract_and_export(device_id: str, dst_dir: Path, systrace_file: Path,
                       systrace_keywords: List[str],
                       bucket: str) -> Tuple[Path, Path, Path]:
    log_file = extract_log_from_device(device_id, dst_dir)
    log_file, csv_file = convert_json_report_to_csv(log_file)

    if systrace_file and systrace_file.exists():
        new_systrace_file = systrace_file.parent.joinpath(log_file.stem +
                                                          "_trace.html")
        systrace_file.rename(new_systrace_file)
        systrace_file = new_systrace_file

    print(
        f"Saved log file to: {log_file} (csv: {csv_file}) (systrace: {systrace_file if systrace_file else None})"
    )

    if systrace_file and systrace_file.exists():
        csv_file = merge_systrace_into_csv(csv_file, systrace_file,
                                           systrace_keywords)

    if bucket is not None:
        upload_to_gcloud(csv_file, bucket)

    return log_file, csv_file, systrace_file
