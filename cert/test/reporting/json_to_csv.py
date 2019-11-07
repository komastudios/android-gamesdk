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

import lib.report

if __name__ == "__main__":
    parser = argparse.ArgumentParser()

    parser.add_argument('path',
                        nargs='+',
                        help='Path of a file or a folder of files.')

    parser.add_argument('-e',
                        '--extension',
                        default='json',
                        help='File extension to filter by.')

    args = parser.parse_args()

    # Parse paths
    full_paths = [os.path.join(os.getcwd(), path) for path in args.path]
    files = set()
    for path in full_paths:
        if os.path.isfile(path):
            files.add(path)
        else:
            files |= set(glob.glob(path + '/*' + args.extension))

    for f in files:
        print(f"Converting {f} to CSV")
        lib.report.convert_json_report_to_csv(Path(f))
