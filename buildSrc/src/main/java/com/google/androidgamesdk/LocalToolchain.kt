package com.google.androidgamesdk

import com.google.androidgamesdk.OsSpecificTools.Companion.joinPath
import org.gradle.api.GradleException
import org.gradle.api.Project

class LocalToolchain(
    override var project_: Project,
    override var androidVersion_: String
) : Toolchain() {
    var ndkPath_: String? = null
    var adbPath_: String? = null
    override var ndkVersion_: String = "UNKNOWN"

    init {
        var sdkPath_ = System.getenv("ANDROID_HOME")
        if (sdkPath_ == null &&
            project_.rootProject.file("local.properties").isFile
        ) {
            val properties = loadPropertiesFromFile(
                project_.rootProject.file("local.properties")
            )
            sdkPath_ = properties.getProperty("sdk.dir")
        }
        if (sdkPath_ == null)
            throw GradleException(
                "Must set ANDROID_HOME or sdk.dir in local.properties"
            )

        // Try to find a side-by-side NDK if a specific version is specified
        var requestedNdkRevision = System.getenv("ANDROID_NDK_REVISION")
        if (requestedNdkRevision != null) {
            val ndkSideBySidePath = joinPath(
                sdkPath_, "ndk",
                requestedNdkRevision
            )
            if (project_.file(ndkSideBySidePath).exists())
                ndkPath_ = ndkSideBySidePath

            // Don't throw an error if not found, as the version might be the default one
            // installed (see next lines).
        }

        // Try to find the NDK specified in ANDROID_NDK or the default one.
        if (ndkPath_ == null) {
            ndkPath_ = System.getenv("ANDROID_NDK")
        }
        if (ndkPath_ == null) {
            ndkPath_ = System.getenv("ANDROID_NDK_HOME")
        }
        if (ndkPath_ == null &&
            project_.rootProject.file("local.properties").isFile()
        ) {
            val properties = loadPropertiesFromFile(
                project_.rootProject.file("local.properties")
            )
            ndkPath_ = properties.getProperty("ndk.dir")
        }
        if (ndkPath_ == null) {
            ndkPath_ = joinPath(sdkPath_, "ndk-bundle")
            if (!project_.file(ndkPath_!!).exists())
                throw GradleException(
                    /* ktlint-disable max-line-length */
                    "No NDK found in SDK: must set ANDROID_NDK or ANDROID_NDK_REVISION with the version of a NDK installed in the SDK \"ndk\" folder (NDK side-by-side)."
                    /* ktlint-enable max-line-length */
                )
        }

        // Get the version of the NDK that was found and sanity check that it's the proper one.
        ndkVersion_ = getNdkVersionFromPropertiesFile()
        adbPath_ = joinPath(sdkPath_, "platform-tools", "adb")

        if (requestedNdkRevision != null) {
            val requestNDKMajorVersion =
                extractNdkMajorVersion(requestedNdkRevision)
            if (ndkVersion_ != requestNDKMajorVersion) {
                throw GradleException(
                    /* ktlint-disable max-line-length */
                    "You specified NDK $requestedNdkRevision but only NDK $ndkVersion_ was found in $ndkPath_. Verify that you have the requested NDK installed with the SDK manager."
                    /* ktlint-enable max-line-length */
                )
            }
        }
    }
    override fun getAndroidNDKPath(): String {
        return ndkPath_!!
    }
    fun getAdbPath(): String {
        return adbPath_!!
    }
}
