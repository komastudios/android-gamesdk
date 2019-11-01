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

package com.google.gamesdk.gamecert.operationrunner.hosts;

import android.content.Context;
import android.content.Intent;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.util.AttributeSet;
import android.util.Log;

import com.google.gamesdk.R;
import com.google.gamesdk.gamecert.operationrunner.transport.Configuration;
import com.google.gamesdk.gamecert.operationrunner.util.NativeInvoker;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class GLSurfaceViewHostActivity extends BaseHostActivity {

    public static final String ID = "GLSurfaceViewHostActivity";
    private static final String TAG = "GLSurfaceViewHostActivity";

    GLES3JNIView _view;

    public static Intent createIntent(Context ctx, Configuration.StressTest stressTest) {
        Intent i = new Intent(ctx, GLSurfaceViewHostActivity.class);
        i.putExtra(STRESS_TEST_JSON, stressTest.toJson());
        return i;
    }

    @Override
    protected int getContentViewResource() {
        return R.layout.activity_glsurfaceview;
    }

    @Override
    protected void onCreate(Bundle icicle) {
        super.onCreate(icicle);

        _view = findViewById(R.id.gles3View);
        _view.init(getNativeDataGathererId(), new TestLifecycleCallback() {
            @Override
            public void onTestStarted() {
                Log.d(TAG, "onTestStarted");
            }

            @Override
            public void onTestFinished() {
                Log.d(TAG, "onTestFinished");
            }
        });
    }

    public static class GLES3JNIView extends GLSurfaceView {

        public GLES3JNIView(Context context) {
            super(context);
        }

        public GLES3JNIView(Context context, AttributeSet attrs) {
            super(context, attrs);
        }

        void init(int testHandle, TestLifecycleCallback lifecycleCallback) {
            // Pick an EGLConfig with RGB8 color, 16-bit depth, no stencil,
            // supporting OpenGL ES 2.0 or later backwards-compatible versions.
            setEGLConfigChooser(8, 8, 8, 0, 24, 0);
            setEGLContextClientVersion(3);
            setRenderer(new Renderer(testHandle, lifecycleCallback));
        }

        public static class Renderer implements GLSurfaceView.Renderer {
            private int _nativeTestId;
            private TestLifecycleCallback _testLifecycleCallback;
            private boolean _testDone = false;

            Renderer(int nativeTestId, TestLifecycleCallback lifecycleCallback) {
                this._nativeTestId = nativeTestId;
                this._testLifecycleCallback = lifecycleCallback;
            }

            public void onDrawFrame(GL10 gl) {
                // if the test is finished, shutdown. Otherwise draw the next frame.
                if (_nativeTestId >= 0 && NativeInvoker.isOperationStopped(_nativeTestId)) {
                    _testDone = true;
                    _nativeTestId = -1;
                    _testLifecycleCallback.onTestFinished();
                    return;
                }

                NativeInvoker.glSurfaceViewHost_Draw();
            }

            public void onSurfaceChanged(GL10 gl, int width, int height) {
                NativeInvoker.glSurfaceViewHost_Resize(width, height);
            }

            public void onSurfaceCreated(GL10 gl, EGLConfig config) {
                NativeInvoker.glSurfaceViewHost_ContextReady();
                _testLifecycleCallback.onTestStarted();
            }
        }
    }

    private interface TestLifecycleCallback {
        void onTestStarted();

        void onTestFinished();
    }

}