#include "test-renderer.h"

#include <EGL/egl.h>
#include <GLES3/gl32.h>

#include "consumer.h"

namespace istresser_testrenderer {

void TestRenderer::Release() { consumers_.clear(); }

int TestRenderer::Render(int to_allocate) {
  istresser_consumer::Consumer *consumer =
      new istresser_consumer::Consumer(to_allocate);
  int used = consumer->GetUsed();
  if (used == 0) {
    delete consumer;
  } else {
    consumers_.emplace_back(consumer);
  }
  return used;
}

}  // namespace istresser_testrenderer
