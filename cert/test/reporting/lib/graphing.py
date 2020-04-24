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
"""Helper functions to load report JSON documents and generate
reports.
"""

from copy import deepcopy
import json
import platform
from typing import Dict, List

import matplotlib
import matplotlib.pyplot as plt

from lib.graphers.__init__ import SUMMARIZERS
from lib.report import BuildInfo, Datum, Suite
from lib.graphers.suite_handler import SuiteSummarizer

#-------------------------------------------------------------------------------

if platform.system() == "Linux":
    matplotlib.use('gtk3cairo')

SMALL_SIZE = 5
MEDIUM_SIZE = 7
BIGGER_SIZE = 10

plt.rc('font', size=SMALL_SIZE)  # controls default text sizes
plt.rc('axes', titlesize=SMALL_SIZE)  # fontsize of the axes title
plt.rc('axes', labelsize=MEDIUM_SIZE)  # fontsize of the x and y labels
plt.rc('xtick', labelsize=SMALL_SIZE)  # fontsize of the tick labels
plt.rc('ytick', labelsize=SMALL_SIZE)  # fontsize of the tick labels
plt.rc('legend', fontsize=SMALL_SIZE)  # legend fontsize
plt.rc('figure', titlesize=BIGGER_SIZE)  # fontsize of the figure title


#-------------------------------------------------------------------------------


def load_report_data(report_file: str) -> Suite:
    # TODO(baxtermichael@google.com): Document
    build: BuildInfo = None
    data: List[Datum] = []
    suite_id = ''

    with open(report_file) as file:
        for i, line in enumerate(file):
            try:
                line_dict = json.loads(line)
                # first line is the build info, every other is a report datum
                if i == 0:
                    build = BuildInfo.from_json(line_dict)
                else:
                    datum = Datum.from_json(line_dict)
                    data.append(datum)
                    suite_id = datum.suite_id
            except json.decoder.JSONDecodeError as ex:
                print(f'Report file {report_file}, line {i}: skipping due to '
                      f'a JSON parsing error "{ex}":\n{line}')

    return Suite(suite_id, build, data, report_file)


def load_suite_summarizers(report_files: List[str]) -> List[SuiteSummarizer]:
    # TODO(baxtermichael@google.com): Document
    # Load all reports
    reports: List[Suite] = []
    for report_file in report_files:
        reports.append(load_report_data(report_file))

    # Figure out which summarizers to load, initializing them with copies of
    # the report data.
    summarizers: List[SuiteSummarizer] = []
    for summarizer_class in SUMMARIZERS:
        report_copies = []
        for report in reports:
            if summarizer_class.can_handle_report(report):
                report_copies.append(deepcopy(report))
        if report_copies:
            summarizer = summarizer_class(report_copies)
            summarizers.append(summarizer)

    return summarizers
