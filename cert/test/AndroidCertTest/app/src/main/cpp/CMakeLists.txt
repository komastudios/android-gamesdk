cmake_minimum_required(VERSION 3.10.2)

get_filename_component(CppDir ${CMAKE_CURRENT_SOURCE_DIR} ABSOLUTE)
get_filename_component(GameSDKDir ${GAMESDK_DIR} ABSOLUTE)
get_filename_component(LibDir ${CMAKE_CURRENT_SOURCE_DIR}/../../../../common/3rd_party ABSOLUTE)
get_filename_component(BasisDir ${LibDir}/basis_universal ABSOLUTE)
get_filename_component(ShadercDir ${ANDROID_NDK}/sources/third_party/shaderc ABSOLUTE)
get_filename_component(GTestDir ${ANDROID_NDK}/sources/third_party/googletest ABSOLUTE)
get_filename_component(CpuFeaturesDir ${ANDROID_NDK}/sources/android/cpufeatures ABSOLUTE)

#===================================================================================================
include("${GameSDKDir}/src/device_info/device_info.cmake")

project(cert-lib LANGUAGES C CXX)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -fPIC")

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
            ${SWAPPY_LOCATION_VULKAN}/SwappyVkGoogleDisplayTiming.cpp
            ${SOURCE_LOCATION}/../common/system_utils.cpp)

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
            LIBRARIES "swappy"
    )
endif ()

add_compile_options("-frtti")

#===================================================================================================

add_definitions(-DVK_USE_PLATFORM_ANDROID_KHR)

add_library(${PROJECT_NAME} SHARED
        ${ANDROID_NDK}/sources/third_party/vulkan/src/common/vulkan_wrapper.cpp
        # Java-Native glue & super-fundamental code
        cert-lib.cpp

        # Framework & utilities
        ancer/BaseOperation.cpp
        ancer/BaseVulkanOperation.cpp
        ancer/Reporting.cpp
        ancer/Suite.cpp
        ancer/SwappyRenderer.cpp
        ancer/System.cpp
        ancer/System.Asset.cpp
        ancer/System.Cpu.cpp
        ancer/System.Gpu.cpp
        ancer/System.Memory.cpp
        ancer/System.Temperature.cpp
        ancer/util/Basics.cpp
        ancer/util/Bitmath.cpp
        ancer/util/GLHelpers.cpp
        ancer/util/GLPixelBuffer.cpp
        ancer/util/GLTexImage2DRenderer.cpp
        ancer/util/Gzip.cpp
        ancer/util/JNIHelpers.cpp
        ancer/util/LibAndroid.cpp
        ancer/util/LibEGL.cpp
        ancer/util/Texture.cpp
        ancer/util/texture/Astc.cpp
        ancer/util/texture/Etc2.cpp
        ancer/util/texture/Jpg.cpp
        ancer/util/texture/Png.cpp
        ancer/util/ThreadSyncPoint.cpp
        ancer/util/Time.cpp
        ancer/util/VkHelpers.cpp
        ancer/util/vulkan/VkHelpersImpl.cpp
        ancer/vulkan/android_helper.cc
        ancer/vulkan/blend_mode.cc
        ancer/vulkan/buffer.cc
        ancer/vulkan/context.cc
        ancer/vulkan/depth_mode.cc
        ancer/vulkan/format.cc
        ancer/vulkan/graphics_pipeline.cc
        ancer/vulkan/image.cc
        ancer/vulkan/render_pass.cc
        ancer/vulkan/resources.cc
        ancer/vulkan/sampler.cc
        ancer/vulkan/shader_module.cc
        ancer/vulkan/swapchain.cc
        ancer/vulkan/upload.cc
        ancer/vulkan/vulkan_base.cc

        # Operations
        operations/CalculatePIOperation.cpp
        operations/CalculateWaitPIOperation.cpp
        operations/ChoreographerTimestampsOperation.cpp
        operations/IOPerformanceOperation.cpp
        operations/MemoryAccessOperation.cpp
        operations/MemoryAllocOperation.cpp
        operations/MonitorOperation.cpp
        operations/NougatSigabrtOperation.cpp
        operations/SaneCpusetOperation.cpp
        operations/ScheduleAffinityOperation.cpp
        operations/ThreadSchedulingOperation.cpp
        operations/VulkanMprotectCheckOperation.cpp

        operations/gl/BigLittleCore.cpp
        operations/gl/BufferStorageGLES3Operation.cpp
        operations/gl/DependentTextureReadGLES3Operation.cpp
        operations/gl/DepthClearGLES3Operation.cpp
        operations/gl/DualSourceBlendingGLES3Operation.cpp
        operations/gl/EGLGetFrameTimestampsGLES3Operation.cpp
        operations/gl/EGLPresentationTimeGLES3Operation.cpp
        operations/gl/ExternalFramebufferGLES3Operation.cpp
        operations/gl/FillRateGLES3Operation.cpp
        operations/gl/GetExtensionsGLES3Operation.cpp
        operations/gl/HelloGLES3Operation.cpp
        operations/gl/MarchingCubesGLES3Operation.cpp
        operations/gl/marching_cubes/MarchingCubes.cpp
        operations/gl/marching_cubes/Storage.cpp
        operations/gl/marching_cubes/Volume.cpp
        operations/gl/MediumPVecNormalizationGLES3Operation.cpp
        operations/gl/ShaderBlobGLES3Operation.cpp
        operations/gl/TeapotRendererGLES3Operation.cpp
        operations/gl/TextureLoadingGLES3Operation.cpp
        operations/gl/VertexStreamGLES3Operation.cpp

        operations/vulkan/VulkanVaryingsTestOperation.cpp

        # integration testing
        operations/integration/FakeOperation.cpp


        # some external deps
        ${CpuFeaturesDir}/cpu-features.c
        ${GameSDKDir}/samples/common/src/Thread.cpp
        ${GameSdkDir}/
        )

target_include_directories(${PROJECT_NAME} PUBLIC
        include
        ${CppDir}
        ${LibDir}
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

# We won't need to borrow JPG and PNG decoders once our target SDK version is Android R or later. By
# then, the NDK provides a C++ accessible ImageDecoder that deals with JPG, PNG and other formats as
# well.
add_library(basis STATIC
        ${BasisDir}/jpgd.cpp
        ${BasisDir}/lodepng.cpp
        )
target_compile_options(basis PRIVATE -Wno-error=invalid-noreturn)

target_link_libraries(${PROJECT_NAME}
        GLESv3
        android
        log
        EGL
        shaderc_lib
        basis
        z
        )

if (CMAKE_HOST_SYSTEM_NAME MATCHES "Windows")
    target_link_libraries(${PROJECT_NAME}
            swappy_static
            )
else ()
    target_link_libraries(${PROJECT_NAME}
            device_info_static
            gamesdk
            )
endif ()

#===================================================================================================

if (true) # build_tests
    add_definitions(-DBUILD_UNIT_TESTS)
    target_sources(${PROJECT_NAME} PRIVATE
            ancer/util/TestResultPrinter.cpp

            ancer/util/Bitmath.tests.cpp
            ancer/util/Json.tests.cpp
            ancer/util/Time.tests.cpp
            )

    add_library(gtest STATIC ${GTestDir}/src/gtest-all.cc)
    target_include_directories(gtest PRIVATE ${GTestDir})
    target_include_directories(gtest PUBLIC ${GTestDir}/include)
    target_link_libraries(${PROJECT_NAME} gtest)
endif ()
