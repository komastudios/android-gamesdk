#include <list>
#include <memory>

class Consumer;

class TestRenderer {
 private:
  std::list<std::unique_ptr<Consumer>> consumers;

public:
  TestRenderer();
  virtual ~TestRenderer();

  long render();
  void release();
};
