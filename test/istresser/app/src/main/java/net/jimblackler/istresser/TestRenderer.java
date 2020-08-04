package net.jimblackler.istresser;

import static javax.microedition.khronos.opengles.GL10.GL_MAX_ELEMENTS_VERTICES;

import android.opengl.GLES20;
import android.opengl.GLSurfaceView;
import android.util.Log;
import java.nio.IntBuffer;
import java.util.concurrent.atomic.AtomicBoolean;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class TestRenderer implements GLSurfaceView.Renderer {
  private static final String TAG = TestRenderer.class.getSimpleName();

  private static final int VIEWPORT_X = 0;
  private static final int VIEWPORT_Y = 0;
  private static final long GL_FLOAT_SIZE = 4L;
  private final AtomicBoolean intialized = new AtomicBoolean(false);
  private long target;
  private long allocated;
  private long maxConsumerSize;
  private boolean failed;
  private int maxElementVertices;

  @Override
  public void onSurfaceCreated(GL10 gl, EGLConfig config) {
    if (intialized.compareAndSet(false, true)) {
      MainActivity.initGl();
      try {
        IntBuffer valueOut = IntBuffer.allocate(1);
        gl.glGetIntegerv(GL_MAX_ELEMENTS_VERTICES, valueOut);
        maxElementVertices = valueOut.get(0);
      } catch (android.opengl.GLException e) {
        maxElementVertices = Integer.MAX_VALUE;
        Log.w(TAG, "Could not get max buffer size", e);
      }
    }
  }

  public void onSurfaceChanged(GL10 unused, int width, int height) {
    GLES20.glViewport(VIEWPORT_X, VIEWPORT_Y, width, height);
  }

  public void onDrawFrame(GL10 unused) {
    long allocate1 = Math.min(target - allocated, maxConsumerSize);
    allocate1 = Math.min(allocate1, maxElementVertices * GL_FLOAT_SIZE);
    int toAllocate = (int) Math.min(Integer.MAX_VALUE, allocate1);

    if (toAllocate > 0) {
      int result = MainActivity.nativeDraw(toAllocate);
      if (result >= toAllocate) {
        allocated += result;
      } else {
        Log.i(TAG, "Allocation failed");
        failed = true;
      }
    }
  }

  public void release() {
    target = 0;
    allocated = 0;
    failed = false;
    MainActivity.release();
  }

  public long getAllocated() {
    return allocated;
  }

  void setMaxConsumerSize(long value) {
    maxConsumerSize = value;
  }

  public void setTarget(long value) {
    target = value;
  }

  boolean getFailed() {
    return failed;
  }
}
