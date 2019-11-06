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

package com.google.gamesdk.gamecert.operationrunner.util;

/**
 * Helper class for parsing time durations such as "300ms" to desired time
 * units, such as TimeParsing.Unit.Seconds
 * TODO(shamyl@google.com): See if an existing Java/Android API can replace this
 */
public class TimeParsing {

    public static class BadFormatException extends Exception {
        BadFormatException(String message, Throwable cause) {
            super(message, cause);
        }
    }

    public enum Unit {
        Nanoseconds,
        Milliseconds,
        Seconds,
        Minutes
    }

    /**
     * Parses a duration description to a specific time unit, e.g, calling
     * parseDurationString("0.5m", Unit.Milliseconds) will return 30000.
     *
     * @param timeDesc  a duration string in format NUMBER%unit with optional
     *                  whitespace between. Supported units are "nanoseconds",
     *                  "ns", "milliseconds", "ms", "seconds", "sec", "s",
     *                  "minutes", and "m"
     * @param intoUnits the time unit desired to convert the input string to
     * @return the duration parsed and converted to the target units
     */
    public static double parseDurationString(String timeDesc, Unit intoUnits)
            throws BadFormatException {
        try {
            double nanoseconds = parseDurationStringToNs(timeDesc);
            switch (intoUnits) {
                case Nanoseconds:
                    return nanoseconds;
                case Milliseconds:
                    return nanoseconds / 1e6;
                case Seconds:
                    return nanoseconds / 1e9;
                case Minutes:
                    return nanoseconds / (60 * 1e9);
            }
        } catch (Exception e) {
            throw new BadFormatException("Unable to parse time \""
                    + timeDesc + " into units: \""
                    + intoUnits + "\"", e);
        }
        return 0;
    }

    private static double parseDurationStringToNs(String timeDesc) {
        timeDesc = timeDesc.trim();

        // special case; zero has no units
        if (timeDesc.equals("0")) return 0;

        String[] components = timeDesc.split("^\\d*\\.?\\d*");
        if (components.length != 2) {
            throw new IllegalArgumentException("Unrecognized time format \""
                    + timeDesc + "\"");
        }
        String units = components[1].trim();
        String timeStr = timeDesc.substring(0, timeDesc.length()
                - units.length()).trim();
        double value = Double.valueOf(timeStr);

        switch (units) {
            case "nanoseconds":
            case "ns":
                return value;

            case "milliseconds":
            case "ms":
                return value * 1e6;

            case "seconds":
            case "sec":
            case "s":
                return (value * 1e9);

            case "minutes":
            case "m":
                return (value * 60 * 1e9);

            default:
                throw new IllegalArgumentException("Unrecognized time unit \""
                        + units + "\"");
        }
    }
}
