cmake_minimum_required(VERSION 3.4.1)

if (GAMESDK_BUILD_SWAPPY)
  add_subdirectory( ./swappy )
endif()
if (GAMESDK_BUILD_TUNINGFORK)
  add_subdirectory( ./tuningfork )
endif()
