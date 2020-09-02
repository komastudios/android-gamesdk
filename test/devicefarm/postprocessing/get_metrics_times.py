#!/usr/bin/env python2

# Take logcats generated by the metrics test app and extract average time taken
# for each device

import os
import re

FILENAME_ENDING = '_logcat'
pattern = r'Average time for \d+ iterations: (\d+) us'

# Process all logcat files
all_logcats = [
    fname for fname in os.listdir('.') if fname.endswith(FILENAME_ENDING)]

with open('results.txt', 'wt') as out_file:
    for filename in all_logcats:
        with open(filename) as in_file:
            data = in_file.read()
        regex = re.search(pattern, data)
        out_file.write(
            '{:<30} {} us\n'.format(
                filename[:-len(FILENAME_ENDING)] + ':',
                regex.group(1) if regex else '-'))
