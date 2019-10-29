#!/bin/bash
echo This script downloads prebuilt Android SDK and NDK from AOSP.
echo It assumes that the root repo of Game SDK was initialized.
echo
cd ..
set -e # Exit on error
#Fetch NDKs availble
ndks=`grep -oP 'prebuilts/ndk[^\"]+' .repo/manifest.xml | sort -u`
#Let user choose one
echo NDKs available:
oldIFS=$IFS
IFS=$'\n'
options=($ndks)
IFS=$oldIFS
PS3='Please enter your choice: '
COLUMNS=12
select opt in "${options[@]}" "Exit";
do
  case "$REPLY" in
    $(( ${#options[@]}+1 )) ) echo "Done."; break;
  esac
  echo Downloading $opt ...
  repo sync -c -j8 $opt > /dev/null 2>&1
  echo Finished downloading $opt
done
#Fetch SDKs availble
ndks=`grep -oP 'prebuilts/sdk[^\"]+' .repo/manifest.xml | sort -u`
#Let user choose one
echo SDKs available:
oldIFS=$IFS
IFS=$'\n'
options=($ndks)
IFS=$oldIFS
PS3='Please enter your choice: '
COLUMNS=12
select opt in "${options[@]}" "Exit";
do
  case "$REPLY" in
    $(( ${#options[@]}+1 )) ) echo "Done."; break;;
  esac
  echo Downloading $opt ...
  repo sync -c -j8 $opt > /dev/null 2>&1
  echo Finished downloading $opt
done
