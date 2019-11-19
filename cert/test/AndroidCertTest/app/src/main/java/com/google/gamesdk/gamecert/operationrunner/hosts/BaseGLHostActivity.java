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
    int redBits = 0;
    int greenBits = 0;
    int blueBits = 0;
    int alphaBits = 0;
    int depthBits = 0;
    int stencilBits = 0;
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
    GLContextConfiguration config = new GLContextConfiguration();
    config.redBits = 8;
    config.greenBits = 8;
    config.blueBits = 8;
    config.alphaBits = 0;
    config.depthBits = 24;
    config.stencilBits = 0;
    return config;
  }
}
