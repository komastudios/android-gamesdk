#!/usr/bin/env python3

from datetime import datetime
import argparse
import json
import os
import re
import subprocess
import sys
import tempfile

def run_test(cmdline_tail):
    cmdline = [
        'gcloud',
        'firebase',
        'test',
        'android',
        'run',
        '--format=json',
        '--flags-file', 'flags.yaml',
    ] + cmdline_tail

    print('Stand by...\n')
    proc = subprocess.run(
        cmdline,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        encoding='utf-8'
    )
    if proc.returncode != 0:
        print(proc.stderr)
        exit(proc.returncode)

    return proc.stdout, proc.stderr


def get_cmdline_args():
    parser = argparse.ArgumentParser()
    parser.add_argument('--all_physical', action='store_true')
    parser.add_argument('--ppscript')
    args, cmdline_tail = parser.parse_known_args()
    if args.all_physical:
        cmdline_tail += get_all_physical_args()
    return (args.ppscript, cmdline_tail)


def get_all_physical_args():
    cmdline = [
        'gcloud',
        'firebase',
        'test',
        'android',
        'models',
        'list',
        '--filter', 'form=PHYSICAL',
        '--format', 'json(codename,supportedVersionIds)',
        '--flags-file', 'flags.yaml',
    ]

    proc = subprocess.run(
        cmdline,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        encoding='utf-8'
    )
    if proc.returncode != 0:
        print(proc.stderr)
        exit(proc.returncode)

    result = []
    devices = json.loads(proc.stdout)
    for device in devices:
        result.append('--device')
        result.append(
            'model={},version={}'.format(
                device['codename'],
                device['supportedVersionIds'][-1]
            )
        )
    return result


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
    pattern = (
        r'^.*GCS bucket at \[(https.*?)\]'
        + r'.*Test \[(matrix-.*?)\]'
        + r'.*streamed to \[(https.*?)\]'
    )
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


def download_cloud_artifacts(test_info, file_pattern):
    pattern = r'^.*storage\/browser\/(.*)'
    re_match = re.match(pattern, test_info['url_storage'])
    gs_dir = re_match.group(1)

    cmdline = ['gsutil', 'ls', 'gs://{}**/{}'.format(gs_dir, file_pattern)]
    proc = subprocess.run(
        cmdline,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        encoding='utf-8'
    )
    if proc.returncode != 0:
        print(proc.stderr)
        exit(proc.returncode)

    tmpdir = tempfile.mkdtemp(
        prefix=datetime.now().strftime('%Y%m%d-%H%M%S-'),
        dir='.'
    )
    for line in proc.stdout.splitlines():
        name_suffix = line[5 + len(gs_dir):]
        cmdline = [
            'gsutil',
            'cp',
            line,
            name_suffix.replace('/', '_')
        ]
        proc = subprocess.run(cmdline, cwd=tmpdir)
    return tmpdir


def postprocess(dir_name, ppscript):
    if not ppscript:
        return
    script = os.path.join(os.getcwd(), 'postprocessing', ppscript)
    proc = subprocess.run(script, cwd=dir_name, encoding='utf-8')
    if proc.returncode != 0:
        exit(proc.returncode)


if __name__ == '__main__':
    ppscript, cmdline_tail = get_cmdline_args()
    stdout, stderr = run_test(cmdline_tail)
    display_test_results(stdout, stderr)
    test_info = get_test_info(stderr)
    display_test_info(test_info)
    dir_name = download_cloud_artifacts(test_info, 'logcat')
    postprocess(dir_name, ppscript)
