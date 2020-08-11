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

import Files.Parser;
import java.io.IOException;
import org.junit.Assert;
import org.junit.Rule;
import org.junit.Test;
import org.junit.rules.TemporaryFolder;

public class ParserTest {
    @Rule public TemporaryFolder folder = new TemporaryFolder();

    @Test
    public void testParserValid() {
        Parser parser = new Parser(folder.getRoot());
        try {
            folder.newFile("dev_tuningfork.proto");
            folder.newFile("tuningfork_settings.txt");
            folder.newFile("dev_tuningfork_fidelityparams_1.txt");
            folder.newFile("dev_tuningfork_fidelityparams_2.txt");
            parser.parseFiles();
        } catch (IOException e) {
            e.printStackTrace();
        }
        Assert.assertTrue(parser.getDevTuningForkFile().isPresent());
        Assert.assertTrue(parser.getTuningForkSettings().isPresent());
        Assert.assertTrue(parser.getDevFidelityParamFiles().isPresent());
        Assert.assertEquals(parser.getDevFidelityParamFiles().get().size(), 2);
    }

    @Test
    public void testParserNoFiles() {
        Parser parser = new Parser(folder.getRoot());
        Assert.assertFalse(parser.getDevTuningForkFile().isPresent());
        Assert.assertFalse(parser.getTuningForkSettings().isPresent());
        Assert.assertFalse(parser.getDevFidelityParamFiles().isPresent());
    }

    @Test
    public void testParserInvalidFilesName() {
        Parser parser = new Parser(folder.getRoot());
        try {
            folder.newFile("dev_Tuning_Fork.proto");
            folder.newFile("settings_tuningfork.txt");
            folder.newFile("dev_tuningfork_fidelityparams_25.txt");
            folder.newFile("dev_tuningfork_2.txt");
            parser.parseFiles();
        } catch (IOException e) {
            e.printStackTrace();
        }
        Assert.assertFalse(parser.getDevTuningForkFile().isPresent());
        Assert.assertFalse(parser.getTuningForkSettings().isPresent());
        Assert.assertFalse(parser.getDevFidelityParamFiles().isPresent());
    }
}
