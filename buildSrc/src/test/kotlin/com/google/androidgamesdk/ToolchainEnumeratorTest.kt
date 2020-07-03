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
        assertEquals(
            // 4 abis, 4 STLs, pre17 ndk/sdks:
            (11 + 11 + 12 + 13) * 4 * 4 +
                // 4 abis, 4 STLs, post17 ndk/sdks:
                (11 + 11 + 11 + 12) * 2 * 4,
            allToolchains.size
        )
    }
    @Test
    fun generateProperAarToolchains() {
        val project = createMockProject()
        val allAarToolchains =
            ToolchainEnumerator().enumerateAllAarToolchains(project)
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
    }
}
