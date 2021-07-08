cmake_minimum_required(VERSION 3.4.1)

include_directories(src/main/cpp)

# Find the Game Text Input package, exposed via Prefab.
find_package(game-text-input REQUIRED CONFIG)

add_library(native-lib SHARED
            src/main/cpp/native-lib.cpp
            )

find_library(log-lib log)
target_link_libraries(native-lib
        game-text-input::game-text-input
        ${log-lib})