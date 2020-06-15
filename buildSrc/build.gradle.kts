repositories {
    jcenter()
}

plugins {
    // Activate Kotlin support
    `kotlin-dsl`
}

val ktlint by configurations.creating

dependencies {
    // Use the Kotlin test library.
    testImplementation(kotlin("test"))

    // Use the Kotlin JUnit integration.
    testImplementation(kotlin("test-junit"))

    // Use ktlint for auto-formatting and linting
    ktlint("com.pinterest:ktlint:0.37.2")
}

// Ktlint tasks:
val ktlintOutputDir = "${project.buildDir}/reports/ktlint/"
val ktlintInputFiles = project.fileTree(mapOf("dir" to "src", "include" to "**/*.kt"))

val ktlintCheck by tasks.creating(JavaExec::class) {
    inputs.files(ktlintInputFiles)
    outputs.dir(ktlintOutputDir)

    description = "Check Kotlin code style."
    classpath = ktlint
    main = "com.pinterest.ktlint.Main"
    args = listOf("src/**/*.kt")
}

val ktlintFormat by tasks.creating(JavaExec::class) {
    inputs.files(ktlintInputFiles)
    outputs.dir(ktlintOutputDir)

    description = "Fix Kotlin code style deviations."
    classpath = ktlint
    main = "com.pinterest.ktlint.Main"
    args = listOf("-F", "src/**/*.kt")
}