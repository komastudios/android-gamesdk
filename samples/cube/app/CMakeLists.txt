cmake_minimum_required(VERSION 3.4.1)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99  -Werror -DVK_USE_PLATFORM_ANDROID_KHR")

# ============== Games SDK

# This builds and uses swappy as a static library from a gamesdk package
include("../../gamesdk.cmake")
add_gamesdk_target("package/local" "localBuild")

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

foreach(shader vert frag)
    add_custom_command(
            COMMENT "Compiling ${shader}"
            OUTPUT cube.${shader}.inc
            COMMAND ${GLSC} -mfmt=num ../../../third_party/cube/app/src/main/cpp/cube.${shader} -o ../../../third_party/cube/app/src/main/cpp/include/cube.${shader}.inc
            MAIN_DEPENDENCY ../../../third_party/cube/app/src/main/cpp/include/cube.${shader}.inc
            WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    )
endforeach()

include_directories( src/main/cpp ../../../third_party/cube/app/src/main/cpp )

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -Werror -DVK_USE_PLATFORM_ANDROID_KHR")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14  -Werror -DVK_USE_PLATFORM_ANDROID_KHR")

add_library( native-lib

             SHARED

             ../../../third_party/cube/app/src/main/cpp/include/cube.vert.inc
             ../../../third_party/cube/app/src/main/cpp/include/cube.frag.inc
             ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c
             src/main/cpp/native-lib.cpp
             ../../../third_party/cube/app/src/main/cpp/common/android_util.cpp
             ../../../third_party/cube/app/src/main/cpp/common/vulkan_wrapper.cpp
        )

target_include_directories(native-lib

                           PRIVATE

                           ${SHADERC_SRC}/third_party/spirv-tools/include
                           ${SHADERC_SRC}/third_party/spirv-tools/include/spirv-tools
                           ../../../third_party/cube/app/src/main/cpp/common
                           ../../../third_party/cube/app/src/main/cpp/include
                           ${ANDROID_NDK}/sources/android/native_app_glue)


target_link_libraries( native-lib

                       gamesdk
                       android
                       log
                       )
