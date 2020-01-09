#include "Consumer.h"

#include "GlUtils.h"
#include "android/log.h"
#include <EGL/egl.h>
#include <GLES3/gl32.h>
#include <cmath>
#include <vector>

#define ALOGE(...) __android_log_print(ANDROID_LOG_ERROR, "GlUtils", __VA_ARGS__)

Consumer::Consumer(int bytes) {
  vertexBuffer = 0;
  glGenBuffers(1, &vertexBuffer);
  checkGlError("glGenBuffers");
  if (vertexBuffer == 0) {
    ALOGE("Could not gen buffers.");
    return;
  }
  glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
  checkGlError("glBindBuffer");
  unsigned int vertices = bytes / sizeof(GLfloat);
  std::vector<GLfloat> _vertices;
  _vertices.resize(vertices);
  auto size = (GLsizeiptr) _vertices.size() * sizeof(GLfloat);
  glBufferData(GL_ARRAY_BUFFER, size, &_vertices[0], GL_STATIC_DRAW);
  checkGlError("glBufferData");
}

Consumer::~Consumer() {
  if (vertexBuffer == 0) {
    ALOGE("Consumer was in invalid state - destructor does nothing.");
    return;
  }
  glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
  glDeleteBuffers(1, &vertexBuffer);
}
