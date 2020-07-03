package com.google.androidgamesdk

import org.gradle.api.Project
import org.gradle.testfixtures.ProjectBuilder
import java.io.File
import kotlin.test.Test
import kotlin.test.assertEquals

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
            ToolchainEnumerator().enumerateAllToolchains(project)

        // Do a few sanity checks on the enumerated toolchains
        assertEquals(
            // 4 abis, 4 STLs, pre17 ndk/sdks:
            (11 + 11 + 12 + 13) * 4 * 4 +
                // 4 abis, 4 STLs, post17 ndk/sdks:
                (11 + 11 + 11 + 12) * 2 * 4,
            allToolchains.size
        )
        assertEquals(
            278,
            allToolchains.filter { it.abi == "armeabi-v7a" }.count()
        )
        assertEquals(
            368,
            allToolchains.filter { it.stl == "c++_static" }.count()
        )
        assertEquals(
            368,
            allToolchains.filter { it.stl == "c++_shared" }.count()
        )
        assertEquals(
            188,
            allToolchains.filter { it.stl == "gnustl_static" }.count()
        )
        assertEquals(
            188,
            allToolchains.filter { it.stl == "gnustl_shared" }.count()
        )
        assertEquals(
            11 * 4 * 4, // 4 abis, 4 STLs, 11 pre17 ndk/sdks
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
            ToolchainEnumerator().enumerateAllAarToolchains(project)

        // Do a few sanity checks on the enumerated toolchains
        assertEquals(
            // 2 32bits abis, 2 STLs, pre17 ndk/sdks:
            2 * 2 * (11 + 11 + 12 + 13) +
                // 2 32bits abis, 1 STL, post17 ndk/sdks:
                2 * 1 * (11 + 11 + 11 + 12) +
                // 2 64bits abis, 2 STLs, pre17 64 bits ndk/sdks:
                2 * 2 * (4 + 5 + 6 + 7) +
                // 2 64bits abis, 1 STL, post17 64bits ndk/sdks:
                2 * 1 * (7 + 7 + 7 + 8),
            allAarToolchains.size
        )
        assertEquals(
            139,
            allAarToolchains.filter { it.abi == "armeabi-v7a" }.count()
        )
        assertEquals(
            0,
            allAarToolchains.filter { it.stl == "c++_static" }.count()
        )
        assertEquals(
            286,
            allAarToolchains.filter { it.stl == "c++_shared" }.count()
        )
        assertEquals(
            0,
            allAarToolchains.filter { it.stl == "gnustl_static" }.count()
        )
        assertEquals(
            138,
            allAarToolchains.filter { it.stl == "gnustl_shared" }.count()
        )
        assertEquals(
            // 2 abis, 2 STLs, 4 r14 ndk/sdks + 2 abis, 2 STLs, 11 r14 ndk/sdks
            4 * 2 * 2 + 11 * 2 * 2,
            allAarToolchains.filter { it.toolchain.getNdkVersion() == "r14" }
                .count()
        )
        assertEquals(
            // 2 abis, 2 STLs, 7 r18 ndk/sdks + 2 abis, 1 STL, 11 r18 ndk/sdks
            7 * 2 * 1 + 11 * 1 * 2,
            allAarToolchains.filter { it.toolchain.getNdkVersion() == "r18" }
                .count()
        )
    }
}
