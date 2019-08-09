from __future__ import print_function
import subprocess
import google.protobuf as pb

print('python protobuf contents:')
for attr in dir(pb):
    if attr == '__builtins__':
        continue
    print('{}: {}'.format(attr, getattr(pb, attr)))

subprocess.call(['ls', '-l', '/google'])
subprocess.call(['ls', '-l', '/google/bin'])
subprocess.call(['ls', '-l', '/google/data'])
subprocess.call([
    'stubby', 'ls', '-l', 'blade:cloud-testservice', 'TestExecutionService'
])
