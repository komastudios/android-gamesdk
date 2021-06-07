#
# Copyright (C) 2021 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     https://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

cmake_minimum_required(VERSION 3.6)

project(paddleboat)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/paddleboat/include)

add_library(paddleboat STATIC
        InternalControllerTable.cpp
        GameController.cpp
        GameControllerDeviceInfo.cpp
        GameControllerLog.cpp
        GameControllerManager.cpp
        GameControllerMappingUtils.cpp
        paddleboat_c.cpp)

target_include_directories(paddleboat PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/paddleboat/include)

target_compile_options(paddleboat
        PRIVATE
        -std=c++17
        -Wall
        -Wextra-semi
        -Wshadow
        -Wshadow-field
        -O0
        "$<$<CONFIG:DEBUG>:-Werror>")

target_link_libraries(paddleboat
        android
        atomic
        log)

set(TEST_SRC_DIR ../../test/cpp)
include(../../test/cpp/CMakeLists.txt)