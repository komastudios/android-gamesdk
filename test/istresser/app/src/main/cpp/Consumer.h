#include <GLES3/gl32.h>

class Consumer {
 public:
  Consumer(int bytes);
  ~Consumer();

  int getUsed();
 private:
  GLuint vertexBuffer;
  int used;
};
