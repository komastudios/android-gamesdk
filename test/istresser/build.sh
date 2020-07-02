#!/bin/bash
export ANDROID_HOME=`pwd`/../../../prebuilts/sdk
export ANDROID_NDK_HOME=`pwd`/../../../prebuilts/ndk/r17
./gradlew build
