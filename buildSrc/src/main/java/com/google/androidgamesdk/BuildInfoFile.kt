package com.google.androidgamesdk

import com.google.gson.GsonBuilder
import java.nio.file.Paths
import java.util.concurrent.TimeUnit
import kotlin.collections.List

class BuildInfoFile {
    private data class ArtifactBuildInfo(
        val groupId: String,
        val artifactId: String,
        val version: String,
        val path: String = "",
        val sha: String,
        val groupZipPath: String = "",
        val projectZipPath: String,
        val groupIdRequiresSameVersion: Boolean = false,
        val dependencies: List<Dependency> = arrayListOf(),
        val checks: List<Check> = arrayListOf()
    ) {
        data class Dependency(
            val groupId: String,
            val artifactId: String,
            val version: String,
            val isTipOfTree: Boolean
        )

        data class Check(
            val name: String,
            val passing: Boolean
        )
    }

    private data class ArtifactsBuildInfo(
        val artifacts: List<ArtifactBuildInfo>
    )

    fun getGitCommitShaAtHead(): String {
        val parts = "git rev-parse HEAD".split("\\s".toRegex())
        val proc = ProcessBuilder(*parts.toTypedArray())
            .redirectOutput(ProcessBuilder.Redirect.PIPE)
            .redirectError(ProcessBuilder.Redirect.PIPE)
            .start()

        proc.waitFor(10, TimeUnit.SECONDS)
        return proc.inputStream.bufferedReader().readText().trim()
    }

    fun writeLibrariesBuildInfoFile(
        nativeLibraries: Collection<NativeLibrary>,
        androidLibraries: Collection<AndroidLibrary>,
        distPath: String,
        packageName: String
    ) {
        val headCommitSha = getGitCommitShaAtHead()

        val artifactsBuildInfo: List<ArtifactBuildInfo> = nativeLibraries.map {
            nativeLibrary: NativeLibrary ->
            ArtifactBuildInfo(
                groupId = "androidx.games",
                artifactId = nativeLibrary.aarLibraryName,
                path = "/src/" + nativeLibrary.nativeLibraryName,
                projectZipPath = packageName + '/' +
                    nativeLibrary.aarLibraryName + "-maven-zip.zip",
                sha = headCommitSha,
                version = nativeLibrary.aarVersion
            )
        } + androidLibraries.map {
            androidLibrary: AndroidLibrary ->
            ArtifactBuildInfo(
                groupId = "androidx.games",
                artifactId = androidLibrary.aarLibraryName,
                path = androidLibrary.projectName,
                projectZipPath = packageName + '/' +
                    androidLibrary.aarLibraryName + "-maven-zip.zip",
                sha = headCommitSha,
                version = androidLibrary.aarVersion
            )
        }

        val gson = GsonBuilder().setPrettyPrinting().create()
        val serializedInfo: String = gson.toJson(
            ArtifactsBuildInfo(
                artifacts = artifactsBuildInfo
            )
        )
        val outputFile = Paths.get(
            distPath,
            "androidx_aggregate_build_info.txt"
        ).toFile()
        outputFile.writeText(serializedInfo)
    }
}
