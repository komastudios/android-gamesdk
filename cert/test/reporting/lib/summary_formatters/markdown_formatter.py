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
"""
Markdown summary formatter.
"""

from pathlib import Path
from typing import TextIO, Union, TypeVar

from lib.summary_components import SummaryFormatter


class MarkdownFormatter(SummaryFormatter):
    """Markdown summary formatter."""

    @classmethod
    def can_publish(cls: TypeVar("SummaryFormatter")) -> bool:
        return False

    def create_writer(self, summary_path: Path) -> TextIO:
        return open(summary_path, "w")

    def init_summary(self, title: str,
                     utc: str, writer: TextIO) \
        -> type(None):
        writer.write(f"""
# {title}
{utc}
""")

    def device_summary(self, device_id: str,
                       plot_path: Path, plot_path_relative: Path,
                       summary: Union[str, type(None)], writer: TextIO) \
                           -> type(None):
        writer.write(f"""
### {device_id}

<img src="{str(plot_path_relative)}" width="90%" alt="{device_id}" />

{summary if summary else ""}

---
""")

    def suite_summary(self, suite_name: str, device_id: str,
                      plot_path: Path, plot_path_relative: Path,
                      summary: Union[str, type(None)], writer: TextIO) \
                           -> type(None):
        writer.write(f"""

## Summary for:\n{suite_name}

<img src="{str(plot_path_relative)}" width="90%" alt="{device_id}" />

{summary if summary else ""}

---
""")

    def cross_suite_summary(self,
                            plot_path: Path, plot_path_relative: Path,
                            summary: Union[str, type(None)], writer: TextIO) \
                                -> type(None):
        writer.write(f"""

## Meta Summary

<img src="{str(plot_path_relative)}" width="90%" alt="Meta Summary" />

{summary if summary else ""}

---
""")
