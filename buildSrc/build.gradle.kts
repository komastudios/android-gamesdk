repositories {
    jcenter()
}

plugins {
    // Activate Kotlin support
    `kotlin-dsl`
}

dependencies {
    // Use the Kotlin test library.
    testImplementation(kotlin("test"))

    // Use the Kotlin JUnit integration.
    testImplementation(kotlin("test-junit"))
}