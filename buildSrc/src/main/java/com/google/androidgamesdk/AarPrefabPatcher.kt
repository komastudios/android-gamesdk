package com.google.androidgamesdk

import net.lingala.zip4j.ZipFile
import net.lingala.zip4j.model.ZipParameters
import java.io.File

/**
 * Helper to inject prefab files into an AAR. Can be removed once we can use
 * prefabPublishing in build.gradle of the projects.
 */
class AarPrefabPatcher {
    fun injectPrefabFolder(aarPath: String, prefabFolderPath: String) {
        val aarZipFile = ZipFile(aarPath)

        val prefabFolderFile = File(prefabFolderPath)
        val zipParameters = ZipParameters()
        zipParameters.isIncludeRootFolder = false
        zipParameters.rootFolderNameInZip = "prefab"
        aarZipFile.addFolder(prefabFolderFile, zipParameters)
    }
}
