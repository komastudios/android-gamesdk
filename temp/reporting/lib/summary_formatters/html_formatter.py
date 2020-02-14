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
"""HTML summary formatter."""

from pathlib import Path
from typing import TextIO, Union, TypeVar

from lib.summary_formatters.formatter import SummaryFormatter


class HtmlFormatter(SummaryFormatter):
    """HTML summary formatter."""

    def __init__(self):
        self.__writer = None

    @classmethod
    def is_gdrive_compatible(cls: TypeVar("SummaryFormatter")) -> bool:
        return False

    def create_writer(self, summary_path: Path) -> TextIO:
        self.__writer = open(summary_path, "w")
        return self.__writer

    def on_init(self, title: str, summary_utc: str) -> type(None):
        self.__writer.write(f"""<!DOCTYPE html>
<html>
<body>

<h1>{title}</h1>
<p>{summary_utc}</p>
""")

    def on_device(self, device_id: str, plot_path: Path,
                  plot_path_relative: Path,
                  summary: Union[str, type(None)]) -> type(None):
        self.__writer.write(f"""
<h3>{device_id}</h3>
<img src="{plot_path_relative}" width="90%" alt="{device_id}" />
<p>{summary if summary else ""}</p>
<hr/>
""")

    def on_cross_suite(self, plot_path: Path, plot_path_relative: Path,
                       summary: Union[str, type(None)]) -> type(None):
        self.__writer.write(f"""
<h2>Meta Summary</h2>
<img src="{plot_path_relative}" width="90%" alt="Meta Summary" />
<p>{summary if summary else ""}</p>
<hr/>
""")

    def on_errors_available(self, title: str) -> type(None):
        self.__writer.write(f"""
<h2>{title}</h2>
""")

    def on_device_error(self, device_id: str, summary: str) -> type(None):
        self.__writer.write(f"""
<h3>{device_id}</h3>
<p>{summary}</p>
<hr/>
""")

    def on_finish(self) \
        -> type(None):
        self.__writer.write(f"""
</body>
</html>""")
