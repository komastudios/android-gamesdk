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
'''Collects sharded output from multiple runs of MarchingCubesGLES3Operation
and collates them together into a single CSV file for processing.
'''

import argparse
import os
from pathlib import Path
from typing import Any, Dict, Union

import lib.graphing
import lib.report


class CSVEntry:
    '''Represents a single line in our output csv format'''

    CONFIG_VALUES = [
        "exec_configuration.job_batching_setup",
        "exec_configuration.sleep_config.duration",
        "exec_configuration.sleep_config.method",
        "exec_configuration.sleep_config.period",
        "exec_configuration.thread_affinity",
        "exec_configuration.thread_pinned",
    ]

    VALUES = [
        "num_iterations",
        "min_vps",
        "max_vps",
        "average_vps",
        "median_vps",
        "fifth_percentile_vps",
        "twentyfifth_percentile_vps",
        "seventyfifth_percentile_vps",
        "ninetyfifth_percentile_vps",
    ]

    COL_HEADERS = {
        "exec_configuration.job_batching_setup" : "Batching",
        "exec_configuration.sleep_config.duration" : "Sleep Duration (ns)",
        "exec_configuration.sleep_config.method" : "Sleep Method",
        "exec_configuration.sleep_config.period" : "Sleep Period (ns)",
        "exec_configuration.thread_affinity" : "Thread Affinity",
        "exec_configuration.thread_pinned" : "Thread Pinning"
    }

    def __init__(self, device_id: str, datum: lib.graphing.Datum):
        self.device_id = device_id
        self.config_values = {}
        self.values = {}

        for value in self.CONFIG_VALUES:
            self.read_and_append_value(datum, value, self.config_values)

        # cache the keys we use in unique_id()
        self.job_batching_setup = self.config_values["exec_configuration.job_batching_setup"][0]
        self.sleep_duration = self.config_values["exec_configuration.sleep_config.duration"][0]
        self.sleep_method = self.config_values["exec_configuration.sleep_config.method"][0]
        self.sleep_period = self.config_values["exec_configuration.sleep_config.period"][0]
        self.thread_affinity = self.config_values["exec_configuration.thread_affinity"][0]
        self.thread_pinned = self.config_values["exec_configuration.thread_pinned"][0]

    def append(self, datum: lib.graphing.Datum):
        '''Append perf data from datum'''

        for value in self.VALUES:
            self.read_and_append_value(datum, value, self.values)

    def __hash__(self):
        return hash(self.unique_id())

    def __eq__(self, other):
        if isinstance(other, CSVEntry):
            return self.unique_id() == other.unique_id()
        return False

    def __lt__(self, other):
        if isinstance(other, CSVEntry):
            return self.unique_id() < other.unique_id()
        return False

    def unique_id(self):
        '''returns a uniquely identifying id for this CSV entry; this
        is used when collating sharded output back together'''
        keys = [str(v[0]) for (k, v) in self.config_values.items()]
        return ":".join(keys)

    def to_csv_line(self):
        '''Serialize this CSVEntry to a single CSV line'''

        config_values = [str(self.config_values[k][0]) for k in self.CONFIG_VALUES]
        value_buckets = [self.values[k] for k in self.VALUES]
        values = [
            CSVEntry.__format_numbers_to_csv_array_entry(b)
            for b in value_buckets
        ]

        return ", ".join(config_values + values)

    def read_and_append_value(self, datum, key: str, into: Dict):
        '''reads the field marching_cubes_permutation_results.{key} from
        the datum and appends it to the internal value bucket of that name'''
        value = datum.get_custom_field(
            f"marching_cubes_permutation_results.{key}")
        bucket = into.setdefault(key, [])
        bucket.append(CSVEntry.__to_number_if_possible(value))

    @staticmethod
    def csv_header():
        '''Generate a CSV Header line'''
        col_names = [key for key in CSVEntry.CONFIG_VALUES + CSVEntry.VALUES]
        col_names = [CSVEntry.COL_HEADERS.get(name, name) for name in col_names]
        return ", ".join(col_names)

    @staticmethod
    def __format_numbers_to_csv_array_entry(numbers):
        return "[{}]".format(" ".join([str(round(n)) for n in numbers]))

    @staticmethod
    def __to_number_if_possible(value: Any) -> Union[float, str]:
        if isinstance(value, bool):
            return value
        num = lib.report.to_float(value)
        return num if num else value


def process(report_files, operation_name, out_file):
    '''Process the collection of input report files to a csv file'''
    suites_by_device = {}
    for report_file in report_files:
        for suite in lib.graphing.load_suites(report_file):
            suites_by_device.setdefault(suite.identifier(), []).append(suite)

    csv_entries = {}
    for suites in suites_by_device.values():
        for suite in suites:
            device_id = suite.build["DEVICE"]
            if operation_name in suite.data_by_operation_id:
                for datum in suite.data_by_operation_id[operation_name]:

                    # we're using a prototype csv_entry as a key to find
                    # an existing one, to append min/max/avg per data to
                    csv_entry = CSVEntry(device_id, datum)

                    if csv_entry in csv_entries:
                        csv_entry = csv_entries[csv_entry]
                    else:
                        csv_entries[csv_entry] = csv_entry

                    csv_entry.append(datum)

    with open(out_file, "w") as file:
        file.write(CSVEntry.csv_header() + "\n")
        for entry in csv_entries.values():
            file.write(entry.to_csv_line() + "\n")


def main():
    '''cmdline entry point'''
    parser = argparse.ArgumentParser()

    parser.add_argument("--out",
                        help="Name of output csv file",
                        required=False,
                        default="out/report.csv")

    parser.add_argument(
        "path",
        type=str,
        nargs=1,
        metavar="Path to report file or folder",
        help="Path to document or folder of documents to render")

    args = parser.parse_args()
    path = Path(args.path[0])
    out_file = Path(args.out)

    if path.exists():
        report_files = []
        if path.is_dir():
            for root, _, files in os.walk(path):
                root = Path(root)
                our_files = [
                    file for file in files if file.endswith("_report.json")
                ]
                our_files = [root / file for file in our_files]
                report_files.extend(our_files)
        elif path.suffix == ".json":
            report_files.append(path)

        process(report_files, "MarchingCubesGLES3Operation", out_file)

    else:
        print(f"Dir {path} does not found; bailing.")


if __name__ == "__main__":
    main()
