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
'''Given a recipe yaml and a template configuration file, create a batch
of recipe/configurations based on all possible permutations of the inputs
'''

import argparse
import copy
import glob
import itertools
import math
import os
from pathlib import Path
import sys
from typing import Any, Dict, List, Tuple
import yaml

from lib.common import dict_lookup


def __json_str(value):
    if isinstance(value, bool):
        return "true" if value else "false"
    return str(value)


def __perform_substitution(text: str, parameters: List[Tuple[str, Any]]) -> str:
    '''for each tuple in parameters, looks for "{{tuple[0]}}" in line
    and replaces it with tuple[1], returning the transformed line'''
    for key, value in parameters:
        key_tag = "{{" + key + "}}"
        if key_tag in text:
            text = text.replace(key_tag, __json_str(value))
    return text


def __write_permutation(recipe: Dict, config_template_path: Path,
                        parameters: List[Tuple[str, Any]], out_dir: Path,
                        out_file_base: str):
    '''Writes a generated set of permutations to a recipe and configuration file
    Args:
        recipe: The base recipe contents which will have the build.configuration
            value set to the generated json configuration path
        config_template_path: Path to the configuration template file
        parameters: list of tuples of (keyname,value) pairs representing the
            permutation to write into the configuration file
        out_dir: The working dir where recipe and configuration will be written
        out_file_base: Base name for recipe and configuration files
    '''
    recipe_path = out_dir / (out_file_base + ".yaml")
    config_path = out_dir / (out_file_base + ".json")

    # write the recipe file
    recipe["build"]["configuration"] = str(config_path)
    with open(recipe_path, "w") as recipe_file:
        yaml.dump(recipe, recipe_file)

    # now do a parameter substitution from source configuration
    # to create the new one; no need to json parse
    with open(config_template_path, "r") as config_template_file,\
        open(config_path, "w") as config_file:
        for line in config_template_file.readlines():
            line = __perform_substitution(line, parameters)
            config_file.write(line)


def __permute(recipe_template: Dict, out_dir: Path, clean: bool):
    base_name = dict_lookup(recipe_template,
                            "build.template.configuration_base_name",
                            fallback=None)
    parameters = dict_lookup(recipe_template,
                             "build.template.parameters",
                             fallback=None)

    config_template_file = dict_lookup(recipe_template,
                                       "build.configuration",
                                       fallback=None)

    permutation_sources = []
    for name, values in parameters.items():
        permute_list = [(name, value) for value in values]
        permutation_sources.append(permute_list)

    permutations = list(itertools.product(*permutation_sources))
    pad_width = int(math.ceil(math.log10(len(permutations))))

    config_template_path = Path(".") / Path(config_template_file)

    # make a copy of recipe and delete the template bit
    recipe = copy.deepcopy(recipe_template)
    del recipe["build"]["template"]

    # delete files matching our template pattern
    if clean:
        for json_file in glob.glob(f"{str(out_dir)}/{base_name}_*.json"):
            Path(json_file).unlink()
        for yaml_file in glob.glob(f"{str(out_dir)}/{base_name}_*.yaml"):
            Path(yaml_file).unlink()
        for output_file in glob.glob(
                f"{str(out_dir)}/{base_name}_*.stdout.txt"):
            Path(output_file).unlink()
        for output_file in glob.glob(
                f"{str(out_dir)}/{base_name}_*.stderr.txt"):
            Path(output_file).unlink()

    for i, parameters in enumerate(permutations):
        file_name_base = f"{base_name}_{i:0{pad_width}d}"
        __write_permutation(copy.deepcopy(recipe), config_template_path,
                            parameters, out_dir, file_name_base)

    print(f"Generated {len(permutations)} permutations")


def main():
    '''cmdline entry point'''
    parser = argparse.ArgumentParser()

    parser.add_argument(
        "--clean",
        help="If true, delete existing files matching the template",
        required=False,
        action="store_true")

    parser.add_argument("--out",
                        help="Folder where recipes will be written",
                        required=False,
                        default="tmp/")

    parser.add_argument("recipe",
                        type=str,
                        nargs=1,
                        metavar="path",
                        help="Path to recipe template file to permute")

    args = parser.parse_args()
    recipe_template_path = Path(args.recipe[0])
    out_dir = Path(args.out)
    clean = args.clean

    if out_dir.exists() and not out_dir.is_dir():
        print(f"Specified output dir {out_dir} is a file, not a directory")
        sys.exit(1)

    if not out_dir.exists():
        os.makedirs(out_dir)

    # open the recipe template
    recipe_template = dict()
    with open(recipe_template_path) as recipe_template_file:
        recipe_template = yaml.load(recipe_template_file,
                                    Loader=yaml.FullLoader)

    __permute(recipe_template, out_dir, clean)


if __name__ == "__main__":
    main()
