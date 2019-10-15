#!/bin/bash
export ANDROID_HOME=../prebuilts/sdk
export ANDROID_NDK_HOME=../prebuilts/ndk/r17
./gradlew gamesdk gamesdkZip
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
