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

import com.google.androidcerttest.transport.Datum;
import com.google.androidcerttest.util.NativeInvoker;

public abstract class BaseOperation {

    public enum Mode {
        DATA_GATHERER(0),
        STRESSOR(1);

        private final int _value;

        Mode(int value) {
            this._value = value;
        }

        public int getValue() {
            return _value;
        }
    }

    private String _operationId;
    private String _suiteId;
    private String _configurationJson;
    private Context _context;
    private boolean _stopped = false;
    private Mode _mode;

    BaseOperation(String operationId, String suiteId, String configurationJson, Context context, Mode mode) {
        this._operationId = operationId;
        this._suiteId = suiteId;
        this._configurationJson = configurationJson;
        this._context = context;
        this._mode = mode;
    }

    public String getOperationId() {
        return _operationId;
    }

    public String getSuiteId() {
        return _suiteId;
    }

    public String getConfigurationJson() {
        return _configurationJson;
    }

    public Context getContext() {
        return _context;
    }

    public Mode getMode() {
        return _mode;
    }

    /**
     * Asynchronously start the operation running.
     *
     * @param runForDurationMillis if > 0, run for this many milliseconds, if 0, run indefinitely (or until stop() is called)
     */
    public abstract void start(long runForDurationMillis);

    /**
     * Wait (blocking) for the operation to complete. If the operation was started with a duration, this will wait for the
     * duration to expire. Otherwise, any operation must be cancelable via calling stop(). So an operation which was
     * started with a duration of zero will block here unless stop() has been called.
     */
    public abstract void waitForCompletion();

    /**
     * Signal immediate termination of the operation.
     */
    public synchronized void stop() {
        _stopped = true;
    }

    public synchronized boolean isStopped() {
        return _stopped;
    }

    protected void report(Object info) {
        if (_mode == Mode.DATA_GATHERER) {
            NativeInvoker.writeToReportFile(Datum.create(_suiteId, _operationId, info).toJson());
        }
    }
}
