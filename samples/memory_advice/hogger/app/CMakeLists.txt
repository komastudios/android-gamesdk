cmake_minimum_required(VERSION 3.4.1)
project(hogger)

set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Werror -Wthread-safety" )
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D _LIBCPP_ENABLE_THREAD_SAFETY_ANNOTATIONS -Os -fPIC" )
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti" )
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DGOOGLE_PROTOBUF_NO_RTTI -DHAVE_PTHREAD")

set( MEMORYADVICE_RESOURCES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../../../test/memoryadvice/memoryadvice/src/main/resources")

include("../../../gamesdk.cmake")

# This builds the memory_advice in package/local and adds a memory_advice shared library target
add_gamesdk_target(
        DO_LOCAL_BUILD
        ROOT_DIR "../../../.."
        PACKAGE_DIR "../../../../../package/local"
        LIBRARIES "memory_advice"
)

#get_target_property(DEP_LIB memory_advice IMPORTED_LOCATION)

file(GLOB TF_MODEL_FILES "${MEMORYADVICE_RESOURCES_DIR}/*.*")
file(COPY ${TF_MODEL_FILES} DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/src/main/assets)

include_directories(../../../../include)
include_directories(../../../common/include ) # Samples Includes


add_library(${CMAKE_PROJECT_NAME}
        SHARED
        src/main/cpp/hogger.cpp
        )

target_link_libraries(${CMAKE_PROJECT_NAME}
        memory_advice
        android
        GLESv2
        log
        )
