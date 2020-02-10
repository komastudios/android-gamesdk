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
"""Command-line interface for generating reports from individual
report JSON files as well as batches.
Example operation:
    python graphy.py --fmt html --dpi 150 path/to/report/dir
"""

import argparse
import glob

from pathlib import Path

from lib.summary import generate_summary

# DPI to save figure graphics; higher looks better but is slower
FIGURE_DPI = 300


def summary_document_name(name):
    """Helper for generating a summary document file name"""
    return f"summary_{name}"


def main():
    """Main entry point"""
    parser = argparse.ArgumentParser()
    parser.add_argument("--dpi",
                        help="Render figures at DPI",
                        default=FIGURE_DPI,
                        type=int)

    parser.add_argument("--fmt",
                        default="md",
                        help="Doc formats: [\"md\", \"html\", \"docx\"]")

    parser.add_argument(
        "path",
        type=str,
        nargs=1,
        metavar="Path to report file or folder",
        help="Path to document or folder of documents to render")

    args = parser.parse_args()
    dpi = args.dpi
    path = Path(args.path[0])
    doc_fmt = args.fmt

    if not path.exists():
        print(f"File {path} does not found; bailing.")

    elif path.is_dir():
        # this is a batch
        reports = [Path(f) for f in sorted(glob.glob(f"{path}/*_report*.json"))]
        errors = [Path(f) for f in sorted(glob.glob(f"{path}/*_error*.json"))]
        generate_summary(reports, errors, doc_fmt, dpi)

    elif path.stem.endswith("_report"):
        generate_summary([path], [], doc_fmt, dpi)

    elif path.stem.endswith("_error"):
        generate_summary([], [path], doc_fmt, dpi)


if __name__ == "__main__":
    main()
