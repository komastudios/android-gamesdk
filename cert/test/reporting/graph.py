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
from typing import List

import lib.graphing

# DPI to save figure graphics; higher looks better but is slower
FIGURE_DPI = 75


def display_interactive_report(report_file: Path):
    for suite in lib.graphing.load_suites(report_file):
        if suite.handler:
            suite.handler.plot(True)


def summary_document_name(name, html):
    return f"summary_{name}" + (".html" if html else ".md")


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--report", type=str, help="JSON report file to graph")
    parser.add_argument("--batch",
                        type=str,
                        help="Path to folder of JSON report files to graph")
    parser.add_argument(
        "--interactive",
        action="store_true",
        default=False,
        help=
        "Show interactive pyplot window; only enabled for single --report input"
    )

    parser.add_argument("--html",
                        action="store_true",
                        default=False,
                        help="If true, render document as html, not markdown")

    args = parser.parse_args()
    html = args.html

    if args.report:
        report_file = Path(args.report)
        if args.interactive:
            display_interactive_report(report_file)
        else:
            report_summary_file_name = summary_document_name(
                str(report_file.stem), html)

            report_summary_file = report_file.parent.joinpath(
                report_summary_file_name)

            lib.graphing.render_report_document([report_file],
                                                report_summary_file,
                                                dpi=FIGURE_DPI)

    elif args.batch:
        report_files = [Path(f) for f in glob.glob(args.batch + '/*.json')]

        report_summary_file_name = summary_document_name(
            str(Path(args.batch).stem), html)

        report_summary_file = Path(
            args.batch).joinpath(report_summary_file_name)

        lib.graphing.render_report_document(report_files,
                                            report_summary_file,
                                            dpi=FIGURE_DPI)
