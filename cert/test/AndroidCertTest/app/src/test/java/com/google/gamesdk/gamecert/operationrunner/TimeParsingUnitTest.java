/*
 * Copyright 2019 The Android Open Source Project
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
 * limitations under the License.
 */

package com.google.gamesdk.gamecert.operationrunner;

import com.google.gamesdk.gamecert.operationrunner.util.TimeParsing;

import org.junit.Test;
import org.junit.runner.RunWith;
import org.junit.runners.Parameterized;

import java.util.Arrays;
import java.util.Collection;

import static com.google.gamesdk.gamecert.operationrunner.util.TimeParsing.Unit;
import static org.junit.Assert.*;

/*
 * Example local unit test, which will execute on the development machine (host).
 * see <a href="http://d.android.com/tools/testing">Testing documentation</a>
 */

@RunWith(Parameterized.class)
public class TimeParsingUnitTest {
    private String timeDesc;
    private Unit intoUnits;
    private double expected;

    public TimeParsingUnitTest(String timeDesc, Unit intoUnits, double expected){
        super();
        this.timeDesc = timeDesc;
        this.intoUnits = intoUnits;
        this.expected = expected;
    }

    @Parameterized.Parameters
    public static Collection input() {
        return Arrays.asList(new Object[][]{ {"0.00000005m", Unit.Nanoseconds, 3000},
            {"0.5m", TimeParsing.Unit.Milliseconds, 30000},
            {"0.5m", Unit.Seconds, 30},
            {"30000000ns", Unit.Minutes, 0.0005}});
    }

    @Test
    public void timeParsing_isCorrect() {
        try {
            assertEquals(expected, TimeParsing.parseDurationString(timeDesc, intoUnits), 0);
        } catch (TimeParsing.BadFormatException ex) {
            ex.printStackTrace();
            fail();
        }
    }
}