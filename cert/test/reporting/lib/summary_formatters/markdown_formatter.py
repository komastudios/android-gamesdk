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

    def generate_summary(self, report_files: List[Path],
                         report_summary_file: Path,
                         figure_dpi: int) -> type(None):
        suites_by_name = {}
        all_suites = []
        folder = report_summary_file.parent
        image_idx = 0

        def next_image_idx() -> int:
            nonlocal image_idx
            r = image_idx
            image_idx += 1
            return r

        for report_file in report_files:
            suites = load_suites(report_file)
            all_suites.extend(suites)
            for suite in suites:
                suites_by_name.setdefault(suite.name, []).append(suite)

        with open(report_summary_file, "w") as file_writer:
            for suite_name in suites_by_name:
                # trim suites list to just those with a handler
                suites = [s for s in suites_by_name[suite_name] if s.handler]

                if suites:
                    file_writer.write(f"""
# {suite_name}
{get_readable_utc()}
""")

                    for suite in suites:
                        base_name = suite.file.stem
                        title = suite.identifier()
                        plot_file = folder.joinpath("images").joinpath(
                            f"{base_name}_{next_image_idx()}.png")
                        summary = suite.handler.plot(plot_file, figure_dpi)
                        file_writer.write(f"""
### {title}

<img src="{str(plot_file.relative_to(folder))}" width="90%" alt="{title}" />

{summary if summary else ""}

---
""")

            # now attempt to generate cross-suite summarizations by finding
            # all the suites which use the same SuiteHandler
            suites_by_handler_class = {}
            for s in [s for s in all_suites if s.handler]:
                suites_by_handler_class.setdefault(s.handler.__class__,
                                                   []).append(s)

            for handler_class in suites_by_handler_class:
                suites = suites_by_handler_class[handler_class]
                if handler_class.can_render_summarization_plot(suites):
                    filename = f"meta_summary_{next_image_idx()}.png"
                    plot_file = folder.joinpath("images").joinpath(filename)

                    summary = handler_class.summarization_plot(
                        suites, plot_file, figure_dpi)

                    file_writer.write(f"""

## Meta Summary


<img src="{str(plot_file.relative_to(folder))}" width="90%" alt="{title}" />

{summary if summary else ""}

---
""")
