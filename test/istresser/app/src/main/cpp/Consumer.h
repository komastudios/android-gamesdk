#include <GLES3/gl32.h>

class Consumer {
 public:
  Consumer(int bytes);
  ~Consumer();

 private:
  GLuint vertexBuffer;
};
