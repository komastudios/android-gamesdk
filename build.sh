#!/bin/bash
# Gamesdk build driver script
# Usage:
# ./build.sh
#   Builds the gamesdk with Swappy only
# ./build.sh samples
#   Builds the gamesdk with Swappy and the Swappy samples
# ./build.sh full
#   Builds the gamesdk with Tuning Fork and all samples

set -e # Exit on error
export ANDROID_HOME=`pwd`/../prebuilts/sdk
export ANDROID_NDK_HOME=`pwd`/../prebuilts/ndk/r20
if [[ $1 == "full" ]]
    then
        TARGET=fullSdkZip
	OUTDIR=fullsdk
    else
        TARGET=gamesdkZip
	OUTDIR=gamesdk
fi
./gradlew $TARGET

if [[ -z $DIST_DIR ]]
then
    export dist_dir=`pwd`/../package/$OUTDIR
else
    export dist_dir=$DIST_DIR/$OUTDIR
fi

export apk_dir=$dist_dir/gamesdk/apks

if [[ $1 == "samples" ]] || [[ $1 == "full" ]]
then
   mkdir -p $apk_dir/samples
   mkdir -p $apk_dir/test
   mkdir -p $apk_dir/tools
fi

# Swappy samples etc.
if [[ $1 == "samples" ]] || [[ $1 == "full" ]]
then
    # Build samples
    cp -Rf samples/sdk_licenses ../prebuilts/sdk/licenses
    pushd samples/bouncyball
    ./gradlew build
    popd
    pushd samples/cube
    ./gradlew build
    popd
    pushd test/swappy/testapp
    ./gradlew build
    popd

    # Copy to $apk_dir
    cp samples/bouncyball/app/build/outputs/apk/debug/app-debug.apk \
      $apk_dir/samples/bouncyball.apk
    cp third_party/cube/app/build/outputs/apk/debug/app-debug.apk \
      $apk_dir/samples/cube.apk
    cp test/swappy/testapp/app/build/outputs/apk/debug/app-debug.apk \
      $apk_dir/test/swappytest.apk
fi

# Tuning fork samples etc.
if [[ $1 == "full" ]]
then
    # Build samples
    pushd samples/tuningfork/expertballs
    ./gradlew build
    popd
    pushd samples/tuningfork/scaledballs
    ./gradlew build
    popd
    pushd test/tuningfork/testapp
    ./gradlew build
    popd
    
    # Copy to $apk_dir
    cp samples/tuningfork/expertballs/app/build/outputs/apk/debug/app-debug.apk \
      $apk_dir/samples/expertballs.apk
    cp samples/tuningfork/scaledballs/app/build/outputs/apk/debug/app-debug.apk \
      $apk_dir/samples/scaledballs.apk
    cp src/tuningfork/tools/TuningForkMonitor/app/build/outputs/apk/debug/app-debug.apk \
      $apk_dir/tools/TuningForkMonitor.apk
    cp test/tuningfork/testapp/app/build/outputs/apk/debug/app-debug.apk \
      $apk_dir/test/tuningforktest.apk    
fi

# Package the apks into the zip file
if [[ $1 == "samples" ]] || [[ $1 == "full" ]]
then
  pushd $dist_dir
  zip -r -u gamesdk.zip gamesdk/apks
  popd
fi

# Calculate hash of the zip file
pushd $dist_dir
sha256sum gamesdk.zip > gamesdk.zip.sha256
popd
