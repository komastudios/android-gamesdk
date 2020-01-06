cmake_minimum_required(VERSION 3.4.1)

add_subdirectory( ./swappy )
add_subdirectory( ./device_info )
if (GAMESDK_BUILD_TUNINGFORK)
  add_subdirectory( ./tuningfork )
endif()
