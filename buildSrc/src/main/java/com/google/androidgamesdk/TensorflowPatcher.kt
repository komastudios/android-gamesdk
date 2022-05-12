package com.google.androidgamesdk

import net.lingala.zip4j.ZipFile
import org.gradle.api.Project
import java.io.File
import java.io.FileOutputStream
import java.net.URL

/**
 * Patches the Tensorflow library and its dependencies to work with the Game SDK.
 */
class TensorflowPatcher(val project: Project) {
    fun patch() {
        patchAbseilCpp()
        patchTensorflow()
        patchCmakeDependencies()
        patchXnnpack()
        patchFarmhash()
    }

    /**
     * Abseil-cpp library doesn't compile with clang++ provided because of
     * a wrong set of flags. Remove the incompatible flag.
     */
    private fun patchAbseilCpp() {
        val generatedCopts = project.file(
                "../external/abseil-cpp/absl/copts/GENERATED_AbseilCopts.cmake"
        )
        var generatedCoptsContent = generatedCopts.readText()

        generatedCoptsContent = generatedCoptsContent.replace(
                "\"-Wno-implicit-int-float-conversion\"",
                ""
        );
        generatedCopts.writeText(generatedCoptsContent);
    }

    /**
     * Tensorflow doesn't compile because it uses a different "farmhash"
     * library than the one provided during compilation. Fix the broken headers
     * and namespaces.
     */
    private fun patchTensorflow() {
        val nnapiDelegate = project.file(
                "../external/tensorflow/tensorflow/lite/" +
                        "delegates/nnapi/nnapi_delegate.cc")
        var nnapiDelegateContent = nnapiDelegate.readText()

        nnapiDelegateContent = nnapiDelegateContent.replace(
                "\"utils/hash/farmhash.h\"",
                "\"farmhash.h\""
        );
        nnapiDelegate.writeText(nnapiDelegateContent);

        val lshProjection = project.file(
                "../external/tensorflow/tensorflow/lite/" +
                        "kernels/lsh_projection.cc")
        var lshProjectionContent = lshProjection.readText()

        lshProjectionContent = lshProjectionContent.replace(
                "\"utils/hash/farmhash.h\"",
                "\"farmhash.h\""
        );
        lshProjectionContent = lshProjectionContent.replace(
                "farmhash::Fingerprint64",
                "::NAMESPACE_FOR_HASH_FUNCTIONS::Fingerprint64");
        lshProjection.writeText(lshProjectionContent);

        val serialization = project.file(
                "../external/tensorflow/tensorflow/lite/" +
                        "delegates/serialization.cc")
        var serializationContent = serialization.readText()

        serializationContent = serializationContent.replace(
                "\"utils/hash/farmhash.h\"",
                "\"farmhash.h\""
        );
        serializationContent = serializationContent.replace(
                "O_RDONLY | O_CLOEXEC,",
                "O_RDONLY | O_CLOEXEC | O_CREAT,"
        );
        serialization.writeText(serializationContent);
    }

