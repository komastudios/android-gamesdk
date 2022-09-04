#!/bin/bash
# Gamesdk build driver script
# Usage:
# ./build.sh
#   Builds the gamesdk with Swappy, Tuning Fork and Oboe (no samples)
# ./build.sh samples
#   Builds the gamesdk with Swappy and the Swappy samples
# ./build.sh full
#   Builds the gamesdk with Swappy, Tuning Fork, Oboe and all samples

set -e # Exit on error

# Set up the environment
export ANDROID_HOME=$(pwd)/../prebuilts/sdk
export ANDROID_NDK_HOME=$(pwd)/../prebuilts/ndk/r23
export BUILDBOT_SCRIPT=true
export BUILDBOT_CMAKE=$(pwd)/../prebuilts/cmake/linux-x86
export PATH="$PATH:$(pwd)/../prebuilts/ninja/linux-x86/"

cp -Rf samples/sdk_licenses ../prebuilts/sdk/licenses

# Use the distribution path given to the script by the build bot in DIST_DIR. Otherwise,
# build in the default location.
if [[ -z $DIST_DIR ]]
then
    dist_dir=$(pwd)/../dist
else
    dist_dir=$DIST_DIR
fi

if [ "$(uname)" == "Darwin" ]; then
    : # Do nothing but skip the next condition so we don't get a bash warning on macos
elif [ "$(expr substr $(uname -s) 1 5)" == "Linux" ]; then
    # Do only for GNU/Linux platform
    export JAVA_HOME=$(pwd)/../prebuilts/jdk/jdk11/linux-x86
fi

## Build the Game SDK distribution zip and the zips for Maven AARs
if [[ $1 == "full" ]]
then
    package_name=fullsdk
    ./gradlew packageZip -Plibraries=swappy,tuningfork,game_activity,game_text_input,paddleboat,memory_advice -PincludeSampleSources -PincludeSampleArtifacts -PdistPath="$dist_dir" -PpackageName=$package_name
    ./gradlew packageMavenZip -Plibraries=swappy          -PdistPath="$dist_dir" -PpackageName=$package_name
    ./gradlew packageMavenZip -Plibraries=tuningfork      -PdistPath="$dist_dir" -PpackageName=$package_name
    ./gradlew packageMavenZip -Plibraries=game_activity   -PdistPath="$dist_dir" -PpackageName=$package_name
    ./gradlew packageMavenZip -Plibraries=game_text_input -PdistPath="$dist_dir" -PpackageName=$package_name
    ./gradlew packageMavenZip -Plibraries=paddleboat      -PdistPath="$dist_dir" -PpackageName=$package_name
    ./gradlew packageMavenZip -Plibraries=memory_advice   -PdistPath="$dist_dir" -PpackageName=$package_name
    ./gradlew jetpadJson -Plibraries=swappy,tuningfork,game_activity,game_text_input,paddleboat,memory_advice -PdistPath="$dist_dir" -PpackageName=$package_name
elif [[ $1 == "samples" ]]
then
    package_name=gamesdk
    ./gradlew packageZip      -Plibraries=swappy -PincludeSampleSources -PincludeSampleArtifacts -PdistPath="$dist_dir"
    ./gradlew packageMavenZip -Plibraries=swappy -PdistPath="$dist_dir"
elif [[ $1 == "maven-only" ]]
then
    # Only the Maven artifacts for Jetpack
    package_name=gamesdk-maven
    ./gradlew packageMavenZip -Plibraries=swappy          -PdistPath="$dist_dir" -PpackageName=$package_name
    ./gradlew packageMavenZip -Plibraries=tuningfork      -PdistPath="$dist_dir" -PpackageName=$package_name
    ./gradlew packageMavenZip -Plibraries=game_activity   -PdistPath="$dist_dir" -PpackageName=$package_name
    ./gradlew packageMavenZip -Plibraries=game_text_input -PdistPath="$dist_dir" -PpackageName=$package_name
    ./gradlew packageMavenZip -Plibraries=paddleboat      -PdistPath="$dist_dir" -PpackageName=$package_name
    ./gradlew packageMavenZip -Plibraries=memory_advice   -PdistPath="$dist_dir" -PpackageName=$package_name
    ./gradlew jetpadJson -Plibraries=swappy,tuningfork,game_activity,game_text_input,paddleboat,memory_advice -PdistPath="$dist_dir" -PpackageName=$package_name
