#!/bin/bash
set -e # Exit on error
export ANDROID_HOME=`pwd`/../prebuilts/sdk
export ANDROID_NDK_HOME=`pwd`/../prebuilts/ndk/r17
./gradlew gamesdk gamesdkZip

# Build samples
cp -Rf samples/sdk_licenses ../prebuilts/sdk/licenses
pushd samples/bouncyball
./gradlew build
popd
# Commented out because of NDK warning
#pushd samples/cube
#./gradlew build
#popd
pushd samples/tuningfork/tftestapp
./gradlew build
popd
