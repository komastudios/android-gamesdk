/*
 * Copyright 2019 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * Tests whether the device features memory region protection (mprotect) on
 * Vulkan memory buffers. mprotect is a UNIX feature that allows setting a lock
 * on memory regions (Read, Write, etc.) Afterwards, any access attempt to all
 * or part of that region that violates the access-level, produces a
 * segmentation fault (SIGSEGV). In a normal scenario, such signal would crash
 * the app if not trapped.
 * For this test, we first set a trap on that segmentation fault signal. Meaning
 * that instead of crashing, the occurrence of the signal invokes a function of
 * ours that marks the test as passed. This is because we had set mprotect on
 * Vulkan mapped memory and then tried to violate the lock just for the segment
 * fault to happen.
 * Conversely, if the segmentation fault didn't occurred in spite of having
 * mprotected a memory region, then the test would consequently fail.
 *
 * Input configuration: None. As this is a functional test (yes-no), the user
 *                      doesn't have to specify any particular input data.
 *
 * Output report: an mprotect JSON object containing two data points.
 * - available: boolean that indicates whether mprotect scope covers Vulkan
 *              allocated memory.
 * - score: an integer that tells, in case of test fail, how far it went.
 *     - 0: success. All fine (i.e., available is true).
 *     - 1: writing on write-protected memory raised a signal as expected, but
 *          the test later failed to remove the write lock to the memory region.
 *     - 2: the test raises a signal if trying to write to write protected
 *          memory, but it also does it when writing is re-enabled.
 *     - 3: the attempt to set a write lock on Vulkan memory failed.
 *     - 4: in spite of being write-locked, attempting to write to the memory
 *          region didn't raise any segmentation fault.
 *     - 5: the attempt to map Vulkan memory as addressable memory failed.
 *     - 6: the test failed to allocate Vulkan memory.
 *     - 7: Vulkan doesn't report memory that can be mapped into an addressable
 *          space.
 *     - 8: the device doesn't support Vulkan.
 */

#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-non-private-member-variables-in-classes"

#include <cassert>
#include <sys/mman.h>
#include <sys/user.h>

#include <ancer/BaseVulkanOperation.hpp>
#include <ancer/DatumReporting.hpp>
#include <ancer/util/Json.hpp>

using namespace ancer;
using namespace ancer::vulkan;

namespace {
/**
 * This mprotect availability test may fail at many stages. For that reason
 * this enum establishes a scoring to measure how far from success a device
 * is.
 * The higher the score, the farther from success. The lowest score, zero,
 * means success (i.e., mprotect is fully available).
 */
enum MprotectScore : int {
  /**
   * Vulkan memory can be successfully protected against writing
   */
      Available,

  /**
   * mprotect failed to make Vulkan mapped memory R/W
   */
      ReadWriteMprotectFailure,

  /**
   * Vulkan memory write error in spite of being R/W
   */
      WriteViolationUnexpected,

  /**
   * mprotect failed to make Vulkan mapped memory read only
   */
      ReadOnlyMprotectFailure,

  /**
   * Vulkan memory write succeeded in spite of being read-only
   */
      WriteViolationExpected,

  /**
   * Vulkan memory mapping failed
   */
      MemoryMappingFailure,

  /**
   * Vulkan memory allocation failed
   */
      MemoryAllocationFailure,

  /**
   * Device doesn't have mappable memory
   */
      NoMappableMemory,

  /**
   * Device doesn't have Vulkan support
   */
      VulkanUnavailable
};

struct mprotect_info {
  bool available{false};
  MprotectScore score{NoMappableMemory};

  mprotect_info(const MprotectScore score) {
    SetScore(score);
  }

  void SetScore(const MprotectScore new_score) {
    available = new_score==Available;
    score = new_score;
  }
};

void WriteDatum(ancer::report_writers::Struct w, const mprotect_info &i) {
  ADD_DATUM_MEMBER(w, i, available);
  ADD_DATUM_MEMBER(w, i, score);
}

struct datum {
  mprotect_info mprotect;
};

void WriteDatum(ancer::report_writers::Struct w, const datum &d) {
  ADD_DATUM_MEMBER(w, d, mprotect);
}

constexpr Log::Tag TAG{"VulkanMprotectCheckOperation"};

volatile bool protectionViolationOccurred;

/**
 * This function is meant to be called when is detected that memory that had
 * been previously protected was accessed in ways that violates the
 * protection. The purpose of this function is to verify that the protection
 * scheme succeeded. See TrapSegmentationFaultSignals().
 */
void OnProtectionViolation(int, siginfo_t *si, void *);

/**
 * Calling this function opens a time frame in which any segmentation fault
 * generated will be captured (rather than crashing the app!). In the
 * context of this operation, a segmentation fault would occur if we attempt
 * to write memory that had been previously mprotect'ed against writing. See
 * OnProtectionViolation().
 * The time frame lasts until ReleaseSegmentationFaultSignals(previous
 * scheme) is invoked.
 *
 * @param pOldSigAction pointer where the pre-existing segmentation fault
 *                      scheme is to be stored.
 */
void TrapSegmentationFaultSignals(struct sigaction *pOldSigAction);

/**
 * Releases the segmentation fault capturing scheme by restoring the
 * previous one.
 */
void ReleaseSegmentationFaultSignals(const struct sigaction *pOldSigAction);

/**
 * Not every Vulkan memory type is mappable. A memory type that isn't
 * mappable can't be mprotected (by design) and therefore must be skipped
 * from this test.
 *
 * @return true if the memory type can be mapped.
 */
bool IsMemoryTypeMappable(const VkMemoryType &memoryType);
}

