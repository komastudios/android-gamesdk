package com.google.gamesdk.gamecert.operationrunner.hosts;

import android.os.Bundle;
import androidx.annotation.Nullable;
import com.google.gamesdk.gamecert.operationrunner.util.NativeInvoker;
import javax.microedition.khronos.egl.EGL10;
import javax.microedition.khronos.egl.EGLConfig;

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
