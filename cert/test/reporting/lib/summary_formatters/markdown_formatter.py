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

import markdown2
from pathlib import Path
from typing import List, TypeVar

from lib.common import get_readable_utc
from lib.graphing import load_suites
from lib.summary_components import SummaryFormatter


class MarkdownFormatter(SummaryFormatter):

    @classmethod
    def can_publish(cls: TypeVar("SummaryFormatter")) -> bool:
        return False

    def generate_summary(self,
                         report_files: List[Path], report_summary_file: Path,
                         figure_dpi: int) -> type(None):
        suites_by_name = {}
        folder = report_summary_file.parent

        for report_file in report_files:
            for suite in load_suites(report_file):
                suites_by_name.setdefault(suite.name, []).append(suite)

        with open(report_summary_file, "w") as file_writer:
            for suite_name in suites_by_name:
                file_writer.write(f"""
# {suite_name}
{get_readable_utc()}
""")

                for i, suite in enumerate(suites_by_name[suite_name]):
                    if suite.handler:
                        base_name = suite.file.stem
                        title = suite.identifier()
                        summary = suite.handler.summary()
                        plot_file = folder.joinpath("images").joinpath(
                            f"{base_name}.png")
                        suite.handler.plot(False,
                                           plot_file_name=plot_file,
                                           dpi=figure_dpi)
                        file_writer.write(f"""
### {title}

<img src="{str(plot_file.relative_to(folder))}" width="90%" alt="{title}" />

{summary if summary else ""}

---
""")
