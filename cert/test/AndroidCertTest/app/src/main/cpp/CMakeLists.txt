cmake_minimum_required(VERSION 3.10.2)

get_filename_component(CppDir ${CMAKE_CURRENT_SOURCE_DIR} ABSOLUTE)
get_filename_component(GameSDKDir ${GAMESDK_DIR} ABSOLUTE)
get_filename_component(LibDir ${CMAKE_CURRENT_SOURCE_DIR}/../../../../common/3rd_party ABSOLUTE)
get_filename_component(ShadercDir ${ANDROID_NDK}/sources/third_party/shaderc ABSOLUTE)
get_filename_component(GTestDir ${ANDROID_NDK}/sources/third_party/googletest ABSOLUTE)
get_filename_component(CpuFeaturesDir ${ANDROID_NDK}/sources/android/cpufeatures ABSOLUTE)

#===================================================================================================

project(native-lib LANGUAGES C CXX)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

# TODO: Need to see about getting the GameSDK to support Windows.
#  The one definite build issue is a phase that uses ar to merge the various components into a
#  single gamesdk lib. There may be other issues in code, but since we're theoretically building
#  for Android and not Windows/Mac/etc., that seems like it shouldn't be an issue.
if (CMAKE_HOST_SYSTEM_NAME MATCHES "Windows")
    set(SWAPPY_LOCATION "${GameSDKDir}/src/swappy")
    set(SWAPPY_LOCATION_COMMON "${SWAPPY_LOCATION}/common")
    set(SWAPPY_LOCATION_OPENGL "${SWAPPY_LOCATION}/opengl")
    set(SWAPPY_LOCATION_VULKAN "${SWAPPY_LOCATION}/vulkan")

    add_library(swappy_static STATIC
            ${SWAPPY_LOCATION_COMMON}/ChoreographerFilter.cpp
            ${SWAPPY_LOCATION_COMMON}/ChoreographerThread.cpp
            ${SWAPPY_LOCATION_COMMON}/FrameStatistics.cpp
            ${SWAPPY_LOCATION_COMMON}/CpuInfo.cpp
            ${SWAPPY_LOCATION_COMMON}/SystemProperties.cpp
            ${SWAPPY_LOCATION_COMMON}/Settings.cpp
            ${SWAPPY_LOCATION_COMMON}/Thread.cpp
            ${SWAPPY_LOCATION_COMMON}/SwappyCommon.cpp
            ${SWAPPY_LOCATION_COMMON}/swappy_c.cpp
            ${SWAPPY_LOCATION_COMMON}/SwappyDisplayManager.cpp
            ${SWAPPY_LOCATION_COMMON}/CPUTracer.cpp
            ${SWAPPY_LOCATION_OPENGL}/EGL.cpp
            ${SWAPPY_LOCATION_OPENGL}/swappyGL_c.cpp
            ${SWAPPY_LOCATION_OPENGL}/SwappyGL.cpp
            ${ANDROID_NDK}/sources/third_party/vulkan/src/common/vulkan_wrapper.cpp
            ${SWAPPY_LOCATION_VULKAN}/swappyVk_c.cpp
            ${SWAPPY_LOCATION_VULKAN}/SwappyVk.cpp
            ${SWAPPY_LOCATION_VULKAN}/SwappyVkBase.cpp
            ${SWAPPY_LOCATION_VULKAN}/SwappyVkFallback.cpp
            ${SWAPPY_LOCATION_VULKAN}/SwappyVkGoogleDisplayTiming.cpp)

    set_target_properties(swappy_static PROPERTIES
            LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/build)

    target_include_directories(swappy_static PUBLIC
            ${SWAPPY_LOCATION}/../../include
            ${SWAPPY_LOCATION}/../common
            ${SWAPPY_LOCATION}/common
            ${SWAPPY_LOCATION}/opengl
            ${SWAPPY_LOCATION}/vulkan
            ${ANDROID_NDK}/sources/third_party/vulkan/src/common
            )
else ()
    include("${GameSDKDir}/samples/gamesdk.cmake")
    add_gamesdk_target(
            DO_LOCAL_BUILD
            ROOT_DIR "${GameSDKDir}"
            PACKAGE_DIR "${GameSDKDir}/../package/local"
            GEN_TASK "localBuild"
    )
endif ()


find_library(log-lib
        log)

#===================================================================================================

add_definitions(-DVK_USE_PLATFORM_ANDROID_KHR)

add_library(${PROJECT_NAME} SHARED
        # Java-Native glue & super-fundamental code
        native-lib.cpp

        # Framework & utilities
        ancer/BaseOperation.cpp
        ancer/BaseVulkanOperation.cpp
        ancer/Reporting.cpp
        ancer/Suite.cpp
        ancer/SwappyRenderer.cpp
        ancer/System.cpp
        ancer/util/GLHelpers.cpp
        ancer/util/Time.cpp
        ancer/util/vulkan/VkHelpersImpl.cpp
        ancer/util/VkHelpers.cpp
        ancer/util/Basics.cpp

        # Operations
        operations/CalculatePIOperation.cpp
        operations/MemoryAccessOperation.cpp
        operations/MemoryAllocOperation.cpp
        operations/MonitorOperation.cpp
        operations/ScheduleAffinityOperation.cpp
        operations/ThreadSchedulingOperation.cpp
        operations/VulkanMprotectCheckOperation.cpp

        operations/gl/FillRateGLES3Operation.cpp
        operations/gl/HelloGLES3Operation.cpp
        operations/gl/StreamDrawGLES3Operation.cpp
        operations/gl/TeapotRendererGLES3Operation.cpp
        operations/gl/HalfFloatPrecisionGLES3Operation.cpp

        # some external deps
        ${CpuFeaturesDir}/cpu-features.c
        ${GameSDKDir}/samples/common/src/Thread.cpp
        )

target_include_directories(${PROJECT_NAME} PUBLIC
        include
        ${CppDir}
        ${LibDir}/json/include
        ${LibDir}/glm/
        ${GameSDKDir}/include
        ${GameSDKDir}/samples/common/include
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

if (CMAKE_HOST_SYSTEM_NAME MATCHES "Windows")
    target_link_libraries(${PROJECT_NAME}
            swappy_static
            )
else ()
    target_link_libraries(${PROJECT_NAME}
            gamesdk
            )
endif ()

#===================================================================================================

if (true) # build_tests
    add_definitions(-DBUILD_UNIT_TESTS)
    target_sources(${PROJECT_NAME} PRIVATE
            ancer/util/TestResultPrinter.cpp

            ancer/util/Json.tests.cpp
            ancer/util/Time.tests.cpp
            )

    add_library(gtest STATIC ${GTestDir}/src/gtest-all.cc)
    target_include_directories(gtest PRIVATE ${GTestDir})
    target_include_directories(gtest PUBLIC ${GTestDir}/include)
    target_link_libraries(${PROJECT_NAME} gtest)
endif ()
