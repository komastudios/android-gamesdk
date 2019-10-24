#!/bin/bash
set -e # Exit on error

# Provides sdk, ndk, and cmake dirs for android-build machines
# WARNING: This script should not be run on Mac/Windows
#   The gradle will rewrite local.properties to locate tools

export BENDER_SCRIPT=true # Enables re-writing local.properties
export ANDROID_HOME=`pwd`/../../../prebuilts/sdk
export ANDROID_NDK_HOME=`pwd`/../../../prebuilts/ndk/r17
export BENDER_CMAKE=`pwd`/../../../prebuilts/cmake/linux-x86
cp -rf sdk_licenses ../../../prebuilts/sdk/licenses

./gradlew build