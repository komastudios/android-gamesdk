#!/bin/bash
# Gamesdk build driver script
# Usage:
# ./build.sh
#   Builds the gamesdk with Swappy and Tuning Fork (no samples)
# ./build.sh samples
#   Builds the gamesdk with Swappy and the Swappy samples
# ./build.sh full
#   Builds the gamesdk with Swappy, Tuning Fork and all samples

set -e # Exit on error

# Set up the environment
export ANDROID_HOME=$(pwd)/../prebuilts/sdk
export ANDROID_NDK_HOME=$(pwd)/../prebuilts/ndk/r20
cp -Rf samples/sdk_licenses ../prebuilts/sdk/licenses

# Use the distribution path given to the script by the build bot in DIST_DIR. Otherwise,
# build in the default location.
if [[ -z $DIST_DIR ]]
then
    dist_dir=$(pwd)/../package
else
    dist_dir=$DIST_DIR
fi

# Build the Game SDK distribution zip and the zips for Maven AARs
if [[ $1 == "full" ]]
then
    package_name=fullsdk
    ./gradlew packageZip -Plibraries=swappy,tuningfork -PincludeSampleSources -PincludeSampleArtifacts -PdistPath="$dist_dir" -PpackageName=$package_name
    ./gradlew packageMavenZip -Plibraries=swappy -PdistPath="$dist_dir" -PpackageName=$package_name
    ./gradlew packageMavenZip -Plibraries=tuningfork -PdistPath="$dist_dir" -PpackageName=$package_name
elif [[ $1 == "samples" ]]
then
    package_name=gamesdk
    ./gradlew packageZip -Plibraries=swappy -PincludeSampleSources -PincludeSampleArtifacts -PdistPath="$dist_dir"
    ./gradlew packageMavenZip -Plibraries=swappy -PdistPath="$dist_dir"
else
    package_name=gamesdk
    ./gradlew packageZip -Plibraries=swappy,tuningfork -PincludeSampleSources -PdistPath="$dist_dir"
    ./gradlew packageMavenZip -Plibraries=swappy -PdistPath="$dist_dir" -PpackageName=$package_name
    ./gradlew packageMavenZip -Plibraries=tuningfork -PdistPath="$dist_dir" -PpackageName=$package_name
fi

# Calculate hash of the zip file
pushd "$dist_dir/$package_name"
sha256sum gamesdk.zip > gamesdk.zip.sha256
popd

<<<<<<< HEAD   (d88333 Merge cherrypicks of [1326755] into android-games-sdk-releas)
# Calculate hash of the AAR files
pushd $dist_dir
sha256sum gaming-frame-pacing.aar > gaming-frame-pacing.aar.sha256
popd
if [[ $1 == "full" ]]
then
  pushd $dist_dir
  sha256sum gaming-performance-tuner.aar > gaming-performance-tuner.aar.sha256
  popd
fi

# Prepare AAR files to be uploaded on Maven
if [[ $1 == "full" ]]
then
  ./gradlew fullSdkGamingFramePacingAarMavenZip
  ./gradlew fullSdkGamingPerformanceTunerAarMavenZip
fi

pushd $dist_dir
# Remove intermediate files that would be very costly to store
rm -rf libs prefab
# Remove other files that we don't care about and are polluting the output
rm -rf external third_party src include samples
=======
pushd "$dist_dir/$package_name"
# Remove intermediate files that would be very costly to store
rm -rf libs prefab
# Remove other files that we don't care about and are polluting the output
rm -rf external third_party src include samples aar
>>>>>>> BRANCH (145218 Merge "Add Tuning Fork to build.sh default target (used by p)
popd