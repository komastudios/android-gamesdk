cmake_minimum_required(VERSION 3.4.1)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99  -Werror -DVK_USE_PLATFORM_ANDROID_KHR")

# ============== Games SDK

include_directories( ../../../includes ) # Games SDK Public Includes
include_directories( ../../../src ) # Games SDK Internal Includes

# add_subdirectory(../../../src/swappy ./swappy-output)   # Swappy Library

# ============== cube

# Build Util Lib
set(UTILS_NAME  vsamputils)
set(GLSLANG_SPIRV_INCLUDE_DIR ${SHADERC_SRC}/third_party/glslang)
get_filename_component(PROJECT_SOURCE_DIR
                       "${CMAKE_SOURCE_DIR}/../../../.."
                       ABSOLUTE)

# Import shaderc lib in NDK
add_library(shaderc_lib STATIC IMPORTED)
set_target_properties(shaderc_lib PROPERTIES IMPORTED_LOCATION
                         ${SHADERC_SRC}/libs/${ANDROID_STL}/${ANDROID_ABI}/libshaderc.a)

find_program(GLSC names glslc HINTS "${ANDROID_NDK}/shader-tools/*")

execute_process(
        COMMAND ${GLSC} -mfmt=num src/main/cpp/cube.vert -o src/main/cpp/include/cube.vert.inc
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        RESULT_VARIABLE result
        )

if(NOT "${result}" STREQUAL "0")
message(FATAL_ERROR "Failed to compile cube.vert: ${result}")
endif()

execute_process(
        COMMAND ${GLSC} -mfmt=num src/main/cpp/cube.frag -o src/main/cpp/include/cube.frag.inc
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        RESULT_VARIABLE result
)

if(NOT "${result}" STREQUAL "0")
        message(FATAL_ERROR "Failed to compile cube.frag: ${result}")
endif()

# Force export ANativeActivity_onCreate(),
# Refer to: https://github.com/android-ndk/ndk/issues/381.
set(CMAKE_SHARED_LINKER_FLAGS
    "${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate")

include_directories( src/main/cpp )

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99  -Werror -DVK_USE_PLATFORM_ANDROID_KHR")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11  -Werror -DVK_USE_PLATFORM_ANDROID_KHR")

add_library( native-lib

             SHARED

             ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c
             src/main/cpp/cube.c
             src/main/cpp/common/android_util.cpp
             src/main/cpp/common/vulkan_wrapper.cpp
        )

target_include_directories(native-lib

                           PRIVATE

                           ${SHADERC_SRC}/third_party/spirv-tools/include
                           ${SHADERC_SRC}/third_party/spirv-tools/include/spirv-tools
                           src/main/cpp/include
                           src/main/cpp/common
                           ${ANDROID_NDK}/sources/android/native_app_glue)


target_link_libraries( native-lib

                       android
                       EGL
                       GLESv2
                       log
                       )


