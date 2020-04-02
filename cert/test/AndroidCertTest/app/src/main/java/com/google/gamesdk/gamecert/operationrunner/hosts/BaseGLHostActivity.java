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

import com.google.gamesdk.gamecert.operationrunner.util.NativeInvoker;

public abstract class BaseGLHostActivity extends BaseHostActivity {

    public static class GLContextConfiguration {
        int redBits;
        int greenBits;
        int blueBits;
        int alphaBits;
        int depthBits;
        int stencilBits;

        public GLContextConfiguration() {
            this(0, 0, 0, 0, 0, 0);
        }

        public GLContextConfiguration(int r, int g, int b, int a, int d, int s) {
            redBits = r;
            greenBits = g;
            blueBits = b;
            alphaBits = a;
            depthBits = d;
            stencilBits = s;
        }

        public String toString() {
            return String.format("{r=%d, g=%d, b=%d, a=%d, d=%d, s=%d}",
                    redBits, greenBits, blueBits, alphaBits, depthBits, stencilBits);
        }
    }

    /**
     * @return the preferred GLContextConfiguration for the data gathering operation being run
     */
    GLContextConfiguration getGLContextConfiguration() {
        GLContextConfiguration config = new GLContextConfiguration();
        NativeInvoker.getGLContextConfiguration(config);
        return config;
    }

    GLContextConfiguration getDefaultGLContextConfiguration() {
        return new GLContextConfiguration(8, 8, 8, 0, 24, 0);
    }
}
