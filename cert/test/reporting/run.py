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
from lib.devicefarm import DeploymentTarget

# ------------------------------------------------------------------------------

RECIPE_DEFAULTS = "./recipes/defaults.yaml"

# ------------------------------------------------------------------------------


def local():
    """Handle `run.py local` arguments"""
    parser = argparse.ArgumentParser(description="Run locally")

    parser.add_argument(
        "recipe",
        type=str,
        nargs=1,
        metavar="<recipe file>",
        help="Path to yaml file describing the run task")

    args = parser.parse_args(sys.argv[2:])

    extra_args = {"local": True, "ftl": False}
    return extra_args, Recipe(Path(args.recipe[0]), Path(RECIPE_DEFAULTS))


def ftl():
    """Handle `run.py ftl` arguments"""
    parser = argparse.ArgumentParser(description="Run on Firebase Test Lab")

    parser.add_argument(
        "recipe",
        type=str,
        nargs=1,
        metavar="<recipe file>",
        help="Path to yaml file describing the run task")

    parser.add_argument("--public",
                        help="Run on public FTL devices",
                        default=False,
                        action="store_true")

    parser.add_argument("--all",
                        help="Run on all FTL devices, public and private",
                        default=False,
                        action="store_true")

    args = parser.parse_args(sys.argv[2:])

    target = DeploymentTarget.FTL_DEVICES_PRIVATE
    if args.public:
        target = DeploymentTarget.FTL_DEVICES_PUBLIC
    if args.all:
        target = DeploymentTarget.FTL_DEVICES_ALL

    extra_args = {
        "local": False,
        "ftl": True,
        "ftl-deployment-target": target,
    }
    return extra_args, Recipe(Path(args.recipe[0]), Path(RECIPE_DEFAULTS))


def main():
    """main entry point for command-line execution
    """
    parser = argparse.ArgumentParser(
        description=
        "Run ACT on local or remote devices, and process their results",
        usage='''run.py <command> [<args>]
The commands are:
    local       Run on locally attached devices
    ftl         Run on Firebase Test Lab
''')

    parser.add_argument('command', help="Subcommand to run")
    args = parser.parse_args(sys.argv[1:2])
    if args.command == "local":
        extra_args, recipe = local()
    elif args.command == "ftl":
        extra_args, recipe = ftl()
    else:
        print("Unrecognized command")
        parser.print_help()
        sys.exit(1)

    run_recipe(recipe, extra_args)


if __name__ == "__main__":
    main()
