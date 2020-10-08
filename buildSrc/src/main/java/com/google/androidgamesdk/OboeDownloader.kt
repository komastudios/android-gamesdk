package com.google.androidgamesdk

import java.io.File
import java.io.FileOutputStream;
import java.net.URL;
import java.nio.file.Paths;
import net.lingala.zip4j.ZipFile;
import org.gradle.api.Project
import java.nio.file.Files
import java.nio.file.StandardCopyOption

class OboeDownloader(val project: Project) {
    fun fetchAndInstall() {
        download()
        unzip()
        patch()
    }

    private fun download() {
        val link = "https://github.com/google/oboe/archive/1.4.3.zip";
        val path = "oboe-sources.zip";
        URL(link).openStream().use { input ->
            FileOutputStream(File(path)).use { output ->
                input.copyTo(output)
            }
        }
    }

    private fun unzip() {
        val zipFile = ZipFile("oboe-sources.zip");
        val temporaryDirectory =
                createTempDir("gamesdk-third-party-library-oboe");
        val oboeRootPath = temporaryDirectory.absolutePath + "/oboe-1.4.3";
        zipFile.extractAll(temporaryDirectory.absolutePath);
        project.delete("./src/oboe")
        project.delete("./include/oboe")
        Files.move(
                Paths.get("$oboeRootPath/src"),
                Paths.get("./src/oboe"),
                StandardCopyOption.REPLACE_EXISTING)
        Files.move(
                Paths.get("$oboeRootPath/include/oboe"),
                Paths.get("./include/oboe"),
                StandardCopyOption.REPLACE_EXISTING)
        Files.move(
                Paths.get("$oboeRootPath/CMakeLists.txt"),
                Paths.get("./src/oboe/CMakeLists.txt"),
                StandardCopyOption.REPLACE_EXISTING)
        project.delete(temporaryDirectory.absolutePath)
    }

    private fun patch() {
        val cmakeListFile = File("./src/oboe/CMakeLists.txt")
        var cmakeListsContent = cmakeListFile.readText()
        cmakeListsContent = cmakeListsContent.replace(
                "src/", "")
        cmakeListsContent = cmakeListsContent.replace(
                "add_library(oboe \${oboe_sources})",
                "add_library(oboe_static STATIC \${oboe_sources})\n" +
                "add_library(oboe SHARED)\n")
        cmakeListsContent = cmakeListsContent.replace(
                "target_include_directories(oboe\n" +
                        "        PRIVATE src\n" +
                        "        PUBLIC include)\n",
                "target_include_directories(oboe_static\n" +
                        "        PRIVATE .\n" +
                        "        PUBLIC ../../include)\n");
        cmakeListsContent = cmakeListsContent.replace(
                "target_compile_options(oboe",
                "target_compile_options(oboe_static")
        cmakeListsContent = cmakeListsContent.replace(
                "target_link_libraries(oboe PRIVATE",
                "target_link_libraries(oboe PRIVATE oboe_static")
        cmakeListFile.writeText(cmakeListsContent)
    }
}