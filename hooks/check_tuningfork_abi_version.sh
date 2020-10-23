#!/bin/bash

TUNINGFORK_H="include/tuningfork/tuningfork.h"
TUNINGFORK_EXTRA_H="include/tuningfork/tuningfork_extra.h"
UNITY_TUNINGFORK_H="include/tuningfork/unity_tuningfork.h"
TUNINGFORK_HEADERS=($TUNINGFORK_H $TUNINGFORK_EXTRA_H $UNITY_TUNINGFORK_H)

files=$2
headerChanged=0
for file in ${files[@]}; do
    for header in ${TUNINGFORK_HEADERS[@]}; do
      if [[ $file =~ $header ]]; then
        headerChanged=1
        break
      fi
    done
done

if [ $headerChanged -eq "1" ]; then

    majorVersion=`git show $1 $TUNINGFORK_H | grep "#define TUNINGFORK_MAJOR_VERSION" | wc -l`
    minorVersion=`git show $1 $TUNINGFORK_H | grep "#define TUNINGFORK_MINOR_VERSION" | wc -l`
    # echo "version_files $version_files"
    if [[ $majorVersion -eq "0" && $minorVersion -eq "0" ]]; then
      echo "Did you break the TuningFork ABI without changing TUNINGFORK_MAJOR_VERSION or TUNINGFORK_MINOR_VERSION ?"
      exit 1
    fi

fi

exit 0
