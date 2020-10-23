#!/bin/bash

SWAPPY_PATTERN="src/swappy/.*[.](cpp|h)$"
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

if [ $swappyChanged -eq "1" ]; then
    bugfixVersion=`git show $1 $SWAPPY_COMMON_H | grep "#define SWAPPY_BUGFIX_VERSION" | wc -l`
    if [[ $bugfixVersion -eq "0" ]]; then
      echo "Did you perform a bugfix in Swappy without updating SWAPPY_BUGFIX_VERSION in $SWAPPY_COMMON_H?"
    fi
fi

if [ $tuningforkChanged -eq "1" ]; then
    bugfixVersion=`git show $1 $TUNINGFORK_H | grep "#define TUNINGFORK_BUGFIX_VERSION" | wc -l`
    if [[ $bugfixVersion -eq "0" ]]; then
      echo "Did you perform a bugfix in Tuning Fork without updating TUNINGFORK_BUGFIX_VERSION in $TUNINGFORK_H?"
    fi
fi

if [ $tuningforkChanged -eq "1" ] || [ $swappyChanged -eq "1" ]; then
    exit 1
fi

exit 0
