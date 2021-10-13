package com.google.androidgamesdk

import net.lingala.zip4j.ZipFile
import org.gradle.api.Project
import java.io.File
import java.io.FileOutputStream
import java.net.URL

private const val OBOE_VERSION: String = "1.5.0"

/**
 * Downloads and patch Oboe (https://github.com/google/oboe) to be included
 * in the Game SDK.
 */
class OboeDownloader(val project: Project) {
    fun fetchAndInstall() {
        download()
        unzip()
        patchCmakeLists()
        patchQuirksManager()
    }

    private fun download() {
        val link = "https://github.com/google/oboe/archive/${OBOE_VERSION}.zip"
        val path = "oboe-sources.zip"
        URL(link).openStream().use { input ->
            FileOutputStream(File(path)).use { output ->
                input.copyTo(output)
            }
        }
    }

    private fun unzip() {
        val zipFile = ZipFile("oboe-sources.zip")
        val temporaryDirectory =
            createTempDir("gamesdk-third-party-library-oboe")
        val oboeRootPath = temporaryDirectory.absolutePath +
                "/oboe-${OBOE_VERSION}"
        zipFile.extractAll(temporaryDirectory.absolutePath)

        // Import in the Game SDK only the sources, includes and build file.
        project.delete("./src/oboe")
        project.mkdir("./src/oboe")
        project.delete("./include/oboe")
        project.mkdir("./include/oboe")
        project.copy {
            from("$oboeRootPath/src")
            into("./src/oboe")
        }
        project.copy {
            from("$oboeRootPath/include/oboe")
            into("./include/oboe")
        }
        project.copy {
            from("$oboeRootPath/CMakeLists.txt")
            into("./src/oboe")
        }

        project.delete(temporaryDirectory.absolutePath)
    }

    private fun patchCmakeLists() {
        val cmakeListFile = project.file("./src/oboe/CMakeLists.txt")
        var cmakeListsContent = cmakeListFile.readText()

        // Patch the sources path, as the CMakeLists file is now in the sources.
        cmakeListsContent = cmakeListsContent.replace(
            "src/", ""
        )

        cmakeListsContent = cmakeListsContent.replace(
          "cmake_minimum_required(VERSION 3.4.1)",
          "cmake_minimum_required(VERSION 3.18.1)\n" +
            "set(CMAKE_CXX_STANDARD 14)\n"
          )

        // Add a static library version of Oboe and patch the include paths.
        cmakeListsContent = cmakeListsContent.replace(
            "add_library(oboe \${oboe_sources})",
            "add_library(oboe_static STATIC \${oboe_sources})\n" +
                "add_library(oboe SHARED \${oboe_sources})\n"
        )
        cmakeListsContent = cmakeListsContent.replace(
            "target_include_directories(oboe\n" +
                "        PRIVATE src\n" +
                "        PUBLIC include)\n",
            "target_include_directories(oboe_static\n" +
                "        PRIVATE .\n" +
                "        PUBLIC ../../include)\n"
        )
        cmakeListsContent = cmakeListsContent.replace(
            "target_compile_options(oboe",
            "target_compile_options(oboe_static"
        )
        cmakeListsContent = cmakeListsContent.replace(
            "target_link_libraries(oboe PRIVATE",
            "target_link_libraries(oboe PRIVATE oboe_static"
        )

        cmakeListFile.writeText(cmakeListsContent)
    }

    /**
     * Apply a patch to fix compilation with some SDK/NDK. See
     * https://github.com/google/oboe/issues/1148
     *
     * Can be removed once a new release of Oboe is published.
     */
    private fun patchQuirksManager() {
        val quirksManagerH = project.file("./src/oboe/common/QuirksManager.h")
        var quirksManagerHContent = quirksManagerH.readText()

        quirksManagerHContent = quirksManagerHContent.replace(
                "namespace oboe {",
                "#ifndef __ANDROID_API_R__\n" +
                "#define __ANDROID_API_R__ 30\n" +
                "#endif\n" +
                "\n" +
                "namespace oboe {"
        );

        quirksManagerH.writeText(quirksManagerHContent);
    }
}
