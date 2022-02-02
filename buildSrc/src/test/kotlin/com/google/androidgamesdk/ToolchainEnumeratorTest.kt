package com.google.androidgamesdk

import java.io.File
import kotlin.test.Test
import kotlin.test.assertEquals
import org.gradle.api.Project
import org.gradle.testfixtures.ProjectBuilder

class ToolchainEnumeratorTest {
    private fun createMockProject(): Project {
        return ProjectBuilder.builder()
            .withProjectDir(File("/tmp/test"))
            .withName("Mock GameSDK project")
            .build()
    }

    @Test
    fun generateProperToolchains() {
        val project = createMockProject()
        val allToolchains =
            ToolchainEnumerator().enumerate(ToolchainSet.ALL, project)

        // Do a few sanity checks on the enumerated toolchains
        assertEquals(
            // 4 abis, 2 STLs, pre17 ndk/sdks:
            (11 + 11 + 12 + 13 + 11 + 11 + 11 + 12 + 14) * 4 * 2,
            allToolchains.size
        )
        assertEquals(
            212,
            allToolchains.filter { it.abi == "armeabi-v7a" }.count()
        )
        assertEquals(
            424,
            allToolchains.filter { it.stl == "c++_static" }.count()
        )
        assertEquals(
            424,
            allToolchains.filter { it.stl == "c++_shared" }.count()
        )
        assertEquals(
            11 * 4 * 2, // 4 abis, 2 STLs, 11 pre17 ndk/sdks
            allToolchains.filter { it.toolchain.getNdkVersion() == "r14" }
                .count()
        )
        assertEquals(
            11 * 4 * 2, // 4 abis, 2 STLs, 11 post17 ndk/sdks
            allToolchains.filter { it.toolchain.getNdkVersion() == "r18" }
                .count()
        )
    }

    @Test
    fun generateProperAarToolchains() {
        val project = createMockProject()
        val allAarToolchains =
            ToolchainEnumerator().enumerate(ToolchainSet.AAR, project)

        // Do a few sanity checks on the enumerated toolchains
        assertEquals(
            // 2 32bits abis, 1 STLs, pre17 ndk/sdks:
            2 * 1 * (11 + 11 + 12 + 13) +
                // 2 32bits abis, 1 STL, post17 ndk/sdks:
                2 * 1 * (11 + 11 + 11 + 12 + 14) +
                // 2 64bits abis, 1 STLs, pre17 64 bits ndk/sdks:
                2 * 1 * (4 + 5 + 6 + 7) +
                // 2 64bits abis, 1 STL, post17 64bits ndk/sdks:
                2 * 1 * (7 + 7 + 7 + 8 + 10),
            allAarToolchains.size
        )
        assertEquals(
            106,
            allAarToolchains.filter { it.abi == "armeabi-v7a" }.count()
        )
        assertEquals(
            0,
            allAarToolchains.filter { it.stl == "c++_static" }.count()
        )
        assertEquals(
            334,
            allAarToolchains.filter { it.stl == "c++_shared" }.count()
        )
        assertEquals(
            // 2 abis, 1 STLs, 4 r14 ndk/sdks + 2 abis, 1 STLs, 11 r14 ndk/sdks
            4 * 2 * 1 + 11 * 2 * 1,
            allAarToolchains.filter { it.toolchain.getNdkVersion() == "r14" }
                .count()
        )
        assertEquals(
            // 2 abis, 1 STLs, 7 r18 ndk/sdks + 2 abis, 1 STL, 11 r18 ndk/sdks
            7 * 2 * 1 + 11 * 1 * 2,
            allAarToolchains.filter { it.toolchain.getNdkVersion() == "r18" }
                .count()
        )
    }

    private fun makeUpLibraryInfo(name: String) =
        LibraryInfo(name, name + "_j", null, SemanticVersion(1, 0, 0), "alpha")

    @Test
    fun canFilterLibraries() {
        val alwaysLibrary = NativeLibrary(makeUpLibraryInfo("alwayslib"))
        val apiMin24Library = NativeLibrary(makeUpLibraryInfo("apiMin24Lib"))
            .setMinimumAndroidApiLevel(24)
        val ndkMin18Library = NativeLibrary(makeUpLibraryInfo("ndkMin28Lib"))
            .setMinimumNdkVersion(18)
        val someStlOnlyLibrary =
            NativeLibrary(makeUpLibraryInfo("someStlOnlyLib"))
                .setSupportedStlVersions(listOf("c++_static"))

        val allLibraries = listOf(
            alwaysLibrary, apiMin24Library, ndkMin18Library, someStlOnlyLibrary
        )
        val project = createMockProject()

        assertEquals(
            allLibraries,
            ToolchainEnumerator.filterBuiltLibraries(
                allLibraries,
                BuildOptions(
                    stl = "c++_static",
                    arch = "armeabi-v7a",
                    buildType = "Release",
                    threadChecks = true
                ),
                SpecificToolchain(project, "25", "r20")
            )
        )
        assertEquals(
            listOf(alwaysLibrary, apiMin24Library, ndkMin18Library),
            ToolchainEnumerator.filterBuiltLibraries(
                allLibraries,
                BuildOptions(
                    stl = "c++_shared",
                    arch = "armeabi-v7a",
                    buildType = "Release",
                    threadChecks = true
                ),
                SpecificToolchain(project, "25", "r20")
            )
        )
        assertEquals(
            listOf(alwaysLibrary, ndkMin18Library),
            ToolchainEnumerator.filterBuiltLibraries(
                allLibraries,
                BuildOptions(
                    stl = "c++_shared",
                    arch = "armeabi-v7a",
                    buildType = "Release",
                    threadChecks = true
                ),
                SpecificToolchain(project, "16", "r20")
            )
        )
        assertEquals(
            listOf(alwaysLibrary),
            ToolchainEnumerator.filterBuiltLibraries(
                allLibraries,
                BuildOptions(
                    stl = "c++_shared",
                    arch = "armeabi-v7a",
                    buildType = "Release",
                    threadChecks = true
                ),
                SpecificToolchain(project, "16", "r17")
            )
        )
    }
}
