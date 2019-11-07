#!/bin/bash

DST_FILE="report.json"
DEVICE="-d"

usage()
{
    echo "usage: extract_report [-s SERIAL] [-o OUTPUT FILE]"
}

while [ "$1" != "" ]; do
    case $1 in
        -s | --serial )     shift
                            DEVICE=$1
                            DEVICE="-s "${DEVICE}
                            ;;
        -o | --out )        shift
                            DST_FILE=$1
                            ;;
        -h | --help )       usage
                            exit
                            ;;
        * )                 usage
                            exit 1
    esac
    shift
done

echo "Extracing report using device" $DEVICE "to file" $DST_FILE
adb $DEVICE shell "run-as com.google.gamesdk.gamecert.operationrunner cat /data/data/com.google.gamesdk.gamecert.operationrunner/files/report.json" > $DST_FILE