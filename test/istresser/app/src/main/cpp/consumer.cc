#include "consumer.h"

#include <EGL/egl.h>
#include <GLES3/gl32.h>

#include <cmath>
#include <vector>

#include "android/log.h"
#include "gl-utils.h"
#include "memory.h"

#define ALOGE(...) \
  __android_log_print(ANDROID_LOG_ERROR, "GlUtils", __VA_ARGS__)

namespace istresser_consumer {

Consumer::Consumer(int bytes) : vertex_buffer_(0), used_(0) {
  glGenBuffers(1, &vertex_buffer_);
  if (istresser_glutils::CheckGlError("glGenBuffers")) {
    return;
  }
  if (vertex_buffer_ == 0) {
    ALOGE("Could not gen buffers.");
    return;
  }
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_);
  if (istresser_glutils::CheckGlError("glBindBuffer")) {
    return;
  }
  unsigned int num_vertices = bytes / sizeof(GLfloat);
  GLsizeiptr allocateBytes = num_vertices * sizeof(GLfloat);
  GLfloat *data = (GLfloat *)malloc(allocateBytes);

  if (!data) {
    ALOGE("Could not malloc for buffer.");
    return;
  }

  LcgFill(data, allocateBytes);

  glBufferData(GL_ARRAY_BUFFER, allocateBytes, data, GL_STATIC_DRAW);
  free(data);
  if (istresser_glutils::CheckGlError("glBufferData")) {
    ALOGE("Could not create buffers.");
    return;
  }
  used_ = allocateBytes;
}

Consumer::~Consumer() {
  if (vertex_buffer_ == 0) {
    ALOGE("Consumer was in invalid state - destructor does nothing.");
    return;
  }
  glDeleteBuffers(1, &vertex_buffer_);
  istresser_glutils::CheckGlError("glDeleteBuffers");
}

int Consumer::GetUsed() { return used_; }

}  // namespace istresser_consumer
