#include "allocator.h"

#include <android/log.h>

#include "memory.h"

namespace istresser_allocator {

constexpr char kAppName[] = "memorytest";

bool Allocator::Allocate(size_t bytes) {
  mtx.lock();
  bool result = true;
  while (result && bytes > 0) {
    size_t byte_count = bytes > SIZE_T_MAX ? SIZE_T_MAX : bytes;
    char *data = (char *)malloc(byte_count);

    if (data) {
      allocated.push_back(data);
      allocated_size.push_back(byte_count);
      LcgFill(data, byte_count);
    } else {
      result = false;
      __android_log_print(ANDROID_LOG_WARN, kAppName, "Could not allocate");
    }
    bytes -= byte_count;
  }

  mtx.unlock();
  return result;
}

void Allocator::Release() {
  mtx.lock();
  __android_log_print(ANDROID_LOG_INFO, kAppName, "Freeing entries");
  while (!allocated.empty()) {
    free(allocated.front());
    allocated.pop_front();
    allocated_size.pop_front();
  }
  mtx.unlock();
}

void Allocator::ReleasePartial(size_t bytes) {
  mtx.lock();
  __android_log_print(ANDROID_LOG_INFO, kAppName, "Freeing entries");
  size_t bytes_freed = 0;
  while (!allocated.empty() && bytes_freed < bytes) {
    free(allocated.front());
    allocated.pop_front();
    bytes_freed += allocated_size.front();
    allocated_size.pop_front();
  }
  mtx.unlock();
}

}  // namespace istresser_allocator
