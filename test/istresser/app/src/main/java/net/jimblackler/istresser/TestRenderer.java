package net.jimblackler.istresser;

import android.opengl.GLES20;
import android.opengl.GLSurfaceView;
import java.util.concurrent.atomic.AtomicBoolean;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class TestRenderer implements GLSurfaceView.Renderer {

  private static final int VIEWPORT_X = 0;
  private static final int VIEWPORT_Y = 0;
  private final AtomicBoolean intialized = new AtomicBoolean(false);
  private static long allocated;

  @Override
  public void onSurfaceCreated(GL10 unused, EGLConfig config) {
    if (intialized.compareAndSet(false, true)) {
      MainActivity.initGl();
    }
  }

  public void onSurfaceChanged(GL10 unused, int width, int height) {
    GLES20.glViewport(VIEWPORT_X, VIEWPORT_Y, width, height);
  }

  public void onDrawFrame(GL10 unused) {
    allocated += MainActivity.nativeDraw();
  }

  public void release() {
    allocated = 0;
    MainActivity.release();
  }

  public long getAllocated() {
    return allocated;
  }
}
