#!/bin/bash
export ANDROID_HOME=../prebuilts/sdk
export ANDROID_NDK=../prebuilts/ndk/r17
./gradlew gamesdk gamesdkZip
