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
import os
import glob

from pathlib import Path

import lib.charting


def process(csv_file: Path, args):
    suites = lib.charting.load_suites(csv_file.resolve())
    suite_names = ", ".join(map(lambda s: s.suite_name, suites))
    print(f"\nProcessing csv {str(csv_file)} with suites [{suite_names}]")

    name_components = csv_file.stem.split("_")
    title = f"{name_components[1]} sdk {name_components[2]}"

    flagged = []
    if args.analyze:
        for suite in suites:
            if not suite.analyze(lambda s, d, m: flagged.append(s)):
                print(f"csv {csv_file} passed without warnings!")

    if args.chart:
        for suite in suites:
            suite.plot(title=title)
    elif args.chart_only_flagged_by_analysis:
        for suite in flagged:
            suite.plot(title=title)


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--csv", type=str, help="Input CSV file")

    parser.add_argument("--batch",
                        type=str,
                        help="Path to directory of CSV files")

    parser.add_argument("--analyze", action="store_true", default=False)
    parser.add_argument("--chart", action="store_true", default=False)
    parser.add_argument("--chart_only_flagged_by_analysis",
                        action="store_true",
                        default=False)

    args = parser.parse_args()

    if args.csv:
        csv_file = Path(args.csv)
        process(csv_file, args)

    elif args.batch:
        csv_files = [Path(f) for f in glob.glob(args.batch + '/*.csv')]
        for csv_file in csv_files:
            process(csv_file, args)
