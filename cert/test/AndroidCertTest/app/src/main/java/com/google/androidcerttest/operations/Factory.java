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

package com.google.androidcerttest.operations;

import android.content.Context;

import java.lang.reflect.Constructor;

public class Factory {

    public static BaseOperation create(String suiteId, String operationId, String configurationJson, Context context, BaseOperation.Mode mode) {

        try {
            Class<?> clazz = Class.forName("com.google.androidcerttest.operations." + operationId);
            Constructor<?> ctor = clazz.getConstructor(String.class, String.class, Context.class, BaseOperation.Mode.class);
            Object object = ctor.newInstance(suiteId, configurationJson, context, mode);
            return (BaseOperation) object;
        } catch (Exception ignored) {
        }

        return null;
    }
}
