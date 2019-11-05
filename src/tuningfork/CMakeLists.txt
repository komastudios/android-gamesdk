cmake_minimum_required(VERSION 3.4.1)

include("../protobuf/protobuf.cmake")
set( MODPB64_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../../external/modp_b64")
set( THIRDPARTY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../third_party")

set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Werror -Wthread-safety" )
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D _LIBCPP_ENABLE_THREAD_SAFETY_ANNOTATIONS -O3 -fPIC" )
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-exceptions" )
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti" )
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ffunction-sections -fdata-sections" )
set( CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -g0")

if (${CMAKE_BUILD_TYPE} STREQUAL "Release")
set( CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--gc-sections,-s")
endif (${CMAKE_BUILD_TYPE} STREQUAL "Release")

set( CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--hash-style=both" )

protobuf_generate_nano_c( ${CMAKE_CURRENT_SOURCE_DIR}/proto
  proto/tuningfork.proto)

protobuf_generate_nano_c( ${CMAKE_CURRENT_SOURCE_DIR}/proto
  proto/descriptor.proto)

include_directories(${PROTOBUF_NANO_SRC_DIR})
include_directories( ../../include )
include_directories( ../../src/swappy )
include_directories( ../common )
include_directories( ${MODPB64_DIR}/modp_b64)
include_directories( ${THIRDPARTY_DIR} )
include_directories( ${THIRDPARTY_DIR}/date/include )

include_directories(${PROTO_GENS_DIR})

# Needed for descriptor.pb.cc
add_definitions(-DPB_FIELD_16BIT)

set( TUNINGFORK_SRCS
  crash_handler.cpp
  histogram.cpp
  prong.cpp
  uploadthread.cpp
  tuningfork.cpp
  tuningfork_c.cpp
  protobuf_util.cpp
  annotation_util.cpp
  tuningfork_extra.cpp
  tuningfork_utils.cpp
  tuningfork_settings.cpp
  tuningfork_swappy.cpp
  unity_tuningfork.cpp
  fpdownload.cpp
  ge_backend.cpp
  ge_serializer.cpp
  file_cache.cpp
  web.cpp
  jni_helper.cpp
  jni_wrap.cpp
  jnictx.cpp
  ${THIRDPARTY_DIR}/json11/json11.cpp
  ${MODPB64_DIR}/modp_b64.cc
  ${PROTO_GENS_DIR}/nano/tuningfork.pb.c
  ${PROTO_GENS_DIR}/nano/descriptor.pb.c)

add_library( tuningfork_static
  STATIC ${TUNINGFORK_SRCS} ${PROTOBUF_NANO_SRCS})
set_target_properties( tuningfork_static PROPERTIES
  COMPILE_OPTIONS "-DPROTOBUF_NANO" )

function(build_shared libname version)
  add_library( ${libname}
    SHARED ${TUNINGFORK_SRCS} ${PROTOBUF_NANO_SRCS})
  set_target_properties( ${libname}  PROPERTIES COMPILE_OPTIONS "-DPROTOBUF_NANO" )
  target_link_libraries( ${libname}
    android
    GLESv2
    log)
  set_link_options(${libname} ${version})
endfunction()

build_shared( tuningfork "tuningfork/version.script")
build_shared( unitytuningfork "tuningfork/unity_version.script")