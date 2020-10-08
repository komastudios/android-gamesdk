package com.google.androidgamesdk

/**
 * Describe a native library that can be compiled with CMake and that can
 * have one or more sample or related Android project.
 *
 * @see ExternalAndroidProject
 */
data class NativeLibrary(
    val nativeLibraryName: String,
    val cmakeOption: String
) {
    var aarLibraryName: String = ""
        private set
    var aarVersion: String = "0.0.0"
        private set
    var sampleAndroidProjects: ArrayList<ExternalAndroidProject> = ArrayList()
        private set
    var sampleExtraFolderPaths: ArrayList<SampleFolder> = ArrayList()
        private set
    var minimumAndroidApiLevel: Int? = null
        private set
    var minimumNdkVersion: Int? = null
        private set
    var supportedStlVersions: List<String>? = null
        private set
    var isThirdParty = false
        private set

    fun setAarLibrary(
        aarLibraryName: String,
        aarVersion: String
    ): NativeLibrary {
        this.aarLibraryName = aarLibraryName
        this.aarVersion = aarVersion

        return this
    }

    fun addSampleAndroidProject(
        sampleAndroidProject: ExternalAndroidProject
    ): NativeLibrary {
        sampleAndroidProjects.add(sampleAndroidProject)
        return this
    }

    fun addSampleExtraFolder(
        sampleFolder: SampleFolder
    ): NativeLibrary {
        sampleExtraFolderPaths.add(sampleFolder)
        return this
    }

    fun setMinimumAndroidApiLevel(
        minimumAndroidApiLevel: Int?
    ): NativeLibrary {
        this.minimumAndroidApiLevel = minimumAndroidApiLevel
        return this
    }

    fun setMinimumNdkVersion(minimumNdkVersion: Int?): NativeLibrary {
        this.minimumNdkVersion = minimumNdkVersion
        return this
    }

    fun setSupportedStlVersions(
        supportedStlVersions: List<String>?
    ): NativeLibrary {
        this.supportedStlVersions = supportedStlVersions
        return this
    }

    fun setThirdParty(): NativeLibrary {
        this.isThirdParty = true
        return this
    }

    data class SampleFolder(
        val sourcePath: String,
        val destinationPath: String
    ) {
        constructor(sourcePath: String) : this(sourcePath, sourcePath)

        var includePattern: String? = null
            private set

        fun include(includePattern: String): SampleFolder {
            this.includePattern = includePattern

            return this
        }
    }
}
