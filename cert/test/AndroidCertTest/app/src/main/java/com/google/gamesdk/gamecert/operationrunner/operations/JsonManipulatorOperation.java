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

package com.google.gamesdk.gamecert.operationrunner.operations;

import android.content.Context;
import android.os.SystemClock;
import android.os.Trace;
import android.util.Log;

import com.google.gson.Gson;
import com.google.gson.annotations.SerializedName;

import java.io.IOException;
import java.io.InputStream;
import java.util.Arrays;

public class JsonManipulatorOperation extends BaseOperation {

    private static final String TAG = JsonManipulatorOperation
            .class.getSimpleName();

    private static class Configuration {
        String json_asset_name;
        long performance_sample_period_millis;
    }

    private Configuration _configuration;
    private Gson _gson;
    private Thread _thread;

    public JsonManipulatorOperation(String suiteId,
                                    String configurationJson,
                                    Context context,
                                    Mode mode) {
        super("JsonManipulatorOperation",
                suiteId,
                configurationJson,
                context,
                mode);

        _gson = new Gson();
        _configuration = _gson.fromJson(configurationJson, Configuration.class);
    }

    @Override
    public void start(long runForDurationMillis) {
        final boolean isTest = getMode() == Mode.DATA_GATHERER;

        _thread = new Thread(() -> {
            String userDataJsonString = loadString(_configuration.json_asset_name);
            if (userDataJsonString == null) {
                throw new IllegalStateException("JsonManipulatorOperation::run" +
                        " - unable to load json asset \""
                        + _configuration.json_asset_name + "\"");
            }

            long totalBytes = userDataJsonString.getBytes().length;
            long startTimeMillis = SystemClock.elapsedRealtime();
            long iterations = 0;

            Datum datum = new Datum(0);

            while (!isStopped()) {

                userDataJsonString = work(userDataJsonString);
                iterations++;
                datum.iterations++;
                totalBytes += userDataJsonString.getBytes().length;

                long nowMillis = SystemClock.elapsedRealtime();
                long elapsedMillis = nowMillis - startTimeMillis;

                if (isTest) {
                    if (elapsedMillis - datum.startTimeMillis >
                            _configuration.performance_sample_period_millis) {
                        datum.endTimeMillis = elapsedMillis;
                        report(datum);
                        datum = new Datum(elapsedMillis);
                    }
                }

                if (runForDurationMillis > 0 && elapsedMillis >=
                        runForDurationMillis) {
                    break;
                }
            }
        });

        _thread.start();
    }

    @Override
    public void waitForCompletion() {
        try {
            _thread.join();
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }

    private String work(String userDataJsonString) {
        try {
            Trace.beginSection("JsonManipulatorOperation::work");
            UserDatum[] data = loadUserData(userDataJsonString);
            data = frobulateUserData(data);
            return encodeUserData(data);
        } finally {
            Trace.endSection();
        }
    }

    class Datum {
        long iterations;

        @SerializedName("start_time_millis")
        long startTimeMillis;

        @SerializedName("end_time_millis")
        long endTimeMillis;

        Datum(long startTimeMillis) {
            this.startTimeMillis = startTimeMillis;
        }
    }


    private String loadString(String assetName) {
        try (InputStream stream = getContext().getAssets().open(assetName)) {
            int size = stream.available();
            byte[] buffer = new byte[size];
            if (stream.read(buffer) == size) {
                return new String(buffer);
            }
        } catch (IOException e) {
            Log.e(TAG, "Unable to open asset \""
                    + assetName + "\" for reading");
        }

        return null;
    }

    private UserDatum[] loadUserData(String jsonString) {
        return _gson.fromJson(jsonString, UserDatum[].class);
    }

    private String encodeUserData(UserDatum[] userData) {
        return _gson.toJson(userData);
    }

    //
    //  "frobulate" functions - these just munge our dataset in nonsensical ways
    //

    private UserDatum[] frobulateUserData(UserDatum[] datums) {
        return Arrays.stream(datums)
                .map(UserDatum::frobulate)
                .toArray(UserDatum[]::new);
    }

    private static String frobulateString(String s) {
        // just make new string with first letter moved to
        // end: "frobulate" -> "robulatef"
        return s.substring(1) + s.substring(0, 1);
    }

    private static String[] frobulateStrings(String[] strs) {
        return Arrays.stream(strs)
                .map(JsonManipulatorOperation::frobulateString)
                .toArray(String[]::new);
    }

    private static FriendDatum[] frobulateFriends(FriendDatum[] friends) {
        return Arrays.stream(friends)
                .map(FriendDatum::frobulate)
                .toArray(FriendDatum[]::new);
    }

    //
    //  FriendDatum and UserDatum map directly to the data
    //  represented in assets/JsonManipulatorOperation.json
    //

    private static class FriendDatum {
        int id;
        String name;

        FriendDatum frobulate() {
            FriendDatum frobulated = new FriendDatum();
            frobulated.id = id + 1;
            frobulated.name = frobulateString(name);
            return frobulated;
        }
    }

    private static class UserDatum {
        String _id;
        int index;
        String guid;
        boolean isActive;
        String balance;
        String picture;
        int age;
        String eyeColor;
        String name;
        String gender;
        String company;
        String email;
        String phone;
        String address;
        String about;
        String registered;
        double latitude;
        double longitude;
        String[] tags;
        FriendDatum[] friends;
        String greeting;
        String favoriteFruit;

        UserDatum frobulate() {
            UserDatum f = new UserDatum();
            f._id = frobulateString(_id);
            f.index = index + 1;
            f.guid = frobulateString(guid);
            f.isActive = !isActive;
            f.balance = frobulateString(balance);
            f.picture = frobulateString(picture);
            f.age = age + 1;
            f.eyeColor = frobulateString(eyeColor);
            f.name = frobulateString(name);
            f.gender = frobulateString(gender);
            f.company = frobulateString(company);
            f.email = frobulateString(email);
            f.phone = frobulateString(phone);
            f.address = frobulateString(address);
            f.about = frobulateString(about);
            f.registered = frobulateString(registered);
            f.latitude = latitude + 1;
            f.longitude = longitude + 1;
            f.tags = frobulateStrings(tags);
            f.friends = frobulateFriends(friends);
            f.greeting = frobulateString(greeting);
            f.favoriteFruit = frobulateString(favoriteFruit);

            return f;
        }
    }
}
