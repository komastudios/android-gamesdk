cmake_minimum_required(VERSION 3.10.2)

get_filename_component(CppDir ${CMAKE_CURRENT_SOURCE_DIR} ABSOLUTE)
get_filename_component(LibDir ${CMAKE_CURRENT_SOURCE_DIR}/../../../../common/3rd_party ABSOLUTE)
get_filename_component(ShadercDir ${ANDROID_NDK}/sources/third_party/shaderc ABSOLUTE)
get_filename_component(GTestDir ${ANDROID_NDK}/sources/third_party/googletest ABSOLUTE)
get_filename_component(CpuFeaturesDir ${ANDROID_NDK}/sources/android/cpufeatures ABSOLUTE)

#===================================================================================================


project(native-lib LANGUAGES C CXX)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -fPIC")

# TODO: Need to see about getting the GameSDK to support Windows.
#  The one definite build issue is a phase that uses ar to merge the various components into a
#  single gamesdk lib. There may be other issues in code, but since we're theoretically building
#  for Android and not Windows/Mac/etc., that seems like it shouldn't be an issue.

add_compile_options("-frtti")

find_library(log-lib
        log)

#===================================================================================================

add_definitions(-DVK_USE_PLATFORM_ANDROID_KHR)

add_library(${PROJECT_NAME} SHARED
        ${ANDROID_NDK}/sources/third_party/vulkan/src/common/vulkan_wrapper.cpp
        # Java-Native glue & super-fundamental code
        native-lib.cpp

        # Framework & utilities
        ancer/BaseOperation.cpp
        ancer/Reporting.cpp
        ancer/Suite.cpp
        ancer/System.cpp
        ancer/util/Basics.cpp
        ancer/util/Bitmath.cpp
        ancer/util/GLHelpers.cpp
        ancer/util/GLPixelBuffer.cpp
        ancer/util/JNIHelpers.cpp
        ancer/util/ThreadSyncPoint.cpp
        ancer/util/Time.cpp

        # Operations
        operations/CalculatePIOperation.cpp
        operations/IOPerformanceOperation.cpp
        operations/MemoryAccessOperation.cpp
        operations/CalculateWaitPIOperation.cpp

        operations/MemoryAllocOperation.cpp
        operations/MonitorOperation.cpp
        operations/ScheduleAffinityOperation.cpp
        operations/ThreadSchedulingOperation.cpp


        # integration testing
        operations/integration/FakeOperation.cpp


        # some external deps
        ${CpuFeaturesDir}/cpu-features.c
        )

target_include_directories(${PROJECT_NAME} PUBLIC
        include
        ${CppDir}
        ${LibDir}/json/include
        ${LibDir}/glm/
        ${ANDROID_NDK}/sources/third_party/vulkan/src/common
        ${ANDROID_NDK}/sources/android/native_app_glue
        ${ANDROID_NDK}/sources/third_party/shaderc/include
        ${CpuFeaturesDir}
        )

set_target_properties(${PROJECT_NAME}
        PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED YES
        )

# Import shaderc lib in NDK
add_library(shaderc_lib STATIC IMPORTED)
set_target_properties(shaderc_lib PROPERTIES IMPORTED_LOCATION
        ${ShadercDir}/libs/${ANDROID_STL}/${ANDROID_ABI}/libshaderc.a)

target_link_libraries(${PROJECT_NAME}
        GLESv3
        android
        ${log-lib}
        EGL
        shaderc_lib
        )


#===================================================================================================

