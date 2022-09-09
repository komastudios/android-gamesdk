cmake_minimum_required(VERSION 3.4.1)
project(Hogger)

find_package(games-memory-advice REQUIRED CONFIG)
#find_package(games-frame-pacing REQUIRED CONFIG)

set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Werror -Wthread-safety" )
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D _LIBCPP_ENABLE_THREAD_SAFETY_ANNOTATIONS -Os -fPIC" )
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti" )
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DGOOGLE_PROTOBUF_NO_RTTI -DHAVE_PTHREAD")

set( MEMORYADVICE_RESOURCES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../../../test/memoryadvice/memoryadvice/src/main/resources")


file(GLOB TF_MODEL_FILES "${MEMORYADVICE_RESOURCES_DIR}/*.*")
file(COPY ${TF_MODEL_FILES} DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/src/main/assets)
#file(COPY ${DEP_LIB} DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/src/main/assets/jni)

include_directories(../../../../include)
include_directories(../../../common/include ) # Samples Includes


add_library(${CMAKE_PROJECT_NAME}
        SHARED
        src/main/cpp/hogger.cpp
        )

target_link_libraries(${CMAKE_PROJECT_NAME}
        games-memory-advice::memory_advice_static
        #games-frame-pacing::swappy
        android
        GLESv2
        log
        )

#add_custom_command(
#        TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
#        COMMAND ${CMAKE_COMMAND} -E copy
#        ${DEP_LIB}
#        ${CMAKE_CURRENT_SOURCE_DIR}/src/main/jniLibs/${ANDROID_ABI}/libmemory_advice.so)