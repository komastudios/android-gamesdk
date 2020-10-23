#!/bin/bash

SWAPPY_GL_H="include/swappy/swappyGL.h"
SWAPPY_GL_EXTRA_H="include/swappy/swappyGL_extra.h"
SWAPPY_VK_H="include/swappy/swappyVk.h"
SWAPPY_VK_EXTRA_H="include/swappy/swappyVk_extra.h"
SWAPPY_COMMON_H="include/swappy/swappy_common.h"

SWAPPY_HEADERS=($SWAPPY_GL_H $SWAPPY_GL_EXTRA_H $SWAPPY_VK_H $SWAPPY_VK_EXTRA_H $SWAPPY_COMMON_H)

files=$2
headerChanged=0
for file in ${files[@]}; do
    for header in ${SWAPPY_HEADERS[@]}; do
      if [[ $file =~ $header ]]; then
        headerChanged=1
        break
      fi
    done
done

if [ $headerChanged -eq "1" ]; then

    majorVersion=`git show $1 $SWAPPY_COMMON_H | grep "#define SWAPPY_MAJOR_VERSION" | wc -l`
    minorVersion=`git show $1 $SWAPPY_COMMON_H | grep "#define SWAPPY_MINOR_VERSION" | wc -l`
    # echo "version_files $version_files"
    if [[ $majorVersion -eq "0" && $minorVersion -eq "0" ]]; then
	echo "Did you break the Swappy ABI without changing SWAPPY_MAJOR_VERSION or SWAPPY_MINOR_VERSION ?"
	exit 1
    fi

fi

exit 0
