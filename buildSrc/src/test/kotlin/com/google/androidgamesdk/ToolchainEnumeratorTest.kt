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
            416,
            allToolchains.size
        )
        assertEquals(
            104,
            allToolchains.filter { it.abi == "armeabi-v7a" }.count()
        )
        assertEquals(
            208,
            allToolchains.filter { it.stl == "c++_static" }.count()
        )
        assertEquals(
            208,
            allToolchains.filter { it.stl == "c++_shared" }.count()
        )
        assertEquals(
            0,
            allToolchains.filter { it.toolchain.getNdkVersion() == "r14" }
                .count()
        )
        assertEquals(
            64,
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
            196,
            allAarToolchains.size
        )
        assertEquals(
            52,
            allAarToolchains.filter { it.abi == "armeabi-v7a" }.count()
        )
        assertEquals(
            0,
            allAarToolchains.filter { it.stl == "c++_static" }.count()
        )
        assertEquals(
            196,
            allAarToolchains.filter { it.stl == "c++_shared" }.count()
        )
        assertEquals(
            0,
            allAarToolchains.filter { it.toolchain.getNdkVersion() == "r14" }
                .count()
        )
        assertEquals(
            30,
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
