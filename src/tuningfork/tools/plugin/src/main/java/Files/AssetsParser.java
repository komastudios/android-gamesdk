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

import static java.nio.charset.StandardCharsets.UTF_8;

import java.io.File;
import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.Arrays;
import java.util.Comparator;
import java.util.List;
import java.util.Optional;
import java.util.logging.Logger;
import java.util.regex.Matcher;
import java.util.regex.Pattern;
import java.util.stream.Collectors;

public class AssetsParser {
    private static final Logger logger = Logger.getLogger(AssetsParser.class.getName());
    private Optional<File> devTuningForkFile = Optional.empty();
    private Optional<String> tuningForkSettings = Optional.empty();
    private Optional<List<File>> devFidelityParamFiles = Optional.empty();
    private File assetsDirectory;

    public AssetsParser(File directory) {
        this.assetsDirectory = directory;
    }

    private static int getDevFidelityFileNumber(File file) {
        Pattern p = Pattern.compile("[\\d]+");
        Matcher matcher = p.matcher(file.getName());
        if (matcher.find()) {
            return Integer.parseInt(matcher.group(0));
        }
        return 0;
    }

    public Optional<File> getDevTuningForkFile() {
        return devTuningForkFile;
    }

    public Optional<List<File>> getDevFidelityParamFiles() {
        return devFidelityParamFiles;
    }

    public Optional<String> getTuningForkSettings() {
        return tuningForkSettings;
    }

    public void parseFiles() throws IOException {
        devTuningForkFile = findDevTuningFork(assetsDirectory);
        devFidelityParamFiles = findDevFidelityParams(assetsDirectory);
        tuningForkSettings = findTuningForkSetting(assetsDirectory);
        if (devTuningForkFile.isPresent()) {
            logger.info(String.format("File %s exists: OK", FolderConfig.DEV_TUNINGFORK_PROTO));
        } else {
            logger.info(String.format("File %s exists: FAIL", FolderConfig.DEV_TUNINGFORK_PROTO));
        }
        if (tuningForkSettings.isPresent()) {
            logger.info(
                String.format("File %s exists: OK", FolderConfig.TUNINGFORK_SETTINGS_TEXTPROTO));
        } else {
            logger.info(
                String.format("File %s exists: FAIL", FolderConfig.TUNINGFORK_SETTINGS_TEXTPROTO));
        }
    }

    private Optional<File> findDevTuningFork(File folder) {
        File file = new File(folder, FolderConfig.DEV_TUNINGFORK_PROTO);
        if (!file.exists()) {
            return Optional.empty();
        }
        return Optional.of(file);
    }

    private Optional<String> findTuningForkSetting(File folder) throws IOException {
        File file = new File(folder, FolderConfig.TUNINGFORK_SETTINGS_TEXTPROTO);
        if (!file.exists()) {
            return Optional.empty();
        }
        String content =
            String.join("\n", Files.readAllLines(Paths.get(file.getAbsolutePath()), UTF_8));
        return Optional.of(content);
    }

    private Optional<List<File>> findDevFidelityParams(File folder) {
        Pattern devFidelityPattern = Pattern.compile(FolderConfig.DEV_FIDELITY_TEXTPROTO);
        File[] devFidelityFiles =
            folder.listFiles((dir, filename) -> devFidelityPattern.matcher(filename).find());
        if (devFidelityFiles == null || devFidelityFiles.length == 0) {
            return Optional.empty();
        } else {
            return Optional.of(
                Arrays.stream(devFidelityFiles)
                    .sorted(Comparator.comparingInt(file -> getDevFidelityFileNumber(file)))
                    .collect(Collectors.toList()));
        }
    }
}
