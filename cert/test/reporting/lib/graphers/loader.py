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
"""Provides factory function load_suite_summarizers which will scan a set of
report files and vend the correct SuiteSummarizer implementation to process
it and generate summaries.
"""

from copy import deepcopy
import json
from typing import List

from lib.graphers.__init__ import SUMMARIZERS
from lib.report import BuildInfo, Datum, Report, Suite, load_report
from lib.graphers.suite_handler import SuiteSummarizer

# -----------------------------------------------------------------------------

# def load_report_data(report_file: str) -> Suite:
#     """
#     Initializes a Suite whose "data" contains all rows from the report
#     (excluding the build info, which is stored as a separate attribute).

#     Note: Initially a Suite was only to contain data with the same "suite_id",
#     but given that it is much easier to filter (rather than join) at later
#     stages in the pipeline, we can reduce overall complexity by simply including
#     any data found in the report file.

#     Args:
#         report_file: path to report file.
#     Returns:
#         an initialized Suite representing the report_file data.
#     """
#     build: BuildInfo = None
#     data: List[Datum] = []
#     suite_id = ''

#     with open(report_file) as file:
#         for i, line in enumerate(file):
#             try:
#                 line_dict = json.loads(line)
#                 # first line is the build info, every other is a report datum
#                 if i == 0:
#                     build = BuildInfo.from_json(line_dict)
#                 else:
#                     datum = Datum.from_json(line_dict)
#                     data.append(datum)
#                     if not suite_id:
#                         suite_id = datum.suite_id
#             except json.decoder.JSONDecodeError as ex:
#                 print(f'Report file {report_file}, line {i}: skipping due to '
#                       f'a JSON parsing error "{ex}":\n{line}')

#     return Suite(suite_id, build, data, report_file)


def load_suite_summarizers(report_files: List[str]) -> List[SuiteSummarizer]:
    """
    Loads report files and determines the set of SuiteSummarizers that claim to
    be able to handle the report data. SuiteSummarizers are initialized with the
    report data, which they can then use to marshall their associated
    SuiteHandlers. A SuiteSummarizer can synthesize results across all of its
    SuiteHandlers, while each SuiteHandler considers a single report file (or
    "Suite") in isolation.

    Args:
        report_files: paths to report files.
    Returns:
        a list of initialized SuiteSummarizers.
    """
    # Load all reports
    reports: List[Suite] = []
    for report_file in report_files:
        build_data, data = load_report(report_file)
        reports.append(Report(build_data, data, report_file))

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
