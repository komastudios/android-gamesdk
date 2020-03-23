#!/bin/bash
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

# Build samples
cp -Rf samples/sdk_licenses ../prebuilts/sdk/licenses
pushd samples/bouncyball
./gradlew build
popd
pushd samples/cube
./gradlew build
popd
pushd samples/tuningfork/expertballs
./gradlew build
popd
pushd test/tuningfork/testapp
./gradlew build
popd

dist_dir=$DIST_DIR
if [[ -z dist_dir ]]
    then
	export dist_dir=`pwd`/../package/$OUTDIR
fi

# Calculate hash of the zip file
pushd $dist_dir
sha256sum gamesdk.zip > gamesdk.zip.sha256
popd

# Copy samples to dist_dir
if [[ $1 == "samples" ]] || [[ $1 == "full" ]]
    then
        mkdir -p $dist_dir/samples
        cp samples/bouncyball/app/build/outputs/apk/debug/app-debug.apk \
            $dist_dir/samples/bouncyball.apk
        cp samples/tuningfork/expertballs/app/build/outputs/apk/debug/app-debug.apk \
            $dist_dir/samples/expertballs.apk
fi
