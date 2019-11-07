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

from datetime import datetime
import argparse
import json
import re
import subprocess
import sys
import tempfile
import shutil
import os
import yaml

from pathlib import Path
from typing import List, Tuple

from lib.report import *
from lib.build import *


def get_all_physical_devices(flags_yaml):
    """Get a list of all physical devices availabe for use on FTL
    Returns:
        Tuple of (list of args for passing on shell command, and JSON
            for using in YAML)
    """
    cmdline = [
        'gcloud',
        'firebase',
        'test',
        'android',
        'models',
        'list',
        '--filter',
        'form=PHYSICAL',
        '--format',
        'json(codename,supportedVersionIds)',
        '--flags-file',
        flags_yaml,
    ]

    proc = subprocess.run(cmdline,
                          stdout=subprocess.PIPE,
                          stderr=subprocess.PIPE,
                          encoding='utf-8')
    if proc.returncode != 0:
        print(proc.stderr)
        exit(proc.returncode)

    result_args_list = []
    result_json_list = []
    devices = json.loads(proc.stdout)
    for device in devices:
        result_args_list.append('--device')
        result_args_list.append('model={},version={}'.format(
            device['codename'], device['supportedVersionIds'][-1]))

        result_json_list.append({
            "model": device['codename'],
            "version": device['supportedVersionIds'][-1]
        })

    result_json = json.dumps(result_json_list)

    return result_args_list, result_json


def run_test(flags_file: Path, args_yaml: Path, test_name: str, enable_systrace,
             run_on_all_physical_devices):
    gcloud_name = (
        '/google/bin/releases/android-games/devicefarm/systrace/gcloud.par'
        if enable_systrace else 'gcloud')
    cmdline = [
        gcloud_name,
        'firebase',
        'test',
        'android',
        'run',
        '--format=json',
        '--flags-file',
        str(flags_file.resolve()),
        # TODO(b/142612658): disable impersonation when API server stops
        # dropping the systrace field when authenticating as a person
        '--impersonate-service-account',
        'gamesdk-testing@appspot.gserviceaccount.com',
        str(args_yaml.resolve()) + ":" + test_name
    ]

    if run_on_all_physical_devices:
        print("run_on_all_physical_devices: - disabled for now.")
        # TODO(shamyl@google.com): Why does this crash the gcloud client?
        # device_args_list, _ = get_all_physical_devices(flags_file)
        # cmdline.extend(device_args_list)

    print('Stand by...\n')
    proc = subprocess.run(cmdline,
                          stdout=subprocess.PIPE,
                          stderr=subprocess.PIPE,
                          encoding='utf-8')

    if proc.returncode != 0:
        print(proc.stderr)

    return proc.stdout, proc.stderr


def display_test_results(stdout, stderr):
    result = json.loads(stdout)
    if len(result) == 0:
        return
    fields = ['axis_value', 'outcome', 'test_details']
    max_width = {f: max(len(res[f]) for res in result) for f in fields}
    for line in result:
        for field in fields:
            print('{:<{w}}'.format(line[field], w=max_width[field]), end='  ')
        print()
    print()


def get_test_info(stderr):
    pattern = (r'^.*GCS bucket at \[(https.*?)\]' + r'.*Test \[(matrix-.*?)\]' +
               r'.*streamed to \[(https.*?)\]')
    re_matches = re.match(pattern, stderr, flags=re.DOTALL)
    return {
        'url_storage': re_matches.group(1),
        'matrix_id': re_matches.group(2),
        'url_info': re_matches.group(3),
    }


def display_test_info(test_info):
    print('GCS:    {}'.format(test_info['url_storage']))
    print('Info:   {}'.format(test_info['url_info']))
    print('Matrix: {}\n'.format(test_info['matrix_id']))


def download_cloud_artifacts(test_info, file_pattern, dst) -> List[Path]:
    pattern = r'^.*storage\/browser\/(.*)'
    re_match = re.match(pattern, test_info['url_storage'])
    gs_dir = re_match.group(1)

    cmdline = ['gsutil', 'ls', 'gs://{}**/{}'.format(gs_dir, file_pattern)]
    proc = subprocess.run(cmdline,
                          stdout=subprocess.PIPE,
                          stderr=subprocess.PIPE,
                          encoding='utf-8')
    if proc.returncode != 0:
        print(proc.stderr)
        exit(proc.returncode)

    tmpdir = tempfile.mkdtemp(prefix=datetime.now().strftime('%Y%m%d-%H%M%S-'),
                              dir=str(dst))

    outfiles = []
    for line in proc.stdout.splitlines():
        name_suffix = line[5 + len(gs_dir):]
        outfile = '{}/{}'.format(tmpdir, name_suffix.replace('/', '_'))
        outfiles.append(Path(outfile))

        cmdline = ['gsutil', 'cp', line, outfile]
        proc = subprocess.run(cmdline)

    return outfiles


def run_on_farm_and_collect_reports(args_dict: Dict, flags_dict: Dict,
                                    test: str, enable_systrace,
                                    enable_all_physical, dst_dir: Path
                                   ) -> Tuple[List[Path], List[Path]]:
    """Runs the tests on FTL, returning a tuple of lists of result json, and result systrace.
    Args:
        args_dict: the contents that ftl expects for the args.yaml parameter
        flags_dict: the contents that ftl expects for the flags file
        test: the top-level test to run as described in args_dict
        enable_systrace: if true, collect systrace from ftl
        enable_all_physical: if true, overrides the devices specified in args_dict for all physical devices available on ftl
        dst_dir: the directory into which result data will be copied
    """

    try:
        args_file: Path = dst_dir.joinpath("args.yaml")
        with open(args_file, "w") as f:
            yaml.dump(args_dict, f)

        flags_file: Path = dst_dir.joinpath("flags.txt")
        with open(flags_file, "w") as f:
            for k in flags_dict:
                line = f"--{k}: {flags_dict[k]}\n"
                f.write(line)

        stdout, stderr = run_test(
            flags_file,
            args_file,
            test,
            enable_systrace=enable_systrace,
            run_on_all_physical_devices=enable_all_physical)

        display_test_results(stdout, stderr)
        test_info = get_test_info(stderr)
        display_test_info(test_info)

        download_cloud_artifacts(test_info, 'logcat', dst_dir)
        result_json_files = download_cloud_artifacts(test_info,
                                                     "results_scenario_0.json",
                                                     dst_dir)

        result_systrace_files = []
        if enable_systrace:
            result_systrace_files = download_cloud_artifacts(
                test_info, "trace.html", dst_dir)

        return result_json_files, result_systrace_files
    finally:
        args_file.unlink()
        flags_file.unlink()