class VulkanMprotectCheckOperation : public BaseVulkanOperation {
 private:
  class VulkanMemoryAllocator;    // forward declaration

 public:
  VulkanMprotectCheckOperation() : BaseVulkanOperation(::TAG),
                                   _oldSigaction{} {
    ::TrapSegmentationFaultSignals(&(this->_oldSigaction));
  }

  // RAII
  ~VulkanMprotectCheckOperation() override {
    ::ReleaseSegmentationFaultSignals(&(this->_oldSigaction));
  }

  void Start() override {
    mprotect_info mprotect{VulkanUnavailable};

    if (!IsVulkanAvailable()) {
      Report(datum{mprotect});
      return;
    }

    mprotect.SetScore(NoMappableMemory);

    for (uint32_t memTypeIndex = 0;
         memTypeIndex < _vk->physical_device_memory_properties.memoryTypeCount;
         ++memTypeIndex) {
      if (::IsMemoryTypeMappable(
          _vk->physical_device_memory_properties.memoryTypes[memTypeIndex])) {
        Log::I(TAG, "Testing memory type index %d", memTypeIndex);

        ProtectMemoryAndAttemptWrite(memTypeIndex, mprotect);
        if (!mprotect.available)
          break;
      } else
        Log::D(TAG, "Skipping memory type index %d", memTypeIndex);
    }

    Report(datum{mprotect});
  }

 private:
  void ProtectMemoryAndAttemptWrite(const uint32_t memTypeIndex,
                                    mprotect_info &mprotect) {
    VulkanMemoryAllocator allocator{
        _vk, memTypeIndex, mprotect
    };
    allocator.ProtectMemoryAndAttemptWrite(mprotect);
  }

  struct sigaction _oldSigaction;

  /**
   * This nested class encapsulates the bulk of the memory access test. It
   * allocates memory that will try to lock against writing and attempt
   * writing on it. It ensures that all allocated memory is released upon
   * destruction.
   */
  class VulkanMemoryAllocator {
   public:
    /**
     * Constructor. It allocates a portion of Vulkan memory and maps it for
     * mprotect().
     * @param device An available Vulkan device
     * @param memoryTypeIndex Index to the physical Vulkan device memory
     *                        type table.
     */
    VulkanMemoryAllocator(Vulkan vk, const uint32_t memoryTypeIndex,
                          mprotect_info &mprotect)
        : _vk{vk}, _deviceMemoryBuffer{VK_NULL_HANDLE}, _pMappedBuffer{nullptr} {
      VkMemoryAllocateInfo allocInfo = {};
      allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
      allocInfo.allocationSize = kAllocationSize;
      allocInfo.memoryTypeIndex = memoryTypeIndex;

      VkResult res = _vk->allocateMemory(_vk->device, &allocInfo, nullptr,
                                         &_deviceMemoryBuffer);
      if (res==VK_SUCCESS) {
        Log::D(TAG, "Vulkan memory allocated");

        // Map a "misaligned" 4-page subset of the 5 pages allocated.
        res = _vk->mapMemory(_vk->device, _deviceMemoryBuffer, kBufferOffset,
                             kMappedSize, 0, (void **) &_pMappedBuffer);
        if (res==VK_SUCCESS) {
          Log::D(TAG, "Vulkan memory mapped");
        } else {
          mprotect.SetScore(MemoryMappingFailure);
          Log::E(TAG, "Vulkan memory mapping failure. res=%d", res);
        }
      } else {
        mprotect.SetScore(MemoryAllocationFailure);
        Log::E(TAG, "Vulkan memory allocation failure. res=%d", res);
      }
    }

    // RAII
    virtual ~VulkanMemoryAllocator() {
      if (_pMappedBuffer) {
        _vk->unmapMemory(_vk->device, _deviceMemoryBuffer);
        Log::D(TAG, "Vulkan memory unmapped");
      }

      if (_deviceMemoryBuffer!=VK_NULL_HANDLE) {
        _vk->freeMemory(_vk->device, _deviceMemoryBuffer, nullptr);
        Log::D(TAG, "Vulkan memory released");
      }
    }

