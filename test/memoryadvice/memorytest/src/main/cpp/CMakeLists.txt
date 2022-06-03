# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)
project(memory-test)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

add_library( ${PROJECT_NAME} SHARED
        allocator.cc
        consumer.cc
        gl-memory.cc
        gl-utils.cc
        memory.cc
        mmap-allocator.cc
        mmap.cc
        test-renderer.cc
        test-vulkan-renderer.cc
        vk-memory.cc
        vulkan_wrapper/vulkan_wrapper.cpp

        memory-test.cc)

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( ${PROJECT_NAME}
        android
        GLESv3
        log)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVK_USE_PLATFORM_ANDROID_KHR")
