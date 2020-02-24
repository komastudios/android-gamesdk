#include "TestRenderer.h"

#include <algorithm>

#include "Consumer.h"
#include <EGL/egl.h>
#include <GLES3/gl32.h>

static const long MAX_CONSUMER_SIZE = 1U << 14U;

TestRenderer::TestRenderer() = default;

TestRenderer::~TestRenderer() = default;

void TestRenderer::release() {
  consumers.clear();
}

long TestRenderer::render(long toAllocate) {
  glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
  int totalAllocated = 0;
  while (toAllocate > 0) {
    auto* consumer = new Consumer(std::min(MAX_CONSUMER_SIZE, toAllocate));
    consumers.emplace_back(consumer);
    int used = consumer->getUsed();
    if (used == 0) {
      break;
    }
    totalAllocated += used;
    toAllocate -= used;
  }
  return totalAllocated;
}
