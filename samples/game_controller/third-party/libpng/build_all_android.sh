# Copyright 2021 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

# Script to build protobuf for multiple Android ABIs and prepare them for distribution
# via Prefab (github.com/google/prefab)
#
# Ensure that ANDROID_NDK environment variable is set to your Android NDK location
# e.g. /Library/Android/sdk/ndk-bundle

#!/bin/bash

LIBPNG_DIR=$1

if [ -z "$ANDROID_NDK" ]; then
  echo "Please set ANDROID_NDK to the Android NDK folder"
  exit 1
fi

echo "Cleaning build folder"
rm -r build/*

# Directories, paths and filenames
BUILD_DIR=build

CMAKE_ARGS="-H. \
  -DBUILD_SHARED_LIBS=true \
  -DCMAKE_BUILD_TYPE=Release \
  -DANDROID_TOOLCHAIN=clang \
  -DANDROID_STL=c++_shared \
  -DCMAKE_TOOLCHAIN_FILE=${ANDROID_NDK}/build/cmake/android.toolchain.cmake \
  -DCMAKE_INSTALL_PREFIX=."

function build_libpng {

  ABI=$1
  MINIMUM_API_LEVEL=$2
  ABI_BUILD_DIR=build/${ABI}
  STAGING_DIR=staging

  echo "Building libpng for ${ABI}"

  mkdir -p ${ABI_BUILD_DIR} ${ABI_BUILD_DIR}/${STAGING_DIR}

  cmake -B${ABI_BUILD_DIR} \
        -DANDROID_ABI=${ABI} \
        -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY=${STAGING_DIR}/lib/${ABI} \
        -DANDROID_PLATFORM=android-${MINIMUM_API_LEVEL}\
        -DLIBPNG_BASE_DIR=${LIBPNG_DIR}\
        -DSKIP_INSTALL_ALL=YES\
        -DHAVE_LD_VERSION_SCRIPT=OFF\
        ${CMAKE_ARGS}

  pushd ${ABI_BUILD_DIR}
  make -j5
  popd
}

build_libpng armeabi-v7a 16
build_libpng arm64-v8a 21
build_libpng x86 16
build_libpng x86_64 21

# Copy the headers
mkdir -p include/
cp "$LIBPNG_DIR/png.h" "include/"
cp "$LIBPNG_DIR/pngconf.h" "include/"
cp "build/arm64-v8a/$LIBPNG_DIR/pnglibconf.h" "include/"

ABIS=("x86" "x86_64" "arm64-v8a" "armeabi-v7a")
# Copy the libraries
for abi in ${ABIS[@]}
do
  echo "Copying the ${abi} libraries"
  mkdir -p "libs/${abi}"
  cp -v "build/${abi}/$LIBPNG_DIR/staging/lib/${abi}/libpng16.a" "libs/${abi}/libpng16.a"
done
