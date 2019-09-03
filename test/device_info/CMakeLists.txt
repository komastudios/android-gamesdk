set( _MY_DIR ${CMAKE_CURRENT_LIST_DIR})

cmake_minimum_required(VERSION 3.4.1)

# Include Google Test
set(ANDROID_GTEST_DIR "../../../external/googletest")
add_subdirectory("${ANDROID_GTEST_DIR}/googletest"
  googletest-build
)
include_directories( "${ANDROID_GTEST_DIR}/googletest/include" )

# Compile specific parts of device_info to be tested
include_directories( "${_MY_DIR}/../../include" )
include_directories( "${_MY_DIR}/../../src" ) # Allow to include private header files from src

add_executable(device_info_test
  main.cpp
  stream_util.cpp
  ../../src/device_info/core/stream_util.cpp
)

target_link_libraries(device_info_test gtest)
