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

import lib.graphing

class CSVEntry:
    '''Represents a single line in our output csv format'''

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

    def __init__(self, device_id: str, thread_setup: str, pinned: bool,
                 num_threads: int, milliseconds_sleep: float):
        self.device_id = device_id
        self.thread_setup = thread_setup
        self.pinned = pinned
        self.num_threads = num_threads
        self.milliseconds_sleep = milliseconds_sleep
        self.values = {}

    def append(self, datum):
        '''Append perf data from datum'''

        for value in self.VALUES:
            self.read_and_append_value(datum, value)

    def __hash__(self):
        return hash(self.unique_id())

    def __eq__(self, other):
        if isinstance(other, CSVEntry):
            return self.unique_id() == other.unique_id()
        return False

    def __lt__(self, other):
        if isinstance(other, CSVEntry):
            if self.device_id != other.device_id:
                return self.device_id < other.device_id
            if self.thread_setup != other.thread_setup:
                return self.thread_setup < other.thread_setup
            if self.pinned != other.pinned:
                return self.pinned < other.pinned
            if self.num_threads != other.num_threads:
                return self.num_threads < other.num_threads
            return self.milliseconds_sleep < other.milliseconds_sleep
        return False

    def unique_id(self):
        '''returns a uniquely identifying id for this CSV entry; this
        is used when collating sharded output back together'''
        return self.device_id + ":" + self.thread_setup + \
            ":" + str(self.pinned) + ":" + str(self.num_threads) + \
                ":" + str(self.milliseconds_sleep)

    def to_csv_line(self):
        '''Serialize this CSVEntry to a single CSV line'''
        cols = [
            self.device_id, self.thread_setup,
            str(self.pinned),
            str(self.num_threads),
            str(self.milliseconds_sleep)
        ]

        value_buckets = [self.values[k] for k in self.VALUES]
        for value_bucket in value_buckets:
            cols.append(CSVEntry.__format_numbers_to_csv_array_entry(value_bucket))

        return ", ".join(cols)

    def read_and_append_value(self, datum, key: str):
        '''reads the field marching_cubes_permutation_results.{key} from
        the datum and appends it to the internal value bucket of that name'''
        value = datum.get_custom_field(
            f"marching_cubes_permutation_results.{key}")
        bucket = self.values.setdefault(key, [])
        bucket.append(value)

    @staticmethod
    def csv_header():
        '''Generate a CSV Header line'''
        col_names = [
            "Device Identifier", "CPU Setup", "Pinned", "#Threads",
            "Milliseconds Sleep"
        ]
        col_names.extend(CSVEntry.VALUES)
        return ", ".join(col_names)

    @staticmethod
    def __format_numbers_to_csv_array_entry(numbers):
        return "[{}]".format(" ".join([str(n) for n in numbers]))


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
                    csv_entry = CSVEntry(
                        device_id=device_id,
                        thread_setup=datum.get_custom_field(
                            "marching_cubes_permutation_results.configuration.thread_setup"
                        ),
                        pinned=datum.get_custom_field(
                            "marching_cubes_permutation_results.configuration.pinned"
                        ),
                        num_threads=datum.get_custom_field(
                            "marching_cubes_permutation_results.num_threads_used"
                        ),
                        milliseconds_sleep=datum.get_custom_field(
                            "marching_cubes_permutation_results.configuration.sleep_per_iteration"
                        ),
                    )

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
