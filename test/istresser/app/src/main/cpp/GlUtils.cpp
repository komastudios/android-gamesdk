#include "GlUtils.h"

#include <GLES3/gl32.h>
#include <android/log.h>
#include <malloc.h>

#define ALOGE(...) __android_log_print(ANDROID_LOG_ERROR, "GlUtils", __VA_ARGS__)

bool checkGlError(const char* funcName) {
  GLint err = glGetError();
  if (err != GL_NO_ERROR) {
    ALOGE("GL error after %s(): 0x%08x\n", funcName, err);
    return true;
  }
  return false;
}
