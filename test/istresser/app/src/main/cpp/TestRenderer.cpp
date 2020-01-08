#include "TestRenderer.h"

#include "Consumer.h"
#include <EGL/egl.h>
#include <GLES3/gl32.h>

TestRenderer::TestRenderer() = default;

TestRenderer::~TestRenderer() = default;

void TestRenderer::release() {
  consumers.clear();
}

void TestRenderer::render() {
  glClearColor(0.75F, 0.05F, 0.05F, 1.0F);
  glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
  for (int count = 0; count != 1 << 8U; count++) {
    auto* consumer = new Consumer(1 << 14U);
    consumers.emplace_back(consumer);
  }
}
