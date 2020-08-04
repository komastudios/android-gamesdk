#!/bin/bash
set -e # Exit on error
export ANDROID_HOME=`pwd`/../../../prebuilts/sdk
export ANDROID_NDK_HOME=`pwd`/../../../prebuilts/ndk/r17
mkdir "$ANDROID_HOME/licenses" || true
echo "d56f5187479451eabf01fb78af6dfcb131a6481e" > "$ANDROID_HOME/licenses/android-sdk-license"
echo "24333f8a63b6825ea9c5514f83c2829b004d1fee" >> "$ANDROID_HOME/licenses/android-sdk-license"
./gradlew build
