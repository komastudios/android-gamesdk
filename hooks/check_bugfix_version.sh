#!/bin/bash

SWAPPY_PATTERN="games-frame-pacing/.*[.](cpp|h)$"
TUNINGFORK_PATTERN="src/tuningfork/.*[.](cpp|h)$"

SWAPPY_COMMON_H="include/swappy/swappy_common.h"
TUNINGFORK_H="include/tuningfork/tuningfork.h"

files=$2
swappyChanged=0
tuningforkChanged=0
for file in ${files[@]}; do
    echo $file

    if [[ $file =~ $SWAPPY_PATTERN ]]; then
        swappyChanged=1
    fi
    if [[ $file =~ $TUNINGFORK_PATTERN ]]; then
        tuningforkChanged=1
    fi
done

echo "Swappy changed: $swappyChanged"
echo "TuningFork changed: $tuningforkChanged"

# TODO (willosborn): check that swappy and tuningfork versions match those in VERSIONS

exit 0
