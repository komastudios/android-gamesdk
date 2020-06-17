package com.google.androidgamesdk

data class BuildOptions(
        val buildType: String,
        val threadChecks: Boolean,
        var stl: String? = null,
        var arch: String? = null,
        var buildFolders: BuildFolders? = null
) {
    constructor(buildType: String, threadChecks: Boolean): this(buildType, threadChecks, null, null, null)
    constructor(buildType: String, threadChecks: Boolean, stl: String): this(buildType, threadChecks, stl, null, null)

    fun isCompleted(): Boolean {
        return stl != null && arch != null && buildFolders != null
    }
}