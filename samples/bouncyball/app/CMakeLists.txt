cmake_minimum_required(VERSION 3.4.1)

project(bouncyball)

set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Werror -Wthread-safety -D _LIBCPP_ENABLE_THREAD_SAFETY_ANNOTATIONS" )

# ============== Games SDK

# This builds and uses swappy as a static library from a gamesdk package
include("../../gamesdk.cmake")
add_gamesdk_target(
    DO_LOCAL_BUILD
    LIBRARIES "swappy"
    ROOT_DIR "../../.."
    PACKAGE_DIR "../../../../package/local"
)

# Uncomment to add the Game SDK sources as part of the project sources, allowing to develop
# (with auto completions) and debug Swappy from Android Studio using this sample.
#add_gamesdk_sources()

# ============== Bouncy Ball

include_directories( ../../common/include ) # Samples Includes
include_directories( src/main/cpp )

add_library( ${CMAKE_PROJECT_NAME}

             SHARED

             src/main/cpp/Circle.cpp
             src/main/cpp/Orbit.cpp
             src/main/cpp/Renderer.cpp
             src/main/cpp/Settings.cpp
             ../../common/src/Thread.cpp
        )

target_link_libraries( ${CMAKE_PROJECT_NAME}

                       android
                       EGL
                       GLESv2
                       log
                       swappy
                       )