    /**
     * Tries to lock the buffer against writing and then write on it. A flag
     * should indicate that a segmentation fault occurred. If not, the test
     * just failed.
     * If the segmentation fault occurred, this function removes the lock
     * and tries writing again. This time no segmentation fault should
     * occur. If it did, the test failed.
     * Otherwise, it passed.
     *
     * @param mprotect The data structure used to hold test results.
     */
    void ProtectMemoryAndAttemptWrite(mprotect_info &mprotect) {
      if (this->MakeBufferReadOnly()) {
        protectionViolationOccurred = false;
        this->WriteBuffer();
        if (protectionViolationOccurred) {
          if (this->MakeBufferWritable()) {
            protectionViolationOccurred = false;
            this->WriteBuffer();
            if (protectionViolationOccurred) {
              mprotect.SetScore(WriteViolationUnexpected);
              Log::I(TAG, "Unexpected memory write violation caught.");
            } else {
              mprotect.SetScore(Available);
              Log::I(TAG, "mprotect can be applied to Vulkan mapped memory.");
            }
          } else {
            mprotect.SetScore(ReadWriteMprotectFailure);
            Log::I(TAG, "mprotect couldn't set Vulkan memory read/write");
          }
        } else {
          mprotect.SetScore(WriteViolationExpected);
          Log::I(TAG, "Expected memory write violation wasn't caught.");
        }
      } else {
        mprotect.SetScore(ReadOnlyMprotectFailure);
        Log::I(TAG, "mprotect couldn't set Vulkan memory read-only");
      }
    }

   private:
    static const int kAllocatedPages{5};
    static const VkDeviceSize kAllocationSize{kAllocatedPages*PAGE_SIZE};
    static const int kMappedPages{4};
    static const VkDeviceSize kMappedSize{kMappedPages*PAGE_SIZE};
    static const int kBufferOffset{2};

    Vulkan _vk;
    VkDeviceMemory _deviceMemoryBuffer;
    char *_pMappedBuffer;

    bool MakeBufferReadOnly() const {
      auto success = SetBufferProtectionLevel(PROT_READ)==0;
      if (success)
        Log::D(TAG, "Vulkan memory buffer set read-only");

      return success;
    }

    bool MakeBufferWritable() const {
      auto success = SetBufferProtectionLevel(PROT_WRITE)==0;
      if (success) {
        Log::D(TAG, "Vulkan memory buffer set read/write");
      }

      return success;
    }

    int SetBufferProtectionLevel(int protectionLevel) const {
      // mprotect requires the beginning of a page as its first parameter.
      // We also map a little more than 4 pages to make sure we can
      // attempt to write to the final page.
      if (mprotect(_pMappedBuffer - kBufferOffset, kMappedSize + 1,
                   protectionLevel)) {
        Log::I(TAG, "mprotect on Vulkan mapped memory failed. errno=%d",
               errno);
        return errno;
      }

      return 0;
    }

    void WriteBuffer() {
      char *p = _pMappedBuffer;
      *p = 'a';
    }
  };
};

namespace {
const auto kAllBits = 0xFFFULL;

void OnProtectionViolation(int, siginfo_t *si, void *) {
  protectionViolationOccurred = true;
  mprotect((void *) ((size_t) si->si_addr & ~kAllBits), 1,
           PROT_READ | PROT_WRITE); // NOLINT(hicpp-signed-bitwise)
}

void TrapSegmentationFaultSignals(struct sigaction *pOldSigAction) {
  struct sigaction sa{};

  sa.sa_flags = SA_SIGINFO;
  sigemptyset(&sa.sa_mask);
  sa.sa_sigaction = OnProtectionViolation;

  if (sigaction(SIGSEGV, &sa, pOldSigAction)) {
    Log::E(TAG, "Setting segmentation fault capturing failed. errno=%d",
           errno);
  } else {
    Log::I(TAG, "Segmentation fault capturing set");
  }
}

void ReleaseSegmentationFaultSignals(const struct sigaction *pOldSigAction) {
  if (sigaction(SIGSEGV, pOldSigAction, nullptr)) {
    Log::E(TAG, "Releasing segmentation fault capturing failed. errno=%d", errno);
  } else {
    Log::I(TAG, "Segmentation fault capturing released");
  }
}

bool IsMemoryTypeMappable(const VkMemoryType &memoryType) {
  return VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT==
      (memoryType.propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
}
}

EXPORT_ANCER_OPERATION(VulkanMprotectCheckOperation)

#pragma clang diagnostic pop
