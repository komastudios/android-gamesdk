#include "consumer.h"

#include "gl-utils.h"
#include "android/log.h"
#include <EGL/egl.h>
#include <GLES3/gl32.h>
#include <cmath>
#include <vector>

#define ALOGE(...) __android_log_print(ANDROID_LOG_ERROR, "GlUtils", __VA_ARGS__)

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
    std::vector<GLfloat> vertices;
    vertices.resize(num_vertices);
    GLsizeiptr size = (GLsizeiptr)vertices.size() * sizeof(GLfloat);
    glBufferData(GL_ARRAY_BUFFER, size, &vertices[0], GL_STATIC_DRAW);
    if (istresser_glutils::CheckGlError("glBufferData")) {
      ALOGE("Could not create buffers.");
      return;
    }
    used_ = bytes;
  }

  Consumer::~Consumer() {
    if (vertex_buffer_ == 0) {
      ALOGE("Consumer was in invalid state - destructor does nothing.");
      return;
    }
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_);
    glDeleteBuffers(1, &vertex_buffer_);
  }

  int Consumer::GetUsed() { return used_; }

}  // namespace istresser_consumer
