include(CMakeParseArguments) # CMake 2.8 - 3.4 compatibility

set( _MY_DIR ${CMAKE_CURRENT_LIST_DIR})

# This function will use (and build if asked to) a static library target called 'gamesdk'.
# The location of the library is set according to your ANDROID_NDK_REVISION
# and ANDROID_PLATFORM, unless you explicitly set ANDROID_NDK_VERSION and/or
# ANDROID_SDK_VERSION arguments.
#
# All supported arguments are:
#  PACKAGE_DIR: where the packaged version of the library is (relative to the caller working directory).
#    This parameter is mandatory.
#  ROOT_DIR: where the gamesdk directory is located (relative to the caller working directory), when building from sources.
#    This must be specified if DO_LOCAL_BUILD is set
#  LIBRARIES: the library names to build, when building from sources.
#    This must be specified if DO_LOCAL_BUILD is set.
#  DO_LOCAL_BUILD: whether to add a custom build command to build the gamesdk (ON/OFF).
#    default value: OFF
#  ANDROID_NDK_VERSION: override the version number for the NDK (major.minor.patch).
#    It's recommended to use `ndkVersion` in your build.gradle file if you want to use a specific NDK.
#    default value: derived from ANDROID_NDK_REVISION.
#  ANDROID_API_LEVEL: override the android API level.
#    It's recommended to change the SDK version in your build.gradle to use a specific SDK.
#    default value: derived from ANDROID_PLATFORM.
#  BUILD_TYPE: type of Game SDK build libraries to use. Can be "Release" or "Debug".
#    default value: Release
function(add_gamesdk_target)
    set(options DO_LOCAL_BUILD)
    set(oneValueArgs LIBRARIES PACKAGE_DIR ROOT_DIR ANDROID_NDK_VERSION ANDROID_API_LEVEL BUILD_TYPE)
    cmake_parse_arguments(GAMESDK "${options}" "${oneValueArgs}" "" ${ARGN} )

    # Make sanity checks to avoid hard to debug errors at compile/link time.
    if(NOT DEFINED GAMESDK_PACKAGE_DIR)
        message(FATAL_ERROR "You must specify PACKAGE_DIR when calling add_gamesdk_target. It should be the folder where gamesdk is extracted (or where packages should be built, if compiling from sources).")
    endif()
    get_filename_component(INCLUDE_FULL_PATH "${GAMESDK_PACKAGE_DIR}/include/" REALPATH)
    if (NOT GAMESDK_DO_LOCAL_BUILD AND NOT EXISTS ${INCLUDE_FULL_PATH})
        message(FATAL_ERROR "Can't find the gamesdk includes in ${INCLUDE_FULL_PATH}. Are you sure you properly set up the path to the Game SDK using GAMESDK_PACKAGE_DIR?")
    endif()
    if (GAMESDK_DO_LOCAL_BUILD AND NOT DEFINED GAMESDK_LIBRARIES)
        message(FATAL_ERROR "You specified DO_LOCAL_BUILD to build the game sdk from sources, but did not specified the libraries to build with LIBRARIES.")
    endif()
    if(GAMESDK_DO_LOCAL_BUILD AND NOT DEFINED GAMESDK_ROOT_DIR)
        message(FATAL_ERROR "You specified DO_LOCAL_BUILD to build the game sdk from sources, but did not specified the gamesdk root folder with ROOT_DIR (used to run Gradle).")
    endif()
    if(NOT DEFINED GAMESDK_BUILD_TYPE)
        set(GAMESDK_BUILD_TYPE "${CMAKE_BUILD_TYPE}")
    endif()

    # Infer Android SDK/NDK and STL versions
    if (NOT DEFINED GAMESDK_ANDROID_NDK_VERSION)
        set(GAMESDK_ANDROID_NDK_VERSION ${ANDROID_NDK_REVISION})
    endif()
    string(REGEX REPLACE "^([^.]+).*" "\\1" GAMESDK_ANDROID_NDK_MAJOR_VERSION ${GAMESDK_ANDROID_NDK_VERSION} ) # Get NDK major version

    # TODO(b/192829605): look into why sometimes ANDROID_NDK_REVISION seems to be wrong
    string(SUBSTRING ${GAMESDK_ANDROID_NDK_MAJOR_VERSION} 0 2 GAMESDK_ANDROID_NDK_MAJOR_VERSION) # Mitigate bug above by taking at most 2 chars

    if (NOT DEFINED GAMESDK_ANDROID_API_LEVEL)
        string(REGEX REPLACE "^android-([^.]+)" "\\1" GAMESDK_ANDROID_API_LEVEL ${ANDROID_PLATFORM} )
    endif()
    string(REPLACE "+" "p" GAMESDK_ANDROID_STL ${ANDROID_STL}) # Game SDK build names use a sanitized STL name (c++ => cpp)

    # Set up the "gamesdk" libraries
    set(BUILD_NAME ${ANDROID_ABI}_API${GAMESDK_ANDROID_API_LEVEL}_NDK${GAMESDK_ANDROID_NDK_MAJOR_VERSION}_${GAMESDK_ANDROID_STL}_${GAMESDK_BUILD_TYPE})
    set(GAMESDK_LIBS_DIR "${GAMESDK_PACKAGE_DIR}/libs/${BUILD_NAME}")
    set(GAMESDK_SHARED_LIBS_DIR "${GAMESDK_PACKAGE_DIR}/libs/${ANDROID_ABI}")

    include_directories( "${GAMESDK_PACKAGE_DIR}/include" ) # Games SDK Public Includes

    get_filename_component(SWAPPY_DEP_LIB "${GAMESDK_LIBS_DIR}/libswappy_static.a" REALPATH)
    get_filename_component(TUNINGFORK_DEP_LIB "${GAMESDK_LIBS_DIR}/libtuningfork_static.a" REALPATH)
    get_filename_component(MEMORY_ADVICE_DEP_LIB "${GAMESDK_LIBS_DIR}/libmemory_advice_static.a" REALPATH)
    get_filename_component(OBOE_DEP_LIB "${GAMESDK_LIBS_DIR}/liboboe_static.a" REALPATH)
    add_library(swappy STATIC IMPORTED GLOBAL)
    add_library(tuningfork STATIC IMPORTED GLOBAL)
    add_library(memory_advice STATIC IMPORTED GLOBAL)
    add_library(oboe STATIC IMPORTED GLOBAL)

    if(GAMESDK_DO_LOCAL_BUILD)
        # Get the absolute path for the root dir, otherwise it can't be used as a working directory for commands.
        get_filename_component(GAMESDK_ROOT_DIR "${GAMESDK_ROOT_DIR}" REALPATH)

        # If building from a project containing local.properties, generated by Android Studio with
        # the local Android SDK and NDK paths, copy it to gamesdk to allow it to build with the local
        # toolchain.
        if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../local.properties")
            file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/../local.properties
                 DESTINATION ${GAMESDK_ROOT_DIR})
        endif()

        # Build Game SDK (Gradle will use local.properties to find the Android SDK/NDK,
        # or the environment variables if no local.properties - i.e: if compiling from command line).
        if (${CMAKE_HOST_SYSTEM_NAME} MATCHES "Windows")
            file(TO_NATIVE_PATH "${GAMESDK_ROOT_DIR}/gradlew.bat" GAMESDK_GRADLE_BIN)
        else()
            file(TO_NATIVE_PATH "${GAMESDK_ROOT_DIR}/gradlew" GAMESDK_GRADLE_BIN)
        endif()
        add_custom_command(
            OUTPUT
                ${SWAPPY_DEP_LIB} ${TUNINGFORK_DEP_LIB}  ${MEMORY_ADVICE_DEP_LIB} ${OBOE_DEP_LIB}
            COMMAND
                ${GAMESDK_GRADLE_BIN} buildLocal -Plibraries=${GAMESDK_LIBRARIES} -PandroidApiLevel=${GAMESDK_ANDROID_API_LEVEL} -PbuildType=${GAMESDK_BUILD_TYPE} -PpackageName=local -Pndk=${GAMESDK_ANDROID_NDK_VERSION} --info
            VERBATIM
            WORKING_DIRECTORY
                "${GAMESDK_ROOT_DIR}"
        )
        add_custom_target(swappy_lib DEPENDS ${SWAPPY_DEP_LIB})
        add_custom_target(tuningfork_lib DEPENDS ${TUNINGFORK_DEP_LIB})
        add_custom_target(memory_advice_lib DEPENDS ${MEMORY_ADVICE_DEP_LIB})
        add_custom_target(oboe_lib DEPENDS ${OBOE_DEP_LIB})
        add_dependencies(swappy swappy_lib)
        add_dependencies(tuningfork tuningfork_lib)
        add_dependencies(memory_advice memory_advice_lib)
        add_dependencies(oboe oboe_lib)
    else()
        # Validity check to ensure that the library files exist
        if(NOT EXISTS ${SWAPPY_DEP_LIB} AND
            NOT EXISTS ${TUNINGFORK_DEP_LIB} AND
            NOT EXISTS ${MEMORY_ADVICE_DEP_LIB} AND
            NOT EXISTS ${OBOE_DEP_LIB})
            message(FATAL_ERROR "Can't find any library in \"${GAMESDK_LIBS_DIR}\". Are you sure you are using a supported Android SDK/NDK and STL variant?")
        endif()
    endif()

    # Set targets to use the libraries (see https://gitlab.kitware.com/cmake/community/wikis/doc/tutorials/Exporting-and-Importing-Targets)
    set_target_properties(swappy PROPERTIES IMPORTED_LOCATION ${SWAPPY_DEP_LIB})
    set_target_properties(tuningfork PROPERTIES IMPORTED_LOCATION ${TUNINGFORK_DEP_LIB})
    set_target_properties(memory_advice PROPERTIES IMPORTED_LOCATION ${MEMORY_ADVICE_DEP_LIB})
    set_target_properties(oboe PROPERTIES IMPORTED_LOCATION ${OBOE_DEP_LIB})

endfunction()

# Use this function in addition to add_gamesdk_target, to integrate GameSDK
# sources to your project - allowing the IDE to provide autocompletions and
# debugging.
function(add_gamesdk_sources)
    add_subdirectory("${_MY_DIR}/../src" "${_MY_DIR}/../src")
endfunction()