else
    # The default is to build the express zip
    package_name=gamesdk-express
    ./gradlew packageZip -Plibraries=swappy,tuningfork,game_activity,game_text_input,paddleboat,memory_advice -PincludeSampleSources -PincludeSampleArtifacts -PdistPath="$dist_dir" -PpackageName=$package_name
    ./gradlew packageMavenZip -Plibraries=swappy          -PdistPath="$dist_dir" -PpackageName=$package_name
    ./gradlew packageMavenZip -Plibraries=tuningfork      -PdistPath="$dist_dir" -PpackageName=$package_name
    ./gradlew packageMavenZip -Plibraries=game_activity   -PdistPath="$dist_dir" -PpackageName=$package_name
    ./gradlew packageMavenZip -Plibraries=game_text_input -PdistPath="$dist_dir" -PpackageName=$package_name
    ./gradlew packageMavenZip -Plibraries=paddleboat      -PdistPath="$dist_dir" -PpackageName=$package_name
    ./gradlew packageMavenZip -Plibraries=memory_advice   -PdistPath="$dist_dir" -PpackageName=$package_name
    ./gradlew jetpadJson -Plibraries=swappy,tuningfork,game_activity,game_text_input,paddleboat,memory_advice -PdistPath="$dist_dir" -PpackageName=$package_name
fi

if [[ $1 != "maven-only" ]]
then
    mkdir -p "$dist_dir/$package_name/apks/samples"
    mkdir -p "$dist_dir/$package_name/apks/test"
    pushd ./samples/tuningfork/insightsdemo/
    ./gradlew ":app:assembleDebug"
    popd
    pushd ./test/tuningfork/testapp/
    ./gradlew ":app:assembleDebug"
    popd
    pushd ./samples/tuningfork/experimentsdemo/
    ./gradlew ":app:assembleDebug"
    popd
    cp samples/tuningfork/insightsdemo/app/build/outputs/apk/debug/app-debug.apk \
      "$dist_dir/$package_name/apks/samples/insightsdemo.apk"
    cp samples/tuningfork/experimentsdemo/app/build/outputs/apk/debug/app-debug.apk \
      "$dist_dir/$package_name/apks/samples/experimentsdemo.apk"
    cp test/tuningfork/testapp/app/build/outputs/apk/debug/app-debug.apk \
      "$dist_dir/$package_name/apks/test/tuningforktest.apk"
    cp -r samples/tuningfork $dist_dir/$package_name/
    pushd $dist_dir/$package_name
    if [[ -z "$(ls -1 agdk-libraries-*.zip 2>/dev/null | grep agdk)" ]] ; then
      echo 'Could not find the zip "agdk-libraries-*.zip".'
      exit
    fi
    zip -ur agdk-libraries-*.zip "apks/samples/insightsdemo.apk"
    zip -ur agdk-libraries-*.zip "apks/samples/experimentsdemo.apk"
    zip -ur agdk-libraries-*.zip "apks/test/tuningforktest.apk"
    popd
    zip -ur $dist_dir/$package_name/agdk-libraries-*.zip "samples/tuningfork/insightsdemo/" -x "samples/tuningfork/insightsdemo/.idea/*"\
      "samples/tuningfork/insightsdemo/app/build/*" "samples/tuningfork/insightsdemo/app/.cxx/*"\
      "samples/tuningfork/insightsdemo/.gradle/*"
    zip -ur $dist_dir/$package_name/agdk-libraries-*.zip "games-performance-tuner/tools/validation" -x "games-performance-tuner/tools/validation/.idea/*"\
      "games-performance-tuner/tools/validation/build/*" "games-performance-tuner/tools/validation/.gradle/*"
    zip -ur $dist_dir/$package_name/agdk-libraries-*.zip "third_party/protobuf-3.0.0/"
fi

# Calculate hash of the zip file
pushd "$dist_dir/$package_name"
for ZIPNAME in agdk-libraries-*
do
    if [[ -e $ZIPNAME ]]
    then
        sha256sum $ZIPNAME > $ZIPNAME.sha256
    fi
    break
done
popd

pushd "$dist_dir/$package_name"
# Remove intermediate files that would be very costly to store
rm -rf libs prefab
# Remove other files that we don't care about and are polluting the output
rm -rf external third_party src include samples aar
popd
