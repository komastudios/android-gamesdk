#pragma once
#include <GLES3/gl32.h>

namespace istresser_consumer {

// An object to reserve GL memory with glBufferData on construction,
// and free it on destruction.
class Consumer {
 public:
  Consumer(int bytes);
  ~Consumer();

  int GetUsed();

 private:
  GLuint vertex_buffer_{};
  int used_;
};

}  // namespace istresser_consumer