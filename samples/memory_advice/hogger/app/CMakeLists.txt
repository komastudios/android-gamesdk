cmake_minimum_required(VERSION 3.4.1)

set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Werror -Wthread-safety" )
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D _LIBCPP_ENABLE_THREAD_SAFETY_ANNOTATIONS -Os -fPIC" )
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti" )
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DGOOGLE_PROTOBUF_NO_RTTI -DHAVE_PTHREAD")

include("../../../gamesdk.cmake")

# This builds the gamesdk in package/localtf and adds a gamesdk static library target
add_gamesdk_target(
        DO_LOCAL_BUILD
        ROOT_DIR "../../../.."
        PACKAGE_DIR "../../../../../package/local"
        LIBRARIES "memory_advice"
)

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
