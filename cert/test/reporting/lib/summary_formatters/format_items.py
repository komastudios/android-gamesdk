#
# Copyright 2020 The Android Open Source Project
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
"""Elements for summary formatters.
"""

from abc import ABC
from pathlib import Path
from typing import List

class Item(ABC):
    """Base class for an item consumable by a summary formatter."""

class Section():
    def __init__(self, title: str, items: List[Item]):
        self.title = title
        # TODO(baxtermichael@google.com): include UTC timestamp?
        self.items = items

class Heading(Item):
    """Heading text data. (level=1 corresponds to H1.)"""
    def __init__(self, text: str, level: int):
        self.text = text
        self.level = level

class Paragraph(Item):
    """Paragraph data."""
    def __init__(self, text: str):
        self.text = text

class Image(Item):
    """Image data. Path is relative to the script root directly, not the report.
    (It can be converted relative to the report if necessary.)"""
    def __init__(self, path: Path, alt: str = ''):
        self.path = path
        self.alt = alt

class Table(Item):
    """Table data. The markdown format requires header cells, thus they are
    passed separate from the subsequent rows. Each row in rows is a List of
    cells. The number of cells in each row should be equal."""
    def __init__(self, header: List[str], rows: List[List[str]]):
        self.header = header
        self.rows = rows
