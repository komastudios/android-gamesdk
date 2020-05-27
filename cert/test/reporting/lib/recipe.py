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
"""All tests have a recipe. Here's how."""

from pathlib import Path
from typing import Any, Dict
import yaml

from lib.common import dict_lookup


class Recipe():
    """Represents a union of two recipe files, the primary and the fallback
    """

    def __init__(self, recipe: Path, default_recipe: Path):
        self._recipe_path = recipe
        self._default_recipe_path = default_recipe
        self._name = recipe.stem

        with open(recipe) as recipe_file:
            self._recipe = yaml.load(recipe_file, Loader=yaml.FullLoader)

        with open(default_recipe) as recipe_file:
            self._default_recipe = yaml.load(recipe_file,
                                             Loader=yaml.FullLoader)

    def get_recipe_path(self) -> Path:
        """Get the path to the primary recipe"""
        return self._recipe_path

    def get_default_recipe_path(self) -> Path:
        """Get the path to the default/fallback recipe"""
        return self._default_recipe_path

    def get_recipe(self) -> Dict:
        """Get the primary recipe"""
        return self._recipe

    def get_default_recipe(self) -> Dict:
        """Get the default/fallback recipe"""
        return self._recipe

    def get_name(self) -> str:
        """Returns the recipe file name (without extension)."""
        return self._name

    def lookup(self, key_path: str, fallback: Any = None) -> Any:
        """Looks up a value in a the recipe file, using the value from
        default_recipe as a fallback if not present in src
        Args:
            key_path: A path to lookup, e.g.,
                "deployment.local.all_attached_devices"
        """

        def fallback_default():
            return dict_lookup(self._default_recipe,
                               key_path,
                               fallback=fallback)

        return dict_lookup(self._recipe, key_path, fallback=fallback_default)
