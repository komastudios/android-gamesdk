#!/bin/sh

# we can't run tests without an attached android device
device_count=`adb devices | wc -l`
if [ $device_count -ge 3 ]
then
    python -m unittest discover -v -s test
else
    echo "Can't run test suite without at least one attached android device"
fi
