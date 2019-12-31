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
import yaml

from lib.build import build_apk
from lib.common import create_output_dir, dict_lookup
from lib.deployment import run_local_deployment, run_ftl_deployment

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

    args = parser.parse_args()

    # load recipe
    recipe_path = Path(args.recipe)
    with open(recipe_path) as recipe_file:
        recipe = yaml.load(recipe_file, Loader=yaml.FullLoader)

    # ensure the out/ dir exists for storing reports/systraces/etc
    # use the name of the provided configuration, or if none, the yaml file
    # to specialize the output dir
    custom_config = dict_lookup(recipe, "build.configuration", fallback=None)
    if custom_config:
        prefix = Path(custom_config).stem
    else:
        prefix = Path(recipe_path).stem

    out_dir = create_output_dir(prefix)

    # build the APK
    apk_path = build_apk(
        clean=dict_lookup(recipe, "build.clean", fallback=False),
        release=dict_lookup(recipe, "build.release", fallback=False),
        custom_configuration=Path(custom_config) if custom_config else None)

    # deply locally
    if "local" in recipe["deployment"] and dict_lookup(
            recipe, "deployment.local.enabled", fallback=True):
        run_local_deployment(recipe, apk_path, out_dir)

    # deploy on ftl
    if "ftl" in recipe["deployment"] and dict_lookup(
            recipe, "deployment.ftl.enabled", fallback=True):
        run_ftl_deployment(recipe, apk_path, out_dir)


if __name__ == "__main__":
    main()
