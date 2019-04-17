#!/bin/bash

SWAPPY_H="include/swappy/swappy.h"

for file in ${@}; do
	if [[ $file =~ $SWAPPY_H ]]; then
		# echo "file $file"
		version_files=`git show $1 $SWAPPY_H | grep "#define SWAPPY_API_VERSION" | wc -l`
		# echo "version_files $version_files"
		if [ $version_files -eq "0" ]; then
			echo "Did you break ABI without changing SWAPPY_API_VERSION ?"
			exit 1
		fi
			
	fi
done
exit 0
