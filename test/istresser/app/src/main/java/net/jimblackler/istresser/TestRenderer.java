package net.jimblackler.istresser;

import android.opengl.GLES20;
import android.opengl.GLSurfaceView;
import java.util.concurrent.atomic.AtomicBoolean;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class TestRenderer implements GLSurfaceView.Renderer {

  private final AtomicBoolean intialized = new AtomicBoolean(false);

  @Override
  public void onSurfaceCreated(GL10 unused, EGLConfig config) {
    GLES20.glClearColor(0.4f, 0.9f, 0.9f, 1.0f);
    if (intialized.compareAndSet(false, true)) {
      MainActivity.initGl();
    }
  }

  public void onSurfaceChanged(GL10 unused, int width, int height) {
    GLES20.glViewport(0, 0, width, height);
  }

  public void onDrawFrame(GL10 unused) {
    MainActivity.nativeDraw();
  }

  public void release() {
    MainActivity.release();
  }
}
