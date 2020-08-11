/*
 * Copyright (C) 2020 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License
 */

package Files;

import com.google.common.base.Preconditions;
import com.google.common.collect.ImmutableList;
import com.google.common.io.Files;
import com.google.protobuf.DescriptorProtos.FileDescriptorProto;
import com.google.protobuf.DescriptorProtos.FileDescriptorSet;
import com.google.protobuf.Descriptors.DescriptorValidationException;
import com.google.protobuf.Descriptors.FileDescriptor;
import com.google.protobuf.InvalidProtocolBufferException;
import com.intellij.util.PathUtil;
import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.util.Optional;
import java.util.logging.Logger;
import java.util.zip.ZipEntry;
import java.util.zip.ZipFile;
import org.apache.commons.io.FileUtils;

public class ProtoCompiler {
    private static final Logger logger = Logger.getLogger(ProtoCompiler.class.getName());
    private static File protocBinary = null;
    private final FileDescriptor[] emptyDeps = new FileDescriptor[0];

    public ProtoCompiler() throws IOException {
        if (protocBinary == null) {
            protocBinary = getProtoExecutableCopy(getProtoBinaryFile());
        }
    }

    public ProtoCompiler(File protoFile) {
        protocBinary = protoFile;
    }

    private File getProtoExecutableCopy(File protoFile) throws IOException {
        File protocTempExecutable = File.createTempFile(protoFile.getName(), "");
        protocTempExecutable.deleteOnExit();
        protocTempExecutable.setExecutable(true);
        File jarFile = protoFile.getParentFile();
        try (ZipFile zipFile = new ZipFile(jarFile)) {
            ZipEntry zipEntry = zipFile.getEntry(protoFile.getName());
            try (InputStream inputStream = zipFile.getInputStream(zipEntry)) {
                FileUtils.copyInputStreamToFile(inputStream, protocTempExecutable);
            }
        }
        return protocTempExecutable;
    }

    private File getProtoBinaryFile() {
        File protoBinary = null;
        String path = PathUtil.getJarPathForClass(ProtoCompiler.class);
        switch (OsUtils.getOS()) {
            case WINDOWS:
                protoBinary = new File(path, "protoc-3.12.3-win64.exe");
                logger.info("Protoc found successfully");
                break;
            case LINUX:
                protoBinary = new File(path, "protoc-3.12.3-linux-x86_64");
                logger.info("Protoc found successfully");
                break;
            case MAC:
                protoBinary = new File(path, "protoc-3.12.3-osx-x86_64");
                logger.info("Protoc found successfully");
                break;
            default:
                logger.severe("Can not find protoc binary");
        }
        return protoBinary;
    }

    public FileDescriptor compile(File file, Optional<File> outFile)
        throws IOException, CompilationException {
        Preconditions.checkNotNull(file, "file");
        FileDescriptor descriptor = buildAndRunCompilerProcess(file, outFile);
        return descriptor;
    }

    public byte[] runCommand(ProcessBuilder processBuilder)
        throws IOException, CompilationException {
        Process process = processBuilder.start();
        try {
            process.waitFor();
        } catch (InterruptedException e) {
            throw new CompilationException("Process was interrupted", e);
        }
        InputStream stdin = process.getInputStream();
        byte[] result = new byte[stdin.available()];
        stdin.read(result);
        return result;
    }

    private FileDescriptor buildAndRunCompilerProcess(File file, Optional<File> outFile)
        throws IOException, CompilationException {
        File tempOutFile = File.createTempFile("temp", "txt");
        ImmutableList<String> commandLine = createCommandLine(file, tempOutFile);
        runCommand(new ProcessBuilder(commandLine));
        byte[] result = Files.toByteArray(tempOutFile);
        tempOutFile.delete();

        try {
            FileDescriptorSet fileSet = FileDescriptorSet.parseFrom(result);
            if (outFile.isPresent()) {
                Files.write(fileSet.toByteArray(), outFile.get());
            }
            for (FileDescriptorProto descProto : fileSet.getFileList()) {
                if (descProto.getName().equals(file.getName())) {
                    return FileDescriptor.buildFrom(descProto, emptyDeps);
                }
            }
        } catch (DescriptorValidationException | InvalidProtocolBufferException e) {
            throw new IllegalStateException(e);
        }
        throw new CompilationException(
            String.format("Descriptor for [%s] does not exist.", file.getName()));
    }

    /* Decode textproto message from text(textprotoFile) into binary(binFile) */
    public File encodeFromTextprotoFile(String message, File protoFile, File textprotoFile,
        String binaryPath, Optional<File> errorFile) throws IOException, CompilationException {
        ImmutableList<String> command = encodeCommandLine(message, protoFile);

        File binFile = new File(binaryPath);

        ProcessBuilder builder =
            new ProcessBuilder(command).redirectInput(textprotoFile).redirectOutput(binFile);

        if (errorFile.isPresent()) {
            builder.redirectError(errorFile.get());
        }
        System.out.println(command.toString());
        runCommand(builder);
        return binFile;
    }

    /* Decode textproto message from binary(binFile) into text(textFile) */
    public File decodeToTextprotoFile(String message, File protoFile, String textprotoPath,
        File binFile, Optional<File> errorFile) throws IOException, CompilationException {
        ImmutableList<String> command = decodeCommandLine(message, protoFile);

        File textprotoFile = new File(textprotoPath);

        ProcessBuilder builder =
            new ProcessBuilder(command).redirectInput(binFile).redirectOutput(textprotoFile);

        if (errorFile.isPresent()) {
            builder.redirectError(errorFile.get());
        }

        runCommand(builder);
        return textprotoFile;
    }

    private ImmutableList<String> createCommandLine(File file, File outFile) {
        return ImmutableList.of(protocBinary.getAbsolutePath(), "-o", outFile.getAbsolutePath(),
            "-I",
            file.getName() + "=" + file.getAbsolutePath(), // That should be one line
            file.getAbsolutePath());
    }

    private ImmutableList<String> encodeCommandLine(String message, File protoFile) {
        return ImmutableList.of(protocBinary.getAbsolutePath(), "--encode=" + message, "-I",
            protoFile.getName() + "=" + protoFile.getAbsolutePath(), // That should be one line
            protoFile.getAbsolutePath());
    }

    private ImmutableList<String> decodeCommandLine(String message, File protoFile) {
        return ImmutableList.of(protocBinary.getAbsolutePath(), "--decode=" + message, "-I",
            protoFile.getName() + "=" + protoFile.getAbsolutePath(), // That should be one line
            protoFile.getAbsolutePath());
    }
}
