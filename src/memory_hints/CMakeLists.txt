cmake_minimum_required(VERSION 3.4.1)

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

include_directories( . )
include_directories( ../../include )
include_directories( ../common )

set(MEMORY_HINTS_SRCS
  c_header_check.c
  memory_hints.cpp
)

add_library(memory_hints_static STATIC ${MEMORY_HINTS_SRCS})
add_library(memory_hints SHARED ${MEMORY_HINTS_SRCS})
target_link_libraries(memory_hints
  android
  log
)
