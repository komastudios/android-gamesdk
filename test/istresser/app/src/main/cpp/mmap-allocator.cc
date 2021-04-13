#include "mmap-allocator.h"

#include <android/log.h>
#include <fcntl.h>  // For open
#include <limits.h>
#include <sys/mman.h>  // For mmap
#include <sys/stat.h>  // For fstat
#include <unistd.h>

#include "memory.h"

namespace istresser_mmap_allocator {

constexpr char kAppName[] = "istresser";

// Read out mmapped memory, so there's no chance some runtime optimization will
// cause the underlying file not to be read.
// TODO(b/151615602): confirm in the object file this read actually happens.
static void ReadAndIgnore(void *addr, size_t byte_len) {
  volatile char val = 0;
  char *ptr = (char *)addr;
  for (size_t i = 0; i < byte_len; ++i) {
    val = ptr[i];
  }
}

void MmapAllocator::AnonFreeAll() {
  mtx.lock();
  __android_log_print(ANDROID_LOG_INFO, kAppName, "Freeing anon mmap memory");
  while (!mmap_allocated.empty()) {
    if (munmap(mmap_allocated.front(), mmap_allocated_size.front()) != 0) {
      __android_log_print(ANDROID_LOG_INFO, kAppName,
                          ": Could not release mmapped memory");
    }
    mmap_allocated.pop_front();
    mmap_allocated_size.pop_front();
  }
  mtx.unlock();
}

size_t MmapAllocator::Consume(size_t byte_count) {
  // Actual mmapped length will always be a multiple of the page size, so we
  // round up to that in order to account properly.
  int32_t page_size_align = sysconf(_SC_PAGE_SIZE) - 1;
  byte_count = (byte_count + page_size_align) & ~page_size_align;

  void *addr = mmap(nullptr, byte_count, PROT_READ | PROT_WRITE,
                    MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  if (addr == MAP_FAILED) {
    __android_log_print(ANDROID_LOG_WARN, kAppName, "Could not mmap");
  } else {
    __android_log_print(ANDROID_LOG_INFO, kAppName, "mmapped %u bytes at %p",
                        byte_count, addr);
    LcgFill(addr, byte_count);
    mmap_allocated.push_back(addr);
    mmap_allocated_size.push_back(byte_count);
  }
  return addr != MAP_FAILED ? byte_count : 0;
}

size_t MmapAllocator::MmapFileConsume(const char *path, size_t bytes,
                                      long offset) {
  auto addr = MAP_FAILED;
  size_t byte_count = (size_t)bytes;
  size_t byte_offset = (size_t)offset;

  // Offset must be a multiple of the page size, so we round down.
  int32_t page_size_align = sysconf(_SC_PAGE_SIZE) - 1;
  byte_offset &= ~page_size_align;

  int file = open(path, O_RDONLY);
  __android_log_print(ANDROID_LOG_INFO, kAppName, "open fd is %d", file);

  if (file != -1) {
    struct stat sb;
    if (fstat(file, &sb) == -1) {
      __android_log_print(ANDROID_LOG_WARN, kAppName, "mmapFile: fstat failed");
      close(file);
      return 0;
    }

    if (byte_offset >= sb.st_size) {
      __android_log_print(ANDROID_LOG_WARN, kAppName,
                          "mmapFile: map begins beyond EOF");
      return 0;
    } else if (byte_count + byte_offset > sb.st_size) {
      byte_offset = sb.st_size - byte_count;
    }

    addr = mmap(nullptr, byte_count, PROT_READ, MAP_PRIVATE, file, byte_offset);
    if (addr == MAP_FAILED) {
      __android_log_print(ANDROID_LOG_WARN, kAppName,
                          "Could not mmap physical file");
    } else {
      __android_log_print(
          ANDROID_LOG_INFO, kAppName,
          "mmapped %zu bytes at %p from physical file %s (offset %zu)",
          byte_count, addr, path, byte_offset);
      ReadAndIgnore(addr, byte_count);
    }
    close(file);
  }
  return addr == MAP_FAILED ? 0 : byte_count;
}

}  // namespace istresser_mmap_allocator
