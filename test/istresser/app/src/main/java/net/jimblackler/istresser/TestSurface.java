package net.jimblackler.istresser;

import android.content.Context;
import android.opengl.GLSurfaceView;
import android.util.AttributeSet;

public class TestSurface extends GLSurfaceView {

  private final TestRenderer renderer;

  public TestSurface(Context context, AttributeSet attrs) {
    super(context, attrs);

    setEGLContextClientVersion(2);
    setEGLConfigChooser(8, 8, 8, 0, 16, 0);
    setDebugFlags(DEBUG_CHECK_GL_ERROR | DEBUG_LOG_GL_CALLS);
    renderer = new TestRenderer();
    setRenderer(renderer);
  }

  TestRenderer getRenderer() {
    return renderer;
  }
}
