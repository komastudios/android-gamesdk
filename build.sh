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

# Set up the environment
export ANDROID_HOME=`pwd`/../prebuilts/sdk
export ANDROID_NDK_HOME=`pwd`/../prebuilts/ndk/r20
cp -Rf samples/sdk_licenses ../prebuilts/sdk/licenses

# Build the Game SDK distribution zip and the zips for Maven AARs
if [[ $1 == "full" ]]
then
    OUT_DIR=fullsdk
    ./gradlew packageZip -Plibraries=swappy,tuningfork -PpackageName=fullsdk -PincludeSampleSources -PincludeSampleArtifacts
    ./gradlew packageMavenZip -Plibraries=swappy -PpackageName=fullsdk
    ./gradlew packageMavenZip -Plibraries=tuningfork -PpackageName=fullsdk
elif [[ $1 == "samples" ]]
then
    OUT_DIR=gamesdk
    ./gradlew packageZip -Plibraries=swappy -PincludeSampleSources -PincludeSampleArtifacts
    ./gradlew packageMavenZip -Plibraries=swappy
else
    OUT_DIR=gamesdk
    ./gradlew packageZip -Plibraries=swappy -PincludeSampleSources
    ./gradlew packageMavenZip -Plibraries=swappy
fi

if [[ -z $DIST_DIR ]]
then
    dist_dir=`pwd`/../package/$OUT_DIR
else
    dist_dir=$DIST_DIR/$OUT_DIR
fi

# Calculate hash of the zip file
pushd $dist_dir
sha256sum gamesdk.zip > gamesdk.zip.sha256
popd

pushd $dist_dir
# Remove intermediate files that would be very costly to store
rm -rf libs prefab
# Remove other files that we don't care about and are polluting the output
rm -rf external third_party src include samples aar
popd