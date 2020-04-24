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
from typing import List, TextIO, TypeVar

from lib.summary_formatters.formatter import SummaryFormatter
import lib.summary_formatters.format_items as fmt


class HtmlFormatter(SummaryFormatter):
    """HTML summary formatter."""

    def __init__(self):
        self.__writer = None
        self.__summary_path = None

    @classmethod
    def is_gdrive_compatible(cls: TypeVar("SummaryFormatter")) -> bool:
        return False

    def create_writer(self, summary_path: Path) -> TextIO:
        self.__summary_path = summary_path
        self.__writer = open(summary_path, "w")
        self.on_init()
        return self.__writer

    # --------------------------------------------------------------------------

    def on_init(self) -> type(None):
        self.__writer.write(f"""<!DOCTYPE html>
<html>
{self.styling()}
<body>
<div class="content">
""")

    def on_errors_available(self, title: str) -> type(None):
        self.write_heading(fmt.Heading(title, 2))

    def on_device_error(self, device_id: str, summary: str) -> type(None):
        self.write_heading(fmt.Heading(device_id, 3))
        self.write_text(fmt.Text(summary))
        self.write_separator()

    def on_finish(self) \
        -> type(None):
        self.__writer.write(f"""
</div>
</body>
</html>""")

    # --------------------------------------------------------------------------

    def styling(self) -> str:
        return f"""
    <style>
    body {{
        font-family: sans-serif;
        font-size: 16pt;
    }}
    .content {{
        width: 90%;
        margin: auto;
    }}
    img {{
        width: 100%;
    }}
    .figure-container {{
        padding: 20px 10%;
        text-align: center;
    }}
    hr {{
        margin: 50px 0px;
    }}
    table, th, td {{
        border: 1px solid #424242;
        border-collapse: collapse;
    }}
    table {{
        width: 100%;
    }}
    th, td {{
        padding: 12px 24px;
    }}
    th {{
        background-color: #d0d0d0;
    }}
    </style>
"""

    # --------------------------------------------------------------------------

    def write_separator(self):
        self.__writer.write('<hr/>\n')

    def write_heading(self, heading: fmt.Heading):
        level = heading.level
        self.__writer.write(f'<h{level}>{heading.text}</h{level}>\n')

    def write_text(self, text: fmt.Text):
        self.__writer.write(f'<p>{text.text}</p>\n')

    def write_image(self, image: fmt.Image):
        relative_path = image.path.relative_to(self.__summary_path.parent)
        self.__writer.write(f"""<div class="figure-container">
    <img src="{relative_path}" alt="{image.alt}" />
</div>\n""")

    def write_table(self, table: fmt.Table):
        def write_row(ele: str, row: List[str]):
            self.__writer.write('\t\t<tr>\n')
            for col in row:
                self.__writer.write(f'\t\t\t<{ele}>{col}</{ele}>\n')
            self.__writer.write('\t\t</tr>\n')

        self.__writer.write("""
<div class="figure-container">
    <table>\n""")

        write_row('th', table.header)
        for row in table.rows:
            write_row('td', row)

        self.__writer.write("""
    </table>
</div>\n""")
