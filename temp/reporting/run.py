#!/usr/bin/env python3

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
"""Entry point for deploying test recipes on local or remote devices"""

import argparse
from pathlib import Path
import sys

from lib.common import Recipe
from lib.deployment import run_recipe

# ------------------------------------------------------------------------------

RECIPE_DEFAULTS = "./recipes/defaults.yaml"

# ------------------------------------------------------------------------------


def main():
    """main entry point for command-line execution
    """
    parser = argparse.ArgumentParser(
        description=
        "Run ACT on local or remote devices, and process their results")

    parser.add_argument("--recipe",
                        help="Path to yaml file describing the run task",
                        required=True)

    parser.add_argument("--local",
                        help="Run local deployment",
                        default=False,
                        action="store_true")

    parser.add_argument("--ftl",
                        help="Run FTL deployment",
                        default=False,
                        action="store_true")

    args = parser.parse_args()

    extra_args = {"local": args.local, "ftl": args.ftl}

    if not args.local and not args.ftl:
        print("At least one of 'local' and 'ftl' must be set"\
            ", else this script does nothing.")
        parser.print_help()
        sys.exit(1)

    recipe = Recipe(Path(args.recipe), Path(RECIPE_DEFAULTS))
    run_recipe(recipe, extra_args)


if __name__ == "__main__":
    main()
