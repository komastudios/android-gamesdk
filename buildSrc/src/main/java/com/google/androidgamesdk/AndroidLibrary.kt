package com.google.androidgamesdk

/**
 * Describe a Android Library project (that will create an AAR), optionally
 * with a prefab folder to include in the AAR (useful as we patch AAR manually
 * to include prefab - can be removed once we use prefabPublishing).
 */
data class AndroidLibrary(
    val nativeLibraryName: String,
    val projectName: String
) {
    var aarLibraryName: String = ""
        private set
    var aarVersion: String = "0.0.0"
        private set
    var prefabFolderName: String = ""
        private set

    fun setAarLibrary(
        aarLibraryName: String,
        aarVersion: String
    ): AndroidLibrary {
        this.aarLibraryName = aarLibraryName
        this.aarVersion = aarVersion

        return this
    }

    fun setPrefabFolderName(
        prefabFolderName: String
    ): AndroidLibrary {
        this.prefabFolderName = prefabFolderName

        return this
    }
}
