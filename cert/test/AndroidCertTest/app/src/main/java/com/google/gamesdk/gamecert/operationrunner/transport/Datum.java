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

import com.google.gamesdk.gamecert.operationrunner.util.NativeInvoker;
import com.google.gson.Gson;

public class Datum {

    private static Gson _gson = new Gson();

    String suite_id;

    String operation_id;

    long timestamp;

    int cpu_id;

    String thread_id;

    Object custom;

    private Datum() {
    }


    public static Datum create(String suiteId, String operationId, Object info) {
        Datum d = new Datum();
        d.suite_id = suiteId;
        d.operation_id = operationId;
        d.timestamp = NativeInvoker.getSteadyClockTimeNanos();
        d.cpu_id = NativeInvoker.getCpuId();
        d.thread_id = NativeInvoker.getThreadId();
        d.custom = info;
        return d;
    }

    public String toJson() {
        return _gson.toJson(this);
    }
}
