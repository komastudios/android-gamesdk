cmake_minimum_required(VERSION 3.4.1)

set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Werror -Wthread-safety -D _LIBCPP_ENABLE_THREAD_SAFETY_ANNOTATIONS" )

add_library( androidgamesdk

             SHARED

             src/main/cpp/swappy/ChoreographerFilter.cpp
             src/main/cpp/swappy/EGL.cpp
             src/main/cpp/swappy/Swappy.cpp
             src/main/cpp/swappy-utils/Settings.cpp
             src/main/cpp/swappy-utils/Thread.cpp
             src/main/cpp/Swappy_c.cpp)

include_directories( src/main/cpp )

target_link_libraries( androidgamesdk
                       EGL
                       log )
