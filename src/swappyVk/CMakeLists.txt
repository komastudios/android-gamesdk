cmake_minimum_required(VERSION 3.4.1)

set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-exceptions -fno-rtti -std=c++17 -Werror -Wthread-safety -D _LIBCPP_ENABLE_THREAD_SAFETY_ANNOTATIONS -Os -fPIC" )

include_directories( ../../includes )

message( STATUS "Building swappyVk_static to ${CMAKE_CURRENT_BINARY_DIR}/build" )
add_library( swappyVk_static

             STATIC

             SwappyVk.cpp
             ${ANDROID_NDK}/sources/third_party/vulkan/src/common/vulkan_wrapper.cpp

             # Add new source files here
             )

set_target_properties( swappyVk_static PROPERTIES
     LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/build )

add_library( swappyVk

             SHARED

             SwappyVk.cpp
             ${ANDROID_NDK}/sources/third_party/vulkan/src/common/vulkan_wrapper.cpp
           )


 target_link_libraries( swappyVk

                        swappyVk_static
                        android
                        log )
