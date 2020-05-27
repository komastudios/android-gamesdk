#include <list>
#include <memory>
#include "consumer.h"

namespace istresser_testrenderer {
// A manager object that uses Consumers to hold GLES memory for the stress test.
  class TestRenderer {
  private:
    std::list<std::unique_ptr<istresser_consumer::Consumer>> consumers_;

  public:
    TestRenderer() = default;

    ~TestRenderer() = default;

    int64_t Render(int64_t to_allocate);

    void Release();
  };
}  // namespace istresser_testrenderer
