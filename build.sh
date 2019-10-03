#!/bin/bash
export DIST_DIR=%dist_dir%
export ANDROID_HOME=../prebuilts/sdk
export ANDROID_NDK=../prebuilts/ndk/r17
./gradlew gamesdk gamesdkZip
