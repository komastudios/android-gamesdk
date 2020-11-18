// This file provides the native functions for the TryAllocTester.

#include <jni.h>
#include <stdlib.h>

extern "C" JNIEXPORT jboolean JNICALL
Java_com_google_android_apps_internal_games_memoryadvice_TryAllocTester_tryAlloc(
    JNIEnv *env, jclass clazz, jint bytes) {
  size_t byte_count = (size_t)bytes;
  char *data = (char *)malloc(byte_count);
  if (data) {
    free(data);
    return true;
  }
  return false;
}

static constexpr uint32_t kLcgPrime1 = 214013;
static constexpr uint32_t kLcgPrime2 = 2531011;

/**
 * Fill a specified area of memory with pseudo-random data and verify it was
 * correctly written.
 * @param addr The memory location to begin writing.
 * @param byte_len The number of bytes.
 * @return true if the memory was successfully written and verfied.
 */
bool LcgFill(void *addr, size_t byte_len) {
  int i = rand();
  uint32_t lcg_current = (uint32_t)i;
  uint32_t *data = (uint32_t *)addr;
  size_t word_len = byte_len / 4;
  for (size_t word_count = 0; word_count < word_len; ++word_count) {
    lcg_current *= kLcgPrime1;
    lcg_current += kLcgPrime2;
    data[word_count] = lcg_current;
  }
  // Read back written data.
  lcg_current = (uint32_t)i;
  for (size_t word_count = 0; word_count < word_len; ++word_count) {
    lcg_current *= kLcgPrime1;
    lcg_current += kLcgPrime2;
    if (data[word_count] != lcg_current) {
      return false;
    }
  }
  return true;
}

extern "C" JNIEXPORT jboolean JNICALL
Java_com_google_android_apps_internal_games_memoryadvice_TryAllocTester_occupyMemory(
    JNIEnv *env, jclass clazz, jint bytes) {
  size_t byte_count = (size_t)bytes;
  char *data = (char *)malloc(byte_count);
  if (!data) {
    return false;
  }
  bool filled = LcgFill(data, byte_count);
  if (!filled) {
    free(data);
  }
  return filled;
}