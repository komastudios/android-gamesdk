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
import android.opengl.EGL14;
import android.opengl.EGLExt;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.util.AttributeSet;

import android.util.Log;
import com.google.gamesdk.R;
import com.google.gamesdk.gamecert.operationrunner.transport.Configuration;
import com.google.gamesdk.gamecert.operationrunner.util.NativeInvoker;

import javax.microedition.khronos.egl.EGL10;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.egl.EGLDisplay;
import javax.microedition.khronos.opengles.GL10;

public class GLSurfaceViewHostActivity extends BaseGLHostActivity {

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

        GLContextConfiguration configuration = getGLContextConfiguration();
        GLContextConfiguration defaultConfiguration = getDefaultGLContextConfiguration();

        _view = findViewById(R.id.gles3View);

        // TODO(shamyl@google.com): Send the success or failure of using preferred
        //  context to the operation what requested it
        _view.init(getNativeDataGathererId(),
            configuration,
            defaultConfiguration,
            null);
    }

    public static class GLES3JNIView extends GLSurfaceView {

        public GLES3JNIView(Context context) {
            super(context);
        }

        public GLES3JNIView(Context context, AttributeSet attrs) {
            super(context, attrs);
        }

        EGLConfigChooser _chooser;

        void init(int testHandle, GLContextConfiguration preferredConfiguration,
            GLContextConfiguration defaultConfiguration, TestLifecycleCallback lifecycleCallback) {

            _chooser = new EGLConfigChooser(3,
                preferredConfiguration, defaultConfiguration);

            setEGLConfigChooser(_chooser);
            setEGLContextClientVersion(3);
            setRenderer(new Renderer(testHandle, _chooser, lifecycleCallback));
        }

        public static class Renderer implements GLSurfaceView.Renderer {
            int _nativeTestId;
            TestLifecycleCallback _testLifecycleCallback;
            EGLConfigChooser _chooser;

            Renderer(int nativeTestId, EGLConfigChooser chooser, TestLifecycleCallback lifecycleCallback) {
                this._nativeTestId = nativeTestId;
                this._chooser = chooser;
                this._testLifecycleCallback = lifecycleCallback;
            }

            public void onDrawFrame(GL10 gl) {
                // if the test is finished, shutdown. Otherwise draw the next frame.
                if (_nativeTestId >= 0 && NativeInvoker.isOperationStopped(_nativeTestId)) {
                    _nativeTestId = -1;
                    if (_testLifecycleCallback != null){
                        _testLifecycleCallback.onTestFinished();
                    }
                    return;
                }

                NativeInvoker.glSurfaceViewHost_Draw();
            }

            public void onSurfaceChanged(GL10 gl, int width, int height) {
                NativeInvoker.glSurfaceViewHost_Resize(width, height);
            }

            public void onSurfaceCreated(GL10 gl, EGLConfig config) {
                NativeInvoker.glSurfaceViewHost_ContextReady(_chooser.getConfigurationUsed());
                if (_testLifecycleCallback != null) {
                    _testLifecycleCallback.onTestStarted();
                }
            }
        }

        /**
         * This is an adaptation of GLSurfaceView.BaseConfigChooser which allows
         * selection of a gl configuration from preferred and fallback configurations.
         * The built-in one simply throws an un-catchable exception, which is no use for us.
         */
        static class EGLConfigChooser implements GLSurfaceView.EGLConfigChooser {

            int _contextClientVersion;
            GLContextConfiguration _prefConfig;
            GLContextConfiguration _fallbackConfig;
            GLContextConfiguration _usingConfiguration;

            EGLConfigChooser(int contextClientVersion,
                GLContextConfiguration prefConfig, GLContextConfiguration fallbackConfig) {

                _contextClientVersion = contextClientVersion;
                _prefConfig = prefConfig;
                _fallbackConfig = fallbackConfig;
                _usingConfiguration = null;
            }

            public GLContextConfiguration getConfigurationUsed() { return _usingConfiguration; }

            @Override
            public EGLConfig chooseConfig(EGL10 egl, EGLDisplay display) {
                try {

                    // attempt to set up context using the preferred configuration
                    EGLConfig config = chooseConfig(egl, display, _prefConfig);
                    _usingConfiguration = _prefConfig;
                    return config;

                } catch (IllegalArgumentException e) {
                    try {
                        Log.i(TAG, "Unable to create requested EGL config, will attempt fallback");

                        // attempt to set up context using fallback configuration
                        EGLConfig config = chooseConfig(egl, display, _fallbackConfig);
                        _usingConfiguration = _fallbackConfig;
                        return config;

                    } catch (IllegalArgumentException e2) {
                        Log.e(TAG, "Unable to create fallback EGL config: e: " + e2.getLocalizedMessage());
                        e.printStackTrace();
                    }
                }

                return null;
            }

            EGLConfig chooseConfig(EGL10 egl, EGLDisplay display, GLContextConfiguration requestedConfig)
                throws IllegalArgumentException{
                int[] configSpec = new int[] {
                    EGL10.EGL_RED_SIZE, requestedConfig.redBits,
                    EGL10.EGL_GREEN_SIZE, requestedConfig.greenBits,
                    EGL10.EGL_BLUE_SIZE, requestedConfig.blueBits,
                    EGL10.EGL_ALPHA_SIZE, requestedConfig.alphaBits,
                    EGL10.EGL_DEPTH_SIZE, requestedConfig.depthBits,
                    EGL10.EGL_STENCIL_SIZE, requestedConfig.stencilBits,
                    EGL10.EGL_NONE };

                int[] filteredConfigSpec = filterConfigSpec(configSpec, _contextClientVersion);

                int[] numConfig = new int[1];
                if (!egl.eglChooseConfig(display, filteredConfigSpec, null, 0,
                    numConfig)) {
                    throw new IllegalArgumentException("eglChooseConfig failed");
                }

                int numConfigs = numConfig[0];

                if (numConfigs <= 0) {
                    throw new IllegalArgumentException(
                        "No configs match configSpec");
                }

                EGLConfig[] configs = new EGLConfig[numConfigs];
                if (!egl.eglChooseConfig(display, filteredConfigSpec, configs, numConfigs,
                    numConfig)) {
                    throw new IllegalArgumentException("eglChooseConfig#2 failed");
                }
                EGLConfig config = chooseConfig(egl, display, configs, requestedConfig);
                if (config == null) {
                    throw new IllegalArgumentException("No config chosen");
                }
                return config;
            }

            static EGLConfig chooseConfig(EGL10 egl, EGLDisplay display, EGLConfig[] configs,
                GLContextConfiguration requestedConfig) {
                for (EGLConfig config : configs) {
                    int d = findConfigAttrib(egl, display, config,
                        EGL10.EGL_DEPTH_SIZE, 0);
                    int s = findConfigAttrib(egl, display, config,
                        EGL10.EGL_STENCIL_SIZE, 0);
                    if ((d >= requestedConfig.depthBits) && (s >= requestedConfig.stencilBits)) {
                        int r = findConfigAttrib(egl, display, config,
                            EGL10.EGL_RED_SIZE, 0);
                        int g = findConfigAttrib(egl, display, config,
                            EGL10.EGL_GREEN_SIZE, 0);
                        int b = findConfigAttrib(egl, display, config,
                            EGL10.EGL_BLUE_SIZE, 0);
                        int a = findConfigAttrib(egl, display, config,
                            EGL10.EGL_ALPHA_SIZE, 0);
                        if ((r == requestedConfig.redBits) && (g == requestedConfig.greenBits)
                            && (b == requestedConfig.blueBits) && (a == requestedConfig.alphaBits)) {
                            return config;
                        }
                    }
                }
                return null;
            }

            static int[] filterConfigSpec(int[] configSpec, int contextClientVersion) {
                if (contextClientVersion != 2 && contextClientVersion != 3) {
                    return configSpec;
                }
                /* We know none of the subclasses define EGL_RENDERABLE_TYPE.
                 * And we know the configSpec is well formed.
                 */
                int len = configSpec.length;
                int[] newConfigSpec = new int[len + 2];
                System.arraycopy(configSpec, 0, newConfigSpec, 0, len-1);
                newConfigSpec[len-1] = EGL10.EGL_RENDERABLE_TYPE;
                if (contextClientVersion == 2) {
                    newConfigSpec[len] = EGL14.EGL_OPENGL_ES2_BIT;  /* EGL_OPENGL_ES2_BIT */
                } else {
                    newConfigSpec[len] = EGLExt.EGL_OPENGL_ES3_BIT_KHR; /* EGL_OPENGL_ES3_BIT_KHR */
                }
                newConfigSpec[len+1] = EGL10.EGL_NONE;
                return newConfigSpec;
            }

            static int findConfigAttrib(EGL10 egl, EGLDisplay display,
                EGLConfig config, int attribute, int defaultValue) {

                int[] _value = new int[1];
                if (egl.eglGetConfigAttrib(display, config, attribute, _value)) {
                    return _value[0];
                }

                return defaultValue;
            }
        }
    }

    private interface TestLifecycleCallback {
        void onTestStarted();

        void onTestFinished();
    }

}