    /**
     * Tensorflow depends on github downloads to populate its dependencies.
     *  Override the github downloader to use libraries from external instead.
     */
    private fun patchCmakeDependencies() {

        val overridableFetchContent = project.file(
            "../external/tensorflow/tensorflow/lite/" +
                "tools/cmake/modules/OverridableFetchContent.cmake");
        val externalDependencies = project.file(
            "src/memory_advice/ExternalDependencies.cmake");
        overridableFetchContent.writeText(externalDependencies.readText());

        val farmhashCMake = project.file(
            "../external/tensorflow/tensorflow/lite/" +
                "tools/cmake/modules/farmhash/CMakeLists.txt");
        var farmhashCMakeContent = farmhashCMake.readText()
        farmhashCMakeContent = farmhashCMakeContent.replace("/src", "");
        farmhashCMake.writeText(farmhashCMakeContent);

        val farmhashDownloader = project.file(
            "../external/tensorflow/tensorflow/lite/" +
                "tools/cmake/modules/farmhash.cmake");
        var farmhashDownloaderContent = farmhashDownloader.readText()
        farmhashDownloaderContent = farmhashDownloaderContent.replace(
            ")\n" +
                "\n" +
                "add_subdirectory(",
            ")\n" +
                "set (farmhash_SOURCE_DIR \"\${EXTERNAL_SOURCE_DIR}/" +
                "libtextclassifier/native/utils/hash\")\n" +
                "set (FARMHASH_SOURCE_DIR \"\${farmhash_SOURCE_DIR}\")\n" +
                "add_subdirectory(");
        farmhashDownloader.writeText(farmhashDownloaderContent);

        val fft2dDownloader = project.file(
            "../external/tensorflow/tensorflow/lite/" +
                "tools/cmake/modules/fft2d.cmake");
        var fft2dDownloaderContent = fft2dDownloader.readText()
        fft2dDownloaderContent = fft2dDownloaderContent.replace(
            "source\")\n" +
                "add_subdirectory(",
            "source\")\n" +
                "set(fft2d_SOURCE_DIR " +
                "\"\${fft2d_SOURCE_DIR}/src/fft2d/fft2d\")\n" +
                "set(FFT2D_SOURCE_DIR \"\${fft2d_SOURCE_DIR}\")\n" +
                "add_subdirectory(");
        fft2dDownloader.writeText(fft2dDownloaderContent);

        val flatbuffersCMake = project.file(
            "../external/tensorflow/tensorflow/lite/" +
                "tools/cmake/modules/flatbuffers.cmake");
        var flatbuffersCMakeContent = flatbuffersCMake.readText()
        flatbuffersCMakeContent = flatbuffersCMakeContent.replace(
            "include(ExternalProject)\n" +
                "\n" +
                "ExternalProject_Add(flatbuffers-flatc\n" +
                "  PREFIX \${CMAKE_BINARY_DIR}/flatbuffers-flatc\n" +
                "  SOURCE_DIR \${CMAKE_BINARY_DIR}/flatbuffers\n" +
                "  CMAKE_ARGS -DCMAKE_CXX_FLAGS=\"-DNOMINMAX=1\"\n" +
                "             -DFLATBUFFERS_BUILD_TESTS=OFF\n" +
                "             -DFLATBUFFERS_BUILD_FLATLIB=OFF\n" +
                "             -DFLATBUFFERS_STATIC_FLATC=ON\n" +
                "             -DFLATBUFFERS_BUILD_FLATHASH=OFF\n" +
                "             -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>\n" +
                "  EXCLUDE_FROM_ALL 1\n" +
                ")",
            "");
        flatbuffersCMake.writeText(flatbuffersCMakeContent);
    }

    /**
    * The xnnpack library on platform/external is updated; fix its cmake file.
    */
    private fun patchXnnpack() {
        val xnnpackCmake = project.file("../external/XNNPACK/CMakeLists.txt");
        var xnnpackCmakeContent = xnnpackCmake.readText()
        xnnpackCmakeContent = xnnpackCmakeContent.replace(
            "\n" +
                "  src/f16-maxpool/9p8x-minmax-f16c-c8.c\n" +
                "  src/f16-prelu/gen/neonfp16arith-2x16.c\n" +
                "  src/f16-vbinary/gen/vadd-minmax-f16c-x16.c",
            "\n" +
                "  src/f16-maxpool/9p8x-minmax-f16c-c8.c\n" +
                "  src/f16-vbinary/gen/vadd-minmax-f16c-x16.c");
        xnnpackCmake.writeText(xnnpackCmakeContent);
    }

    /**
    * The farmhash library's structure doesn't match the expected structure
    * from tensorflow, fix the headers
    */
    private fun patchFarmhash() {
        val farmhash = project.file(
            "../external/libtextclassifier/native/utils/hash/farmhash.cc");
        var farmhashContent = farmhash.readText()
        farmhashContent = farmhashContent.replace(
            "#include \"utils/hash/farmhash.h\"",
            "#include \"farmhash.h\"");
        farmhash.writeText(farmhashContent);
  }
}
