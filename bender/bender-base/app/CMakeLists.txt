cmake_minimum_required(VERSION 3.4.1)

# build native_app_glue as a static lib
set(APP_GLUE_DIR ${ANDROID_NDK}/sources/android/native_app_glue)
include_directories(${APP_GLUE_DIR})
add_library(app-glue STATIC ${APP_GLUE_DIR}/android_native_app_glue.c)

# incorporate glm
include_directories(../external)

# build vulkan app
set(SRC_DIR src/main/jni)
set(KIT_DIR ../utils/bender_kit)
set(WRAPPER_DIR ${KIT_DIR}/vulkan_wrapper)
set(UTILS_DIR ../utils/src)

include_directories(${WRAPPER_DIR})
include_directories(${KIT_DIR})
include_directories(${UTILS_DIR})

add_library(bender-kit STATIC
        ${KIT_DIR}/bender_kit.cc
        ${KIT_DIR}/debug_marker.cc
        ${WRAPPER_DIR}/vulkan_wrapper.cc)

add_library(base SHARED
        ${SRC_DIR}/vulkan_main.cc
        ${SRC_DIR}/android_main.cc
        ${SRC_DIR}/renderer.cc
        ${SRC_DIR}/shader_state.cc
        ${SRC_DIR}/geometry.cc
        ${SRC_DIR}/uniform_buffer.cc
        )

add_library(utils STATIC
        ${UTILS_DIR}/bender_helpers.cc
        ${UTILS_DIR}/bender_textures.cc)

target_link_libraries(base bender-kit)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wall -Werror \
                     -DVK_USE_PLATFORM_ANDROID_KHR -DENABLE_VALIDATION_LAYERS")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate")
target_link_libraries(base app-glue utils log android)