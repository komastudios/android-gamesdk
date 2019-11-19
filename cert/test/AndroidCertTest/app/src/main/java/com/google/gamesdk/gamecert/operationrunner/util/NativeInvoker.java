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

import android.view.Surface;

import com.google.gamesdk.gamecert.operationrunner.hosts.BaseGLHostActivity;
import com.google.gamesdk.gamecert.operationrunner.hosts.BaseHostActivity;
import com.google.gamesdk.gamecert.operationrunner.hosts.SwappyGLHostActivity;

public class NativeInvoker {
    static {
        System.loadLibrary("native-lib");
    }

    /**
     * Runs the native unit tests via gtest, returns true iff all tests passed
     */
    public static native boolean runUnitTests();

    public static native void initializeSuite(BaseHostActivity activity);

    public static native void shutdownSuite();

    /**
     * @return the current time in nanoseconds from the monotonic time source
     * used in c++ operations. Java time code should rely on this instead of
     * System.currentTime* functions to ensure we have the same clock sync
     */
    public static native long getSteadyClockTimeNanos();

    /**
     * @return the id of the cpu running the calling thread
     */
    public static native int getCpuId();

    /**
     * @return the id of the calling thread
     */
    public static native String getThreadId();

    public static native double getCurrentFps();

    public static native long getCurrentMinFrameTimeNs();

    public static native long getCurrentMaxFrameTimeNs();

    public static native void openReportFile(String pathToFile);

    public static native void openReportFileDescriptor(int fileDescriptor);

    public static native void closeReportFile();

    public static native void writeToReportFile(String msg);

    /**
     * Create an operation instance (from native-lib) by name.
     *
     * @param suiteId     the name of the test suite the operation will run in
     * @param operationId The class name of the operation to instantiate
     * @param mode        Mode to run operation in, corresponding to
     *                    BaseOperation.Mode enum
     * @return an id to refer to the operation when calling startOperation()
     * and stopOperation()
     */
    public static native int createOperation(String suiteId, String operationId,
                                             int mode);

    /**
     * start running an operation asynchronously
     *
     * @param id                 the operation id returned by createOperation
     * @param durationMillis     the number of milliseconds to run the operation
     *                           or zero to run it until calling stopOperation
     * @param jsonConfiguration: the operations's configuration json
     */
    public static native void startOperation(int id, long durationMillis,
                                             String jsonConfiguration);

    /**
     * Signal an operation to stop execution immediately
     *
     * @param id the operation id returned by createOperation
     */
    public static native void stopOperation(int id);

    /**
     * Check if the operation has determined it has finished its work
     *
     * @param id the operation id returned by createOperation
     */
    public static native boolean isOperationStopped(int id);

    /**
     * Waits for an operation to finish running.
     *
     * @param id the operation id returned by createOperation
     */
    public static native void waitForOperation(int id);

    /**
     * Invoked when the host activity receives an onTrimMemory call
     *
     * @param level the level received by the host activity
     */
    public static native void onTrimMemory(int level);

    /**
     * Get configuration for the gl context the data gathering operation requires
     * This is only called by GL operations hosted by GLSurfaceViewHostActivity and
     * SwappyGLHostActivity
     * @param c Configuration will be written into this
     * TODO(shamyl@google.com): This may eventually be refactored to support Vulkan hosts
     */
    public static native void getGLContextConfiguration(
        BaseGLHostActivity.GLContextConfiguration c);

    public static native void glSurfaceViewHost_ContextReady(
        BaseGLHostActivity.GLContextConfiguration c);

    public static native void glSurfaceViewHost_Draw();

    public static native void glSurfaceViewHost_Resize(int width, int height);

    public static native void swappyGLHost_Init(SwappyGLHostActivity activity,
        BaseGLHostActivity.GLContextConfiguration preferredCtxConfiguration,
        BaseGLHostActivity.GLContextConfiguration fallbackCtxConfiguration);

    public static native void swappyGLHost_SetSurface(Surface surface,
                                                      int width,
                                                      int height);

    public static native void swappyGLHost_StartRenderer();

    public static native void swappyGLHost_StopRenderer();

    public static native void swappyGLHost_ClearSurface();

    public static native void swappyGLHost_SetAutoSwapInterval(boolean enabled);

    public static native int swappyGLHost_GetSwappyStats(int stat, int bin);


}
