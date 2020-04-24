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
"""Items for summary formatters.
"""

from abc import ABC
from pathlib import Path
from typing import List

class Item(ABC):
    """Base class"""

class Header(Item):
    """Header text data, consumable by a Formatter"""
    def __init__(self, text: str, level: int):
        self.text = text
        self.level = level

class Paragraph(Item):
    """Paragraph data consumable by a Formatter"""
    def __init__(self, text: str):
        self.text = text

class Image(Item):
    """Image data consumable by a Formatter"""
    def __init__(self, path: Path, alt: str = ''):
        self.path = path
        self.alt = alt

class Table(Item):
    """Table data consumable by a Formatter"""
    def __init__(self, header: List[str], rows: List[List[str]]):
        self.header = header
        self.rows = rows
