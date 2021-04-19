#pragma once

#include <list>
#include <mutex>

namespace istresser_mmap_allocator {

// An object to reserve and free mapped memory.
class MmapAllocator {
 private:
  std::mutex mtx;
  std::list<void *> mmap_allocated;
  std::list<size_t> mmap_allocated_size;

 public:
  MmapAllocator() = default;
  ~MmapAllocator() = default;

  void AnonFreeAll();
  size_t Consume(size_t bytes);
  size_t MmapFileConsume(const char *path, size_t bytes, long offset);
};

}  // namespace istresser_mmap_allocator