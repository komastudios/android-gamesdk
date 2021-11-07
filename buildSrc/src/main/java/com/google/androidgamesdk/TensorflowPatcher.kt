package com.google.androidgamesdk

import net.lingala.zip4j.ZipFile
import org.gradle.api.Project
import java.io.File
import java.io.FileOutputStream
import java.net.URL

/**
 * Patches the Tensorflow library to work with the Game SDK.
 */
class TensorflowPatcher(val project: Project) {
    fun patch() {
        patchAbseilCpp()
        patchTensorflow()
    }

    /**
     * Abseil-cpp library doesn't compile with clang++ provided because of
     * a wrong set of flags. Remove the incompatible flag.
     */
    private fun patchAbseilCpp() {
        val generatedCopts = project.file("../external/abseil-cpp/absl/copts/GENERATED_AbseilCopts.cmake")
        var generatedCoptsContent = generatedCopts.readText()

        generatedCoptsContent = generatedCoptsContent.replace("\"-Wno-implicit-int-float-conversion\"", "");
        generatedCopts.writeText(generatedCoptsContent);
    }

    /**
     * Tensorflow doesn't compile because it uses a different "farmhash"
     * library than the one provided during compilation. Fix the broken headers
     * and namespaces.
     */
    private fun patchTensorflow() {
        val nnapiDelegate = project.file("../external/tensorflow/tensorflow/lite/delegates/nnapi/nnapi_delegate.cc")
        var nnapiDelegateContent = nnapiDelegate.readText()

        nnapiDelegateContent = nnapiDelegateContent.replace("\"utils/hash/farmhash.h\"", "\"farmhash.h\"");
        nnapiDelegate.writeText(nnapiDelegateContent);

        val lshProjection = project.file("../external/tensorflow/tensorflow/lite/kernels/lsh_projection.cc")
        var lshProjectionContent = lshProjection.readText()

        lshProjectionContent = lshProjectionContent.replace("\"utils/hash/farmhash.h\"", "\"farmhash.h\"");
        lshProjectionContent = lshProjectionContent.replace("farmhash::Fingerprint64", "::NAMESPACE_FOR_HASH_FUNCTIONS::Fingerprint64");
        lshProjection.writeText(lshProjectionContent);

        val serialization = project.file("../external/tensorflow/tensorflow/lite/delegates/serialization.cc")
        var serializationContent = serialization.readText()

        serializationContent = serializationContent.replace("\"utils/hash/farmhash.h\"", "\"farmhash.h\"");
        serialization.writeText(serializationContent);
    }
}
