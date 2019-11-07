#!/bin/bash

DST_FILE="out/trace.html"

usage()
{
    echo "usage: systrace -s device_serial [-o OUTPUT FILE]"
}

while [ "$1" != "" ]; do
    case $1 in
        -o | --out )        shift
                            DST_FILE=$1
                            ;;
        -h | --help )       usage
                            exit
                            ;;
        -s | --serial )     shift
                            SERIAL_NO=$1
                            ;;
        * )                 usage
                            exit 1
    esac
    shift
done

echo "Running systrace and saving to file" $DST_FILE
python $ANDROID_HOME/platform-tools/systrace/systrace.py --serial $SERIAL_NO -a com.google.gamesdk.gamecert.operationrunner -o $DST_FILE sched freq idle am wm gfx view binder_driver hal dalvik input res
