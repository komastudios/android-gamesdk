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
    """Base class for data consumable by summary formatters."""


class Separator(Item):
    """A separator, such as a line or page break. (The exact implementation is
    to be determined by whatever makes sense for the formatter.)"""


class Heading(Item):
    """Heading data.
    Args:
        text: the string title for a heading.
        level: the heading level; 1 corresponds to H1.
    """
    def __init__(self, text: str, level: int):
        self.text = text
        self.level = level


class Text(Item):
    """Plain text data, which may include multiple paragraphs separated by
    newline characters.
    Args:
        text: a text string.
    """
    def __init__(self, text: str):
        self.text = text


class Image(Item):
    """Image metadata, corresponding to an existing image file.
    Args:
        path: image path, relative to script root directory.
        alt: alternate name or description of image.
    """
    def __init__(self, path: Path, alt: str = ''):
        self.path = path
        self.alt = alt


class Table(Item):
    """Table data. (Note: the markdown format requires header cells, thus they
    are passed separate from the subsequent rows.)
    Args:
        header: the table's column headers
        rows: an list of rows, where each row is a list of strings; all rows
            must contain the same number of elements as header.
    """
    def __init__(self, header: List[str], rows: List[List[str]]):
        self.header = header
        self.rows = rows
