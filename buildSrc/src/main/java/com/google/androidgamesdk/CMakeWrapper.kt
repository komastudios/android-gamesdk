package com.google.androidgamesdk

import org.gradle.api.Project

class CMakeWrapper {
    companion object {
        private fun ensureFoldersReady(
            project: Project,
            buildFolders: BuildFolders
        ) {
            project.mkdir(buildFolders.workingFolder)
            project.mkdir(buildFolders.outputFolder)
        }

        @JvmStatic
        fun runAndroidCMake(
            project: Project,
            buildFolders: BuildFolders,
            toolchain: Toolchain,
            buildOptions: BuildOptions,
            libraries: Collection<NativeLibrary>
        ) {
            ensureFoldersReady(project, buildFolders)

            val ndkPath = toolchain.getAndroidNDKPath()
            val toolchainFilePath = ndkPath +
                "/build/cmake/android.toolchain.cmake"
            val androidVersion = toolchain.getAndroidVersion()

            var cxx_flags =
                "-DANDROID_NDK_VERSION=${toolchain.getNdkVersionNumber()}"
            if (buildOptions.stl == "gnustl_static" ||
                buildOptions.stl == "gnustl_shared"
            )
                cxx_flags += " -DANDROID_GNUSTL"

            val cmdLine = mutableListOf(
                toolchain.getCMakePath(),
                buildFolders.projectFolder,
                "-DCMAKE_BUILD_TYPE=" + buildOptions.buildType,
                "-DANDROID_PLATFORM=android-$androidVersion",
                "-DANDROID_NDK=$ndkPath",
                "-DANDROID_STL=" + buildOptions.stl,
                "-DANDROID_ABI=" + buildOptions.arch,
                "-DANDROID_UNIFIED_HEADERS=1",
                "-DCMAKE_CXX_FLAGS=$cxx_flags",
                "-DCMAKE_TOOLCHAIN_FILE=$toolchainFilePath",
                "-DCMAKE_ARCHIVE_OUTPUT_DIRECTORY=" + buildFolders.outputFolder,
                "-DGAMESDK_THREAD_CHECKS=" +
                    (if (buildOptions.threadChecks) "1" else "0"),
                "-DCMAKE_MAKE_PROGRAM=" + toolchain.getNinjaPath(),
                "-GNinja"
            )

            libraries.forEach { nativeLibrary ->
                val nativeLibraryCMakeOption = nativeLibrary.cmakeOption
                cmdLine.add("-D$nativeLibraryCMakeOption=ON")
            }

            project.exec {
                val protocBinDir = toolchain.getProtobufInstallPath() + "/bin"
                environment(
                    "PATH",
                    protocBinDir + ":" +
                        environment.get("PATH")
                )
                workingDir(buildFolders.workingFolder)
                commandLine(cmdLine)
            }
        }

        @JvmStatic
        fun runHostCMake(
            project: Project,
            buildFolders: BuildFolders,
            toolchain: Toolchain,
            buildType: String
        ) {
            ensureFoldersReady(project, buildFolders)

            val cmdLine = listOf(
                toolchain.getCMakePath(),
                buildFolders.projectFolder,
                "-DCMAKE_BUILD_TYPE=$buildType",
                "-DCMAKE_CXX_FLAGS=",
                "-DCMAKE_RUNTIME_OUTPUT_DIRECTORY=" + buildFolders.outputFolder
            )

            project.exec {
                workingDir(buildFolders.workingFolder)
                commandLine(cmdLine)
            }
        }
    }
}
