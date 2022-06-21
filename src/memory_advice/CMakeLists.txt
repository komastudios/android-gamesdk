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
set( XNNPACK_SOURCE_DIR "${EXTERNAL_SOURCE_DIR}/xnnpack")
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

function(bundle_static_library tgt_name bundled_tgt_name)
  list(APPEND static_libs ${tgt_name})

  function(_recursively_collect_dependencies input_target)
    set(_input_link_libraries LINK_LIBRARIES)
    get_target_property(_input_type ${input_target} TYPE)
    if (${_input_type} STREQUAL "INTERFACE_LIBRARY")
      set(_input_link_libraries INTERFACE_LINK_LIBRARIES)
    endif()
    get_target_property(public_dependencies ${input_target} ${_input_link_libraries})
    foreach(dependency IN LISTS public_dependencies)
      if(TARGET ${dependency})
        get_target_property(alias ${dependency} ALIASED_TARGET)
        if (TARGET ${alias})
          set(dependency ${alias})
        endif()
        get_target_property(_type ${dependency} TYPE)
        if (${_type} STREQUAL "STATIC_LIBRARY")
          list(APPEND static_libs ${dependency})
        endif()

        get_property(library_already_added
          GLOBAL PROPERTY _${tgt_name}_static_bundle_${dependency})
        if (NOT library_already_added)
          set_property(GLOBAL PROPERTY _${tgt_name}_static_bundle_${dependency} ON)
          _recursively_collect_dependencies(${dependency})
        endif()
      endif()
    endforeach()
    set(static_libs ${static_libs} PARENT_SCOPE)
  endfunction()

  _recursively_collect_dependencies(${tgt_name})

  list(REMOVE_DUPLICATES static_libs)

  set(bundled_tgt_full_name
    ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/${CMAKE_STATIC_LIBRARY_PREFIX}${bundled_tgt_name}${CMAKE_STATIC_LIBRARY_SUFFIX})

  if (CMAKE_CXX_COMPILER_ID MATCHES "^(Clang|GNU)$")
    file(WRITE ${CMAKE_BINARY_DIR}/${bundled_tgt_name}.ar.in
      "CREATE ${bundled_tgt_full_name}\n" )

    foreach(tgt IN LISTS static_libs)
      file(APPEND ${CMAKE_BINARY_DIR}/${bundled_tgt_name}.ar.in
        "ADDLIB $<TARGET_FILE:${tgt}>\n")
    endforeach()

    file(APPEND ${CMAKE_BINARY_DIR}/${bundled_tgt_name}.ar.in "SAVE\n")
    file(APPEND ${CMAKE_BINARY_DIR}/${bundled_tgt_name}.ar.in "END\n")

    file(GENERATE
      OUTPUT ${CMAKE_BINARY_DIR}/${bundled_tgt_name}.ar
      INPUT ${CMAKE_BINARY_DIR}/${bundled_tgt_name}.ar.in)

    set(ar_tool ${CMAKE_AR})
    if (CMAKE_INTERPROCEDURAL_OPTIMIZATION)
      set(ar_tool ${CMAKE_CXX_COMPILER_AR})
    endif()

    add_custom_command(
      COMMAND ${ar_tool} -M < ${CMAKE_BINARY_DIR}/${bundled_tgt_name}.ar
      OUTPUT ${bundled_tgt_full_name}
      COMMENT "Bundling ${bundled_tgt_name}"
      VERBATIM)
  elseif(MSVC)
    find_program(lib_tool lib)

    foreach(tgt IN LISTS static_libs)
      list(APPEND static_libs_full_names $<TARGET_FILE:${tgt}>)
    endforeach()

    add_custom_command(
      COMMAND ${lib_tool} /NOLOGO /OUT:${bundled_tgt_full_name} ${static_libs_full_names}
      OUTPUT ${bundled_tgt_full_name}
      COMMENT "Bundling ${bundled_tgt_name}"
      VERBATIM)
  else()
    message(FATAL_ERROR "Unknown bundle scenario!")
  endif()

  add_custom_target(bundling_target ALL DEPENDS ${bundled_tgt_full_name})
  add_dependencies(bundling_target ${tgt_name})

  add_library(${bundled_tgt_name} STATIC IMPORTED)
  set_target_properties(${bundled_tgt_name}
    PROPERTIES
      IMPORTED_LOCATION ${bundled_tgt_full_name}
      INTERFACE_INCLUDE_DIRECTORIES $<TARGET_PROPERTY:${tgt_name},INTERFACE_INCLUDE_DIRECTORIES>)
  add_dependencies(${bundled_tgt_name} bundling_target)

endfunction()

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

add_library(memory_advice_static_pre STATIC ${MEMORY_ADVICE_SRCS})
add_library(memory_advice SHARED ${MEMORY_ADVICE_SRCS})
target_link_libraries(memory_advice
  PUBLIC
  android
  log
  absl::flags
  tensorflow-lite
)

target_link_libraries(memory_advice_static_pre
  PUBLIC
  android
  log
  absl::flags
  tensorflow-lite
)

bundle_static_library(memory_advice_static_pre memory_advice_static)