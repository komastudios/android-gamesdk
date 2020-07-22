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
export ANDROID_HOME=$(pwd)/../prebuilts/sdk
export ANDROID_NDK_HOME=$(pwd)/../prebuilts/ndk/r20
cp -Rf samples/sdk_licenses ../prebuilts/sdk/licenses

# Use the distribution path given to the script by the build bot in DIST_DIR. Otherwise,
# build in the default location.
if [[ -z $DIST_DIR ]]
then
    dist_dir=$(pwd)/../package/gamesdk
else
    dist_dir=$DIST_DIR/gamesdk
fi

# Build the Game SDK distribution zip and the zips for Maven AARs
if [[ $1 == "full" ]]
then
    ./gradlew packageZip -Plibraries=swappy,tuningfork -PincludeSampleSources -PincludeSampleArtifacts -PdistPath="$dist_dir"
    ./gradlew packageMavenZip -Plibraries=swappy -PdistPath="$dist_dir"
    ./gradlew packageMavenZip -Plibraries=tuningfork -PdistPath="$dist_dir"
elif [[ $1 == "samples" ]]
then
    ./gradlew packageZip -Plibraries=swappy -PincludeSampleSources -PincludeSampleArtifacts -PdistPath="$dist_dir"
    ./gradlew packageMavenZip -Plibraries=swappy -PdistPath="$dist_dir"
else
    ./gradlew packageZip -Plibraries=swappy -PincludeSampleSources -PdistPath="$dist_dir"
    ./gradlew packageMavenZip -Plibraries=swappy -PdistPath="$dist_dir"
fi

# Calculate hash of the zip file
pushd "$dist_dir"
sha256sum gamesdk.zip > gamesdk.zip.sha256
popd

pushd "$dist_dir"
# Remove intermediate files that would be very costly to store
rm -rf libs prefab
# Remove other files that we don't care about and are polluting the output
rm -rf external third_party src include samples aar
popd