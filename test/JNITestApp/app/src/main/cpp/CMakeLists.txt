# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

add_library( # Sets the name of the library.
        native-lib

        # Sets the library as a shared library.
        SHARED

        # Provides a relative path to your source file(s).
        native-lib.cpp

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

find_library( # Sets the name of the path variable.
        log-lib

        # Specifies the name of the NDK library that
        # you want CMake to locate.
        log)


target_link_libraries( # Specifies the target library.
        native-lib

        android
        # Links the target library to the log library
        # included in the NDK.
        ${log-lib}
        )