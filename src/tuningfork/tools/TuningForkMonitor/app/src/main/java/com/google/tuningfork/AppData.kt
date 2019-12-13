package com.google.tuningfork

// Import settings from tuningfork.proto
import com.google.tuningfork.Tuningfork.Settings

// Decoded from dev_tuningfork.descriptor
data class FidelityParameter(val name: String, val type: String, val value: String)

data class Annotation(val name: String, val type: String, val value: Int)

data class DeserializedTuningForkDescriptor(val package_name: String,
                                            val enums: Map<String, List<String>>,
                                            val fidelityParameters: List<FidelityParameter>,
                                            val annotations: List<Annotation>)

// Info about and from the app
data class AppData(val desc: DeserializedTuningForkDescriptor,
                   val settings: Settings)
