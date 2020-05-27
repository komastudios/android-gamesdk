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
"""Outer test configuration."""

import json
from pathlib import Path
from typing import Any

from lib.common import dict_lookup


class Configuration():
    """Configuration data for an outer test."""

    def __init__(self, config_path: Path):
        with open(config_path) as json_file:
            self.__data = json.load(json_file)

    def lookup(self, key_path: str, default: Any = None) -> Any:
        """Looks up a value in the configuration data.
        Args:
            key_path: A path to lookup, e.g.,
                "deployment.local.all_attached_devices"
            default: default value to return if key_path isn't found.
        """

        def fallback_default():
            first_entry = dict_lookup(self.__data, 'stress_tests', None)
            if first_entry:
                first_entry = first_entry[0]
                return dict_lookup(first_entry, key_path, fallback=default)
            return default

        return dict_lookup(self.__data, key_path, fallback_default)
