#
# Copyright (C) The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

cmake_minimum_required(VERSION 3.4.1)
#set(CMAKE_CXX_STANDARD 17)

# Export GameActivity_onCreate(),
# Refer to: https://github.com/android-ndk/ndk/issues/381.
set(CMAKE_SHARED_LINKER_FLAGS
    "${CMAKE_SHARED_LINKER_FLAGS} -u GameActivity_onCreate")

# Set common compiler options
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++14 -Wall -Werror")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror")
add_definitions("-DGLM_FORCE_SIZE_T_LENGTH -DGLM_FORCE_RADIANS")

# Import the CMakeLists.txt for the glm library
add_subdirectory(glm)

# Find the GameActivity package, exposd via Prefab.
find_package(game-activity REQUIRED CONFIG)

# now build app's shared lib
add_library(game SHARED
     android_main.cpp
     anim.cpp
     ascii_to_geom.cpp
     dialog_scene.cpp
     game_activity_included.cpp
     game_input_included.cpp
     indexbuf.cpp
     input_util.cpp
     jni_util.cpp
     native_app_glue_included.c
     native_engine.cpp
     obstacle.cpp
     obstacle_generator.cpp
     our_shader.cpp
     play_scene.cpp
     scene.cpp
     scene_manager.cpp
     sfxman.cpp
     shader.cpp
     shape_renderer.cpp
     tex_quad.cpp
     text_renderer.cpp
     texture.cpp
     ui_scene.cpp
     util.cpp
     vertexbuf.cpp
     welcome_scene.cpp
     )

target_include_directories(game PRIVATE
     ${CMAKE_CURRENT_SOURCE_DIR}
     ${CMAKE_CURRENT_SOURCE_DIR}/data)

# add lib dependencies
target_link_libraries(game
     game-activity::game-activity
     android
     atomic
     EGL
     GLESv2
     glm
     log
     OpenSLES)
