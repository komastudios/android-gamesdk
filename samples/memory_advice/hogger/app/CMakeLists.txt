cmake_minimum_required(VERSION 3.4.1)

# If you have protobuf installed from a different directory, set it here. The source version
#  must match the protoc version.
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Werror -Wthread-safety" )
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D _LIBCPP_ENABLE_THREAD_SAFETY_ANNOTATIONS -Os -fPIC" )
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti" )
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DGOOGLE_PROTOBUF_NO_RTTI -DHAVE_PTHREAD")

set( GAMESDK_BUILD_TUNINGFORK ON)
include("../../../gamesdk.cmake")

# This builds the gamesdk in package/localtf and adds a gamesdk static library target
add_gamesdk_target(
        DO_LOCAL_BUILD
        ROOT_DIR "../../../.."
        PACKAGE_DIR "../../../../../package/local"
        LIBRARIES "swappy,tuningfork"
)

# Uncomment to add the Game SDK sources as part of the project sources, allowing to develop
# (with auto completions) and debug Tuning Fork from Android Studio using this sample.
#add_gamesdk_sources()


include_directories(${PROTO_GENS_DIR})
include_directories(${PROTOBUF_SRC_DIR})
include_directories(${PROTOBUF_NANO_SRC_DIR})
include_directories(../../common)
include_directories(../../../common/include)
include_directories(../../../../include)

add_library( native-lib
        SHARED
        src/main/cpp/native-lib.cpp
        )

target_link_libraries( native-lib
        gamesdk
        android
        EGL
        GLESv2
        log
        )
