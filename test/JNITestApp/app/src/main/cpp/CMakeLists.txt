# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)
project(JNITestApp)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

add_library(${PROJECT_NAME} SHARED
       jni_test_app.cpp

        ../../../../../../src/tuningfork/jni_helper.cpp
        ../../../../../../src/tuningfork/jni_wrap.cpp
        ../../../../../../src/tuningfork/protobuf_util.cpp
        ../../../../../../src/tuningfork/tuningfork_utils.cpp
        ../../../../../../src/tuningfork/file_cache.cpp
        ../../../../../../third_party/json11/json11.cpp
        )

include_directories(../../../../../../src/tuningfork)
include_directories(../../../../../../src/common)
include_directories(../../../../../../include)
include_directories(../../../../../../third_party)
include_directories(../../../../../../../external/nanopb-c)


target_link_libraries( ${PROJECT_NAME}
        android
        log )