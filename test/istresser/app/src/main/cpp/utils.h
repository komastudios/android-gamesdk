#include <memory>
#include <mutex>

#include "consumer.h"

namespace istresser_utils {

// A class to contain necessary long term variables for native-lib.cc
class Utils {
 public:
  std::list<char *> allocated;
  std::list<size_t> allocated_size;
  std::list<void *> mmap_allocated;
  std::list<size_t> mmap_allocated_size;
  std::mutex mtx;

  Utils() = default;

  ~Utils() = default;
};

}  // namespace istresser_utils
