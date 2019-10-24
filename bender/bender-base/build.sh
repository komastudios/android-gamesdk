#!/bin/bash
set -e # Exit on error

echo $OSTYPE

if [[ "$OSTYPE"=="cygwin" || "$OSTYPE"=="msys" || "$OSTYPE"=="darwin"* ]]; then
	echo 'This script should only be run on Linux, and is
	built primarily for use by android-build machines.'
	exit 1;
fi

# Provides sdk, ndk, and cmake dirs for android-build machines
export BENDER_SCRIPT=true # Enables re-writing local.properties
export ANDROID_HOME=`pwd`/../../../prebuilts/sdk
export ANDROID_NDK_HOME=`pwd`/../../../prebuilts/ndk/r17
export BENDER_CMAKE=`pwd`/../../../prebuilts/cmake/linux-x86
cp -rf sdk_licenses ../../../prebuilts/sdk/licenses

./gradlew build