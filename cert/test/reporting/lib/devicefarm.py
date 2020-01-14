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
"""Helper functions for pushing to the devicefarm, and pulling results back
"""

from datetime import datetime
import json
from pathlib import Path
import re
import subprocess
import tempfile
from typing import Dict, List, Tuple
import yaml

from lib.common import NonZeroSubprocessExitCode
from lib.device import DeviceCatalog, DeviceInfo


def populate_device_catalog_from_gcloud(flags_yaml):
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
                          check=False,
                          encoding='utf-8')
    if proc.returncode != 0:
        raise NonZeroSubprocessExitCode(proc.stderr)

    device_catalog = DeviceCatalog()
    devices = json.loads(proc.stdout)
    for device in devices:
        device_info = DeviceInfo(device['codename'], device['brand'],
                                 device['name'],
                                 device['supportedVersionIds'][-1])
        device_catalog.push(device_info)

    return device_catalog


def make_device_args_list_from_catalog() -> str:
    """Turns the device catalog into a string of --device model/version
    arguments for gcloud."""
    result_args_list = []
    for device in DeviceCatalog():
        result_args_list.append('--device')
        result_args_list.append('model={},version={}'.format(
            device.codename, device.sdk_version))

    return result_args_list


def run_test(flags_file: Path, args_yaml: Path, test_name: str, enable_systrace,
             run_on_all_physical_devices):
    """Executes an android test deployment on firebase test lab
    Args:
        flags_file: Path to the flags file expected by ftl
        args_yaml: Path to the arguments yaml file expected by ftl
        test_name: The name of the test execution as will be displayed in the
            firebase dashboard
        enable_systrace: if true, cause FTL to record a systrace during exec
        run_on_all_physical_devices: if true, overrides any specified devices
            in the args_yaml file and tells firebase to run the test on all
            devices which are backed by actual physical hardware
    Returns:
        tuple of stdout and sterr
    """
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

    populate_device_catalog_from_gcloud(flags_file)
    if run_on_all_physical_devices:
        cmdline.extend(make_device_args_list_from_catalog())

    print('Stand by...\n')
    proc = subprocess.run(cmdline,
                          stdout=subprocess.PIPE,
                          stderr=subprocess.PIPE,
                          check=False,
                          encoding='utf-8')

    if proc.returncode != 0:
        print(proc.stderr)

    return proc.stdout, proc.stderr


def display_test_results(stdout, stderr, dst_dir: Path):
    """Helper function to display output from execution on FTL
    Args:
        stdout: The stdout from run_test()
        sterr: The stderr from run_test()
        dst_dir: Path to the directory where output will be stored
    """
    result = json.loads(stdout)
    if not result:
        return

    fields = ['axis_value', 'outcome', 'test_details']
    max_width = {f: max(len(res[f]) for res in result) for f in fields}
    for line in result:
        for field in fields:
            print('{:<{w}}'.format(line[field], w=max_width[field]), end='  ')
        print()
    print()

    for line in filter(lambda line: line["outcome"] != "Passed", result):
        device_parts = re.match(r"^(.+)-(\d+)-(.+)$", line["axis_value"])
        if device_parts is not None and len(device_parts.groups()) >= 2:
            device_info = DeviceCatalog()[device_parts.group(1)]
            file_name = f"{device_info.brand}_{device_info.model}_" \
                f"{device_parts.group(2)}_error.json"
            args_file: Path = dst_dir.joinpath(file_name)
            with open(args_file, "w") as write_file:
                json.dump(line, write_file)


def get_test_info(stderr):
    """Helper function to get test info from an FTL execution
    Args:
        stderr: The stderr from run_test()
    Returns:
        dictionary of {
            "url_storage" : google cloud storage
            "matrix_id" : the name of the execution as shown in ftl dashboard
            "url_info" : human readable info about the url
        }
    """
    pattern = (r'^.*GCS bucket at \[(https.*?)\]' + r'.*Test \[(matrix-.*?)\]' +
               r'.*streamed to \[(https.*?)\]')
    re_matches = re.match(pattern, stderr, flags=re.DOTALL)
    return {
        'url_storage': re_matches.group(1),
        'matrix_id': re_matches.group(2),
        'url_info': re_matches.group(3),
    }


def display_test_info(test_info):
    """Helper function to display the output of get_test_info()
    """
    print('GCS:    {}'.format(test_info['url_storage']))
    print('Info:   {}'.format(test_info['url_info']))
    print('Matrix: {}\n'.format(test_info['matrix_id']))


def download_cloud_artifacts(test_info, file_pattern, dst: Path) -> List[Path]:
    """Helper function to download ftl exeuction artifacts
    Args:
        test_info: output from get_test_info()
        file_pattern: name of the remote file to download
        dst: Path to the destination folder where artifacts will be downloaded
    """
    pattern = r'^.*storage\/browser\/(.*)'
    re_match = re.match(pattern, test_info['url_storage'])
    gs_dir = re_match.group(1)

    cmdline = ['gsutil', 'ls', 'gs://{}**/{}'.format(gs_dir, file_pattern)]
    proc = subprocess.run(cmdline,
                          stdout=subprocess.PIPE,
                          stderr=subprocess.PIPE,
                          check=False,
                          encoding='utf-8')
    if proc.returncode != 0:
        raise NonZeroSubprocessExitCode(proc.stderr)

    tmpdir = tempfile.mkdtemp(prefix=datetime.now().strftime('%Y%m%d-%H%M%S-'),
                              dir=str(dst))

    outfiles = []
    for line in proc.stdout.splitlines():
        name_suffix = line[5 + len(gs_dir):]
        outfile = '{}/{}'.format(tmpdir, name_suffix.replace('/', '_'))
        outfiles.append(Path(outfile))

        cmdline = ['gsutil', 'cp', line, outfile]
        proc = subprocess.run(cmdline, check=False)

    return outfiles


def run_on_farm_and_collect_reports(args_dict: Dict, flags_dict: Dict,
                                    test: str, enable_systrace,
                                    enable_all_physical, dst_dir: Path
                                   ) -> Tuple[List[Path], List[Path]]:
    """Runs the tests on FTL, returning a tuple of lists of result
    json, and result systrace.
    Args:
        args_dict: the contents that ftl expects for the args.yaml parameter
        flags_dict: the contents that ftl expects for the flags file
        test: the top-level test to run as described in args_dict
        enable_systrace: if true, collect systrace from ftl
        enable_all_physical: if true, overrides the devices specified in
            args_dict for all physical devices available on ftl
        dst_dir: the directory into which result data will be copied
    """

    try:
        args_file: Path = dst_dir.joinpath("args.yaml")
        with open(args_file, "w") as file:
            yaml.dump(args_dict, file)

        flags_file: Path = dst_dir.joinpath("flags.txt")
        with open(flags_file, "w") as file:
            for k in flags_dict:
                line = f"--{k}: {flags_dict[k]}\n"
                file.write(line)

        stdout, stderr = run_test(
            flags_file,
            args_file,
            test,
            enable_systrace=enable_systrace,
            run_on_all_physical_devices=enable_all_physical)

        display_test_results(stdout, stderr, dst_dir)
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
