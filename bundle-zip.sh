#!/bin/bash
# Gamesdk bundle zip script
# Usage:
# ./bundle-zip.sh
#   Bundles all the latest versions listed in the file into a zip file
#
# Make an update to the header section and commit to trigger a build on ab.
# Only use the versions already published on maven.

set -e

# Header section - version selector
ZIP_VERSION=2023.1.0.0
GAME_ACTIVITY_VERSION=2.0.0
GAME_CONTROLLER_VERSION=2.0.0
GAME_TEXT_INPUT_VERSION=2.0.0
GAMES_FRAME_PACING_VERSION=2.0.0
GAMES_MEMORY_ADVICE_VERSION=2.0.0-beta01
GAMES_PERFORMANCE_TUNER_VERSION=2.0.0-alpha03
OBOE_VERSION=1.7.0

# Setup the library name dictionary
declare -A agdk_libs
agdk_libs=( ["game-activity"]="games-activity-${GAME_ACTIVITY_VERSION}"
            ["paddleboat"]="games-controller-${GAME_CONTROLLER_VERSION}"
            ["game-text-input"]="games-text-input-${GAME_TEXT_INPUT_VERSION}"
            ["swappy"]="games-frame-pacing-${GAMES_FRAME_PACING_VERSION}"
            ["tuningfork"]="games-performance-tuner-${GAMES_PERFORMANCE_TUNER_VERSION}"
            ["memory_advice"]="games-memory-advice-${GAMES_MEMORY_ADVICE_VERSION}"
            ["oboe"]="oboe-${OBOE_VERSION}"
          )

# Set up the defaults
#
# Use the distribution path given to the script by the build bot in DIST_DIR. Otherwise,
# build in the default location.
if [[ -z $DIST_DIR ]]
then
    dist_dir=$(pwd)/../dist
else
    dist_dir=$DIST_DIR
fi

#Using a similar structure as legacy to avoid a lot of changes
PACKAGE_NAME=fullsdk
WORKING_DIR="$dist_dir/$PACKAGE_NAME"
ZIP_NAME="agdk-libraries-$ZIP_VERSION.zip"


if [ -d "$WORKING_DIR" ]; then rm -Rf $WORKING_DIR; fi
mkdir -p $WORKING_DIR

# First copy the standard files
cp VERSIONS $WORKING_DIR
cp THIRD_PARTY_NOTICES $WORKING_DIR
cp RELEASE_NOTES $WORKING_DIR
cp README.md $WORKING_DIR
cp LICENSE $WORKING_DIR

pushd $WORKING_DIR

mkdir -p include

declare -a abi_array=("arm64-v8a"
                      "armeabi-v7a"
                      "x86"
                      "x86_64"
                      )
#Create abi specific libs dirs
for abi in "${abi_array[@]}"
do
  mkdir -p "libs/${abi}_cpp_shared_Release"
  mkdir -p "libs/${abi}_cpp_static_Release"
done
# Fetch the AARs, this section is expanded on purpose for easy maintenance.
wget "https://dl.google.com/android/maven2/androidx/games/games-activity/$GAME_ACTIVITY_VERSION/games-activity-$GAME_ACTIVITY_VERSION.aar"

wget "https://dl.google.com/android/maven2/androidx/games/games-controller/$GAME_CONTROLLER_VERSION/games-controller-$GAME_CONTROLLER_VERSION.aar"

wget "https://dl.google.com/android/maven2/androidx/games/games-frame-pacing/$GAMES_FRAME_PACING_VERSION/games-frame-pacing-$GAMES_FRAME_PACING_VERSION.aar"

wget "https://dl.google.com/android/maven2/androidx/games/games-memory-advice/$GAMES_MEMORY_ADVICE_VERSION/games-memory-advice-$GAMES_MEMORY_ADVICE_VERSION.aar"

wget "https://dl.google.com/android/maven2/androidx/games/games-performance-tuner/$GAMES_PERFORMANCE_TUNER_VERSION/games-performance-tuner-$GAMES_PERFORMANCE_TUNER_VERSION.aar"

wget "https://dl.google.com/android/maven2/androidx/games/games-text-input/$GAME_TEXT_INPUT_VERSION/games-text-input-$GAME_TEXT_INPUT_VERSION.aar"

wget "https://github.com/google/oboe/releases/download/$OBOE_VERSION/oboe-$OBOE_VERSION.aar"

# Process each library.
for lib_name in "${!agdk_libs[@]}";
do
  aar_name=${agdk_libs[$lib_name]}.aar
  # First unzip
  mkdir -p $lib_name
  unzip $aar_name -d $lib_name
  # Copy include directories to include/

  rsync -av --ignore-missing-args $lib_name/prefab/modules/$lib_name/include/* include/


  for abi in "${abi_array[@]}"
  do
  # Copy shared libs to libs/ABI_shared_Release
    rsync -av --ignore-missing-args $lib_name/prefab/modules/$lib_name/libs/android.$abi/*.so libs/${abi}_cpp_shared_Release/
    rsync -av --ignore-missing-args $lib_name/prefab/modules/$lib_name/libs/android.$abi/*.a libs/${abi}_cpp_shared_Release/
  # Copy static libs to libs/ABI_static_Release except for oboe
    if [ "$lib_name" != "oboe" ]; then
      rsync -av --ignore-missing-args $lib_name/prefab/modules/${lib_name}_static/libs/android.$abi/*.a libs/${abi}_cpp_static_Release/
    fi
  done
  # Copy class.jar to jar-class/$lib_name/
  mkdir -p jar-classes/$lib_name
  rsync -av --ignore-missing-args $lib_name/classes.jar jar-classes/$lib_name/
  rm -rf $lib_name
done

# Create the bundle with AARs first
zip $ZIP_NAME *.aar

# Add the version/release_notes/readme/license files
zip -ur $ZIP_NAME VERSIONS
zip -ur $ZIP_NAME THIRD_PARTY_NOTICES
zip -ur $ZIP_NAME RELEASE_NOTES
zip -ur $ZIP_NAME README.md
zip -ur $ZIP_NAME LICENSE


# Add the include directory
zip -ur $ZIP_NAME include

# Add the libs directory
zip -ur $ZIP_NAME libs

# Add the jar-classes directory
zip -ur $ZIP_NAME jar-classes
popd
