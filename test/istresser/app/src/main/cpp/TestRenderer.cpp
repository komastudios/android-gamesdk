#include "TestRenderer.h"

#include "Consumer.h"
#include <EGL/egl.h>
#include <GLES3/gl32.h>

TestRenderer::TestRenderer() = default;

TestRenderer::~TestRenderer() = default;

void TestRenderer::release() {
  consumers.clear();
}

long TestRenderer::render() {
  glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
  int allocated = 0;
  for (int count = 0; count != 1 << 8U; count++) {
    int toAllocate = 1 << 14U;
    auto* consumer = new Consumer(toAllocate);
    consumers.emplace_back(consumer);
    allocated += consumer->getUsed();
  }
  return allocated;
}
