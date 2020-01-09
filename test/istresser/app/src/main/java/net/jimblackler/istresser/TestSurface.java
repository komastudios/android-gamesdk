package net.jimblackler.istresser;

import android.content.Context;
import android.opengl.GLSurfaceView;
import android.util.AttributeSet;

public class TestSurface extends GLSurfaceView {

  public static final int GL_CLIENT_VERSION = 2;
  public static final int RED_SIZE = 8;
  public static final int GREEN_SIZE = 8;
  public static final int BLUE_SIZE = 8;
  public static final int ALPHA_SIZE = 0;
  public static final int DEPTH_SIZE = 16;
  public static final int STENCIL_SIZE = 0;

  private final TestRenderer renderer;

  public TestSurface(Context context, AttributeSet attrs) {
    super(context, attrs);

    setEGLContextClientVersion(GL_CLIENT_VERSION);
    setEGLConfigChooser(RED_SIZE, GREEN_SIZE, BLUE_SIZE, ALPHA_SIZE, DEPTH_SIZE, STENCIL_SIZE);
    setDebugFlags(DEBUG_CHECK_GL_ERROR | DEBUG_LOG_GL_CALLS);
    renderer = new TestRenderer();
    setRenderer(renderer);
  }

  TestRenderer getRenderer() {
    return renderer;
  }
}
