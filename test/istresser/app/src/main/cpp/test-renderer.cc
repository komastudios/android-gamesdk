#include "test-renderer.h"

#include <algorithm>

#include "consumer.h"
#include <EGL/egl.h>
#include <GLES3/gl32.h>

static const long MAX_CONSUMER_SIZE = 1U << 14U;

namespace istresser_testrenderer {

  static const int64_t kMaxConsumerSize = 1 << 14U;
  void TestRenderer::Release() { consumers_.clear(); }

  int64_t TestRenderer::Render(int64_t to_allocate) {
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    int total_allocated = 0;
    while (to_allocate > 0) {
      istresser_consumer::Consumer *consumer = new istresser_consumer::Consumer(
          (int)std::min(kMaxConsumerSize, to_allocate));
      consumers_.emplace_back(consumer);
      int used = consumer->GetUsed();
      if (used == 0) {
        break;
      }
      total_allocated += used;
      to_allocate -= used;
    }
    return total_allocated;
  }

}  // namespace istresser_testrenderer
