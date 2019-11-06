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

package com.google.gamesdk.gamecert.operationrunner.transport;

import android.content.Context;
import android.text.TextUtils;
import android.util.Log;

import com.google.gamesdk.gamecert.operationrunner.util.TimeParsing;
import com.google.gson.Gson;
import com.google.gson.annotations.SerializedName;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.Reader;
import java.io.StringWriter;
import java.io.UnsupportedEncodingException;
import java.io.Writer;
import java.nio.charset.StandardCharsets;
import java.util.Arrays;

/**
 * Describes a set of stress tests to execute.
 * Intended to be loaded from JSON; may be
 * read over the network, compiled into the app, etc
 */
public class Configuration {

    private static final String TAG = "Configuration";
    private static final Gson _gson = new Gson();

    @SuppressWarnings("unused")
    public static class Operation {
        private boolean enabled = true;
        private String id;
        private Object configuration;

        public boolean isEnabled() {
            return enabled;
        }

        public String getId() {
            return id;
        }

        public String getConfigurationJson() {
            return _gson.toJson(configuration);
        }
    }

    public static class StressTest {
        private boolean enabled = true;

        // display name of the StressTest
        private String name;

        // longish description of what the stress test will do
        private String description;

        // "host", e.g., the activity which will be
        // launched to execute the stressors/tests
        private String host;

        // list of operations to run as silent stressors
        private Operation[] stressors;

        private Operation[] monitors;

        // operation to run as instrumented data gatherer
        @SerializedName("data_gatherer")
        private Operation dataGatherer;

        @SerializedName("duration")
        private String duration;

        public String getName() {
            return name;
        }

        public String getDescription() {
            return description;
        }

        public boolean isEnabled() {
            return enabled;
        }

        public String getHost() {
            return host;
        }

        public Operation[] getStressors() {
            return stressors;
        }

        public Operation getDataGatherer() {
            return dataGatherer;
        }

        public Operation[] getMonitors() {
            return monitors;
        }

        public int getDurationMillis() {
            try {
                return (int) TimeParsing.parseDurationString(
                        duration,
                        TimeParsing.Unit.Milliseconds);
            } catch (TimeParsing.BadFormatException bfe) {
                Log.e(TAG, "Unable to parse duration; error: "
                        + bfe.getLocalizedMessage());

                return 0;
            }
        }

        public String toJson() {
            return _gson.toJson(this);
        }

        public static StressTest fromJson(String json) {
            return _gson.fromJson(json, StressTest.class);
        }
    }

    @SerializedName("auto_run")
    private boolean autoRun;

    @SerializedName("delay_between_tests_millis")
    private int delayBetweenTestsMillis;

    @SerializedName("stress_tests")
    private StressTest[] stressTests;

    private String host;

    private Operation[] defaultMonitors;

    /**
     * If true, when MainActivity starts and reads this configuration,
     * it will immediately start executing the StressTests one by one
     * until exhausted, then will call stop()
     */
    public boolean isAutoRun() {
        return autoRun;
    }

    public void setAutoRun(boolean autoRun) {
        this.autoRun = autoRun;
    }

    /**
     * Delay (in milliseconds) to allow the device to "breath"
     * between execution of stress tests
     */
    public int getDelayBetweenTestsMillis() {
        return delayBetweenTestsMillis > 0 ? delayBetweenTestsMillis : 0;
    }

    public String getHost() {
        return host;
    }

    /**
     * The stress tests to execute
     */
    public StressTest[] getStressTests() {
        return stressTests;
    }

    /**
     * @return array of default monitors to inject into a StressTest
     * if it hasn't specified its own
     */
    public Operation[] getDefaultMonitors() {
        return defaultMonitors;
    }

    /**
     * Load a Configuration instance from a JSON string
     *
     * @param jsonString the configuration JSON data
     * @return a Configuration instance, or null if json is invalid
     */
    public static Configuration loadFromJson(String jsonString) {
        Configuration c = _gson.fromJson(jsonString, Configuration.class);

        // filter out disabled stress tests
        c.stressTests = Arrays.stream(c.stressTests)
                .filter((st) -> st.enabled)
                .toArray(StressTest[]::new);

        // for each enabled stress filter out disabled stressors
        for (StressTest st : c.stressTests) {
            st.stressors = Arrays.stream(st.stressors)
                    .filter((s) -> s.enabled)
                    .toArray(Operation[]::new);
            if (TextUtils.isEmpty(st.host)) {
                st.host = c.host;
            }

            if (st.getMonitors() == null || st.getMonitors().length == 0) {
                st.monitors = c.defaultMonitors;
            }
        }

        return c;
    }

    /**
     * Load a Configuration from a JSON resource compiled into the app bundle
     *
     * @param ctx            an Android Context
     * @param jsonResourceId the id of the JSON resource, ie R.raw.configuration_json
     * @return The Configuration, or null if resource isn't valid
     */
    public static Configuration loadFromJsonResource(Context ctx,
                                                     int jsonResourceId) {
        try (InputStream is = ctx.getResources()
                .openRawResource(jsonResourceId)) {

            Writer writer = new StringWriter();
            char[] buffer = new char[1024];
            Reader reader = new BufferedReader(
                    new InputStreamReader(is, StandardCharsets.UTF_8));
            int n;
            while ((n = reader.read(buffer)) != -1) {
                writer.write(buffer, 0, n);
            }
            return loadFromJson(writer.toString());
        } catch (UnsupportedEncodingException e) {
            Log.d(TAG, "JSON Resource was not a properly encoded JSON file"
                    + ", error: " + e.getLocalizedMessage());
            e.printStackTrace();
        } catch (IOException e) {
            Log.d(TAG, "Unable to open json resource for reading, error: "
                    + e.getLocalizedMessage());
            e.printStackTrace();
        }
        return null;
    }
}
