#pragma once

#include <list>
#include <mutex>

namespace istresser_allocator {

// An object to reserve and free memory.
class Allocator {
 private:
  std::mutex mtx;
  std::list<char *> allocated;
  std::list<size_t> allocated_size;

 public:
  Allocator() = default;
  ~Allocator() = default;
  bool Allocate(size_t to_allocate);
  void Release();
  void ReleasePartial(size_t bytes);
};

}  // namespace istresser_allocator