// Native entry points for Test Renderer.
#include <jni.h>

#include "test-renderer.h"

namespace {
istresser_testrenderer::TestRenderer *test_renderer;
}  // namespace

extern "C" JNIEXPORT void JNICALL
Java_net_jimblackler_istresser_MainActivity_initGl(JNIEnv *env, jclass thiz) {
  test_renderer = new istresser_testrenderer::TestRenderer();
}

extern "C" JNIEXPORT jint JNICALL
Java_net_jimblackler_istresser_MainActivity_nativeDraw(JNIEnv *env,
                                                       jclass clazz,
                                                       jint toAllocate) {
  if (test_renderer == NULL) {
    return 0;
  } else {
    return test_renderer->Render(toAllocate);
  }
}

extern "C" JNIEXPORT void JNICALL
Java_net_jimblackler_istresser_MainActivity_release(JNIEnv *env, jclass clazz) {
  if (test_renderer != NULL) {
    test_renderer->Release();
  }
}
