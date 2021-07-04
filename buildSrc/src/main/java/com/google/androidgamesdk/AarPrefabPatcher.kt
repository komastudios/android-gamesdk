package com.google.androidgamesdk

import com.google.androidgamesdk.OsSpecificTools.Companion.joinPath
import net.lingala.zip4j.ZipFile
import net.lingala.zip4j.model.FileHeader
import net.lingala.zip4j.model.ZipParameters
import org.gradle.api.GradleException
import org.gradle.api.Project
import java.io.File

/**
 * Helper to inject prefab files into an AAR. Can be removed once we can use
 * prefabPublishing in build.gradle of the projects.
 */
class AarPrefabPatcher {
    fun injectPrefabFolder(aarPath: String, prefabFolderPath: String) {
        // Remove the game controller classes and put the new classes.jar
        // back in the .aar
        removeGameControllerClasses(aarPath, "com/google/android/")

        val aarZipFile = ZipFile(aarPath)

        val prefabFolderFile = File(prefabFolderPath)
        val zipParameters = ZipParameters()
        zipParameters.isIncludeRootFolder = false
        zipParameters.rootFolderNameInZip = "prefab"
        aarZipFile.addFolder(prefabFolderFile, zipParameters)
    }

    fun extractAarClasses(aarPath: String, prefabFolderPath: String, doRemove: Boolean) {
        val jarName = "classes.jar"
        val jarPath = joinPath(prefabFolderPath, jarName)

        val checkExists = File(jarPath)
        if (!checkExists.exists()) {
            val aarZipFile = ZipFile(aarPath)
            aarZipFile.extractFile(jarName, prefabFolderPath)
            aarZipFile.removeFile(jarName)
            // Remove the non game controller classes and 
            // put the modified classes.jar back in the .aar
            if (doRemove) {
                removeClasses(aarZipFile, jarPath, "com/google/androidgamesdk/")
            }
        }
    }

    fun removeGameControllerClasses(aarPath: String, directoryToRemove: String) {
        val temporaryDirectory =
            createTempDir("gamesdk-remove-classes")
        var aarZipFile = ZipFile(aarPath)
        val jarName = "classes.jar"
        var jarPath = joinPath(temporaryDirectory.absolutePath, jarName)
        aarZipFile.extractFile(jarName, temporaryDirectory.absolutePath)
        aarZipFile.removeFile(jarName)
        removeClasses(aarZipFile, jarPath, directoryToRemove)
    }

    fun removeClasses(aarZipFile: ZipFile, jarPath: String, directoryToRemove: String) {
        var jarZipFile = ZipFile(jarPath)

        val fileHeaders = jarZipFile.getFileHeaders();
        val removeList = mutableListOf<String>()
        for (fileHeader in fileHeaders) {
            val fileName = fileHeader.getFileName()
            if (fileName.startsWith(directoryToRemove)) {
                removeList.add(fileName)
            }
        }

        for (removeFileName in removeList) {
            jarZipFile.removeFile(removeFileName)
        }

        val zipParameters = ZipParameters()
        zipParameters.isIncludeRootFolder = false
        aarZipFile.addFile(jarPath, zipParameters)
    }
}