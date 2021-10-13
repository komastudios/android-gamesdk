cmake_minimum_required(VERSION 3.18.1)
project(tuningfork C CXX)
set(CMAKE_CXX_STANDARD 14)

option(TUNINGFORK_TEST_OPTION "Set TUNINGFORK_TEST C++ flag")

include("../protobuf/protobuf.cmake")
set( MODPB64_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../../external/modp_b64")
set( THIRDPARTY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../third_party")

set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -Wthread-safety" )
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D _LIBCPP_ENABLE_THREAD_SAFETY_ANNOTATIONS -O3 -fPIC" )
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-exceptions" )
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti" )
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ffunction-sections -fdata-sections" )
set( CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -g0")

if (${TUNINGFORK_TEST_OPTION})
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DTUNINGFORK_TEST" )
endif (${TUNINGFORK_TEST_OPTION})

if (${CMAKE_BUILD_TYPE} STREQUAL "Release")
set( CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--gc-sections,-s")
endif (${CMAKE_BUILD_TYPE} STREQUAL "Release")

set( CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--hash-style=both" )

protobuf_generate_nano_c( ${CMAKE_CURRENT_SOURCE_DIR}/proto
  proto/tuningfork.proto)

protobuf_generate_nano_c( ${CMAKE_CURRENT_SOURCE_DIR}/proto
  proto/descriptor.proto)

include_directories( . )
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
  c_header_check.c
  core/activity_lifecycle_state.cpp
  core/annotation_map.cpp
  core/annotation_util.cpp
  core/async_telemetry.cpp
  core/battery_reporting_task.cpp
  core/battery_provider.cpp
  core/chrono_time_provider.cpp
  core/crash_handler.cpp
  core/file_cache.cpp
  core/frametime_metric.cpp
  core/loadingtime_metric.cpp
  core/memory_telemetry.cpp
  core/protobuf_util_internal.cpp
  core/request_info.cpp
  core/runnable.cpp
  core/session.cpp
  core/thermal_reporting_task.cpp
  core/tuningfork.cpp
  core/tuningfork_c.cpp
  core/tuningfork_extra.cpp
  core/tuningfork_impl.cpp
  core/tuningfork_settings.cpp
  core/tuningfork_swappy.cpp
  core/tuningfork_utils.cpp
  core/uploadthread.cpp
  http_backend/debugInfo.cpp
  http_backend/generateTuningParameters.cpp
  http_backend/http_backend.cpp
  http_backend/http_request.cpp
  http_backend/json_serializer.cpp
  http_backend/ultimate_uploader.cpp
  ../common/jni/jni_helper.cpp
  ../common/jni/jni_wrap.cpp
  ../common/jni/jnictx.cpp
  ../common/system_utils.cpp
  proto/protobuf_util.cpp
  unity/unity_tuningfork.cpp
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
    log)
  set_link_options(${libname} ${version})
endfunction()

build_shared( tuningfork "${CMAKE_CURRENT_LIST_DIR}/version.script")
build_shared( unitytuningfork "${CMAKE_CURRENT_LIST_DIR}/unity_version.script")
