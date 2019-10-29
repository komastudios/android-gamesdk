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

import java.lang.reflect.Constructor;

public class Factory {

    private static final String OPERATION_PACKAGE =
        "com.google.gamesdk.gamecert.operationrunner.operations.";

    /**
     * Instantiate an operation by class name
     * @param suiteId the suite the operation will be a part of once instantiated
     * @param operationId the name of the operation class
     * @param configurationJson the operation's configuration json string
     * @param context an android context
     * @param mode the operation's mode at run time
     * @return a new operation instance, or null
     */
    public static BaseOperation create(String suiteId,
                                       String operationId,
                                       String configurationJson,
                                       Context context,
                                       BaseOperation.Mode mode) {
        try {
            Class<?> clazz = Class.forName(OPERATION_PACKAGE + operationId);

            Constructor<?> ctor = clazz.getConstructor(String.class,
                    String.class,
                    Context.class,
                    BaseOperation.Mode.class);

            Object object = ctor.newInstance(suiteId,
                    configurationJson,
                    context, mode);

            return (BaseOperation) object;
        } catch (Exception ignored) {
            // Failure to load operation is OK; responsibility lies with
            // BaseHostActivity::OperationWrapper
        }

        return null;
    }
}
