cmake_minimum_required(VERSION 3.18.1)
project(memory-advice C CXX)
set(CMAKE_CXX_STANDARD 14)
set(IgnoreOldToolchainWarning "${ANDROID_UNIFIED_HEADERS}")

include("../../samples/gamesdk.cmake")

set( MEMORYADVICE_ASSETS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../test/memoryadvice/memoryadvice/src/main/assets/memoryadvice")
set( MEMORYADVICE_RESOURCES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../test/memoryadvice/memoryadvice/src/main/resources")
set( THIRD_PARTY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../third_party")
set( TENSORFLOW_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../../external/tensorflow")
set( EXTERNAL_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../../external")
set( TFLITE_FLATBUFFERS_SCHEMA_DIR "${CMAKE_CURRENT_BINARY_DIR}/../flatbuffers/include")

set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -Wthread-safety" )
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D _LIBCPP_ENABLE_THREAD_SAFETY_ANNOTATIONS -O3 -fPIC" )
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-exceptions" )
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti" )
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ffunction-sections -fdata-sections" )
set( CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -g0")

set( CPUINFO_SOURCE_DIR "${EXTERNAL_SOURCE_DIR}/cpuinfo")
set( CLOG_SOURCE_DIR "${EXTERNAL_SOURCE_DIR}/cpuinfo")
set( PTHREADPOOL_SOURCE_DIR "${EXTERNAL_SOURCE_DIR}/pthreadpool")
set( FXDIV_SOURCE_DIR "${EXTERNAL_SOURCE_DIR}/FXdiv")
set( FP16_SOURCE_DIR "${EXTERNAL_SOURCE_DIR}/FP16")
set( PSIMD_SOURCE_DIR "${EXTERNAL_SOURCE_DIR}/psimd")
set( XNNPACK_SOURCE_DIR "${EXTERNAL_SOURCE_DIR}/XNNPACK")
set( EIGEN_SOURCE_DIR "${EXTERNAL_SOURCE_DIR}/eigen")

if (${CMAKE_BUILD_TYPE} STREQUAL "Release")
set( CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--gc-sections,-s")
endif (${CMAKE_BUILD_TYPE} STREQUAL "Release")
set( CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--hash-style=both" )

file(READ "${MEMORYADVICE_ASSETS_DIR}/default.json" PARAMS_FILE)
set(PARAMS_STRING "namespace memory_advice {\nconst char* parameters_string = R\"PARAMS(\n")
string(APPEND PARAMS_STRING "${PARAMS_FILE}")
string(APPEND PARAMS_STRING "\n )PARAMS\";\n}\n")
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/advisor_parameters.cpp" "${PARAMS_STRING}")

include_directories( . )
include_directories( ../../include )
include_directories( ../common )
include_directories( ${CMAKE_CURRENT_BINARY_DIR} )
include_directories( ${THIRD_PARTY_DIR} )

add_subdirectory(
  "${EXTERNAL_SOURCE_DIR}/abseil-cpp"
  "${CMAKE_CURRENT_BINARY_DIR}/abseil-cpp"
  EXCLUDE_FROM_ALL
)

set(TFLITE_INCLUDE_DIRS
  "${TENSORFLOW_SOURCE_DIR}"
  "${TFLITE_FLATBUFFERS_SCHEMA_DIR}"
)
include_directories(
  BEFORE
  ${TFLITE_INCLUDE_DIRS}
)

add_subdirectory(
  "${TENSORFLOW_SOURCE_DIR}/tensorflow/lite"
  "${CMAKE_CURRENT_BINARY_DIR}/tensorflow-lite"
  EXCLUDE_FROM_ALL
)


set(MEMORY_ADVICE_SRCS
  c_header_check.c
  core/memory_advice.cpp
  core/memory_advice_impl.cpp
  core/memory_advice_c.cpp
  core/memory_advice_utils.cpp
  core/metrics_provider.cpp
  core/state_watcher.cpp
  core/predictor.cpp
  test/basic.cpp
  ../common/jni/jni_helper.cpp
  ../common/jni/jni_wrap.cpp
  ../common/jni/jnictx.cpp
  ../common/apk_utils.cpp
  ../common/system_utils.cpp
  ${THIRD_PARTY_DIR}/json11/json11.cpp
  advisor_parameters.cpp
  ${MEMORYADVICE_RESOURCES_DIR}/realtime.tflite
  ${MEMORYADVICE_RESOURCES_DIR}/oom_features.json
)

add_library(memory_advice_static STATIC ${MEMORY_ADVICE_SRCS})
add_library(memory_advice SHARED ${MEMORY_ADVICE_SRCS})
target_link_libraries(memory_advice
  PUBLIC
  android
  log
  absl::flags
  tensorflow-lite
)

target_link_libraries(memory_advice_static
  PUBLIC
  android
  log
  absl::flags
  tensorflow-lite
)
