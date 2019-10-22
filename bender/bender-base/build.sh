#!/bin/bash
set -e # Exit on error
export ANDROID_HOME=`pwd`/../../../prebuilts/sdk
export ANDROID_NDK_HOME=`pwd`/../../../prebuilts/ndk/r17
cp -rf sdk_licenses ../../../prebuilts/sdk/licenses
./gradlew build