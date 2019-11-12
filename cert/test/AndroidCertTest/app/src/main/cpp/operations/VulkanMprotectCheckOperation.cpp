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

#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-non-private-member-variables-in-classes"

#include <ancer/BaseVulkanOperation.hpp>
#include <ancer/util/Json.hpp>

#include <cassert>
#include <sys/mman.h>
#include <sys/user.h>

using namespace ancer;


namespace {
    /**
     * This mprotect availability test may fail at many stages. For that reason
     * this enum establishes a ranking to measure how far from success a device
     * is.
     * The higher the score, the farther from success. The lowest score, zero,
     * means success (i.e., mprotect is fully available).
     */
    enum MprotectRank : int {
      /**
       * Vulkan memory can be successfully protected against writing
       */
          AVAILABLE,
      /**
       * mprotect failed to make Vulkan mapped memory R/W
       */
          READ_WRITE_MPROTECT_FAILURE,
      /**
       * Vulkan memory write error in spite of being R/W
       */
          WRITE_VIOLATION_UNEXPECTED,
      /**
       * mprotect failed to make Vulkan mapped memory read only
       */
          READ_ONLY_MPROTECT_FAILURE,
      /**
       * Vulkan memory write succeeded in spite of being read-only
       */
          WRITE_VIOLATION_EXPECTED,
      /**
       * Vulkan memory mapping failed
       */
          MEMORY_MAPPING_FAILURE,
      /**
       * Vulkan memory allocation failed
       */
          MEMORY_ALLOCATION_FAILURE,
      /**
       * Device doesn't have mappable memory
       */
          NO_MAPPABLE_MEMORY
    };

    struct mprotect_info {
        bool available{false};
        MprotectRank rank{NO_MAPPABLE_MEMORY};

        mprotect_info(const MprotectRank rank) {
            this->SetRank(rank);
        }

        void SetRank(const MprotectRank rank) {
            this->available = rank==AVAILABLE;
            this->rank = rank;
        }
    };

    struct datum {
        mprotect_info mprotect;
    };

    JSON_WRITER(mprotect_info) {
        JSON_REQVAR(available);
        JSON_REQVAR(rank);
    }

    JSON_WRITER(datum) {
        JSON_REQVAR(mprotect);
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
        mprotect_info mprotect{NO_MAPPABLE_MEMORY};    // until proved otherwise

        auto &info{this->GetInfo()};
        for (uint32_t memTypeIndex = 0;
             memTypeIndex < info.memoryProperties.memoryTypeCount;
             ++memTypeIndex) {
            if (::IsMemoryTypeMappable(
                    info.memoryProperties.memoryTypes[memTypeIndex])) {
                Log::I(TAG, "Testing memory type index %d", memTypeIndex);

                ProtectMemoryAndAttemptWrite(memTypeIndex, mprotect);
                if (!mprotect.available) {
                    break;
                }
            } else {
                Log::D(TAG, "Skipping memory type index %d", memTypeIndex);
            }
        }

        Report(datum{mprotect});
    }

private:
    void ProtectMemoryAndAttemptWrite(const uint32_t memTypeIndex,
                                      mprotect_info &mprotect) {
        VulkanMemoryAllocator allocator{
              this->GetDevice(), memTypeIndex, mprotect
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
        VulkanMemoryAllocator(
              VkDevice device,
              const uint32_t memoryTypeIndex,
              mprotect_info &mprotect
        ) : _device{device}, _deviceMemoryBuffer{VK_NULL_HANDLE},
            _pMappedBuffer{nullptr} {
            VkMemoryAllocateInfo allocInfo = {};
            allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
            allocInfo.allocationSize = ALLOCATION_SIZE;
            allocInfo.memoryTypeIndex = memoryTypeIndex;

            VkResult res = vkAllocateMemory(device, &allocInfo, nullptr,
                                            &_deviceMemoryBuffer);
            if (res == VK_SUCCESS) {
                Log::D(TAG, "Vulkan memory allocated");

                // Map a "misaligned" 4-page subset of the 5 pages allocated.
                res = vkMapMemory(
                        device, _deviceMemoryBuffer, BUFFER_OFFSET, MAPPED_SIZE,
                        0,
                        (void **) &_pMappedBuffer
                );
                if (res == VK_SUCCESS) {
                    Log::D(TAG, "Vulkan memory mapped");
                } else {
                    mprotect.SetRank(MEMORY_MAPPING_FAILURE);
                    Log::E(TAG, "Vulkan memory mapping failure. res=%d", res);
                }
            } else {
                mprotect.SetRank(MEMORY_ALLOCATION_FAILURE);
                Log::E(TAG, "Vulkan memory allocation failure. res=%d", res);
            }
        }

        // RAII
        virtual ~VulkanMemoryAllocator() {
            if (_pMappedBuffer) {
                vkUnmapMemory(_device, _deviceMemoryBuffer);
                Log::D(TAG, "Vulkan memory unmapped");
            }
            if (_deviceMemoryBuffer != VK_NULL_HANDLE) {
                vkFreeMemory(_device, _deviceMemoryBuffer, nullptr);
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
                            mprotect.SetRank(WRITE_VIOLATION_UNEXPECTED);
                            Log::I(TAG,
                                   "Unexpected memory write violation caught.");
                        } else {
                            mprotect.SetRank(AVAILABLE);
                            Log::I(TAG,
                                   "mprotect can be applied to Vulkan mapped "
                                   "memory.");
                        }
                    } else {
                        mprotect.SetRank(READ_WRITE_MPROTECT_FAILURE);
                        Log::I(TAG, "mprotect couldn't set Vulkan memory "
                                    "read/write");
                    }
                } else {
                    mprotect.SetRank(WRITE_VIOLATION_EXPECTED);
                    Log::I(TAG,
                           "Expected memory write violation wasn't caught.");
                }
            } else {
                mprotect.SetRank(READ_ONLY_MPROTECT_FAILURE);
                Log::I(TAG, "mprotect couldn't set Vulkan memory read-only");
            }
        }

    private:
        static const int ALLOCATED_PAGES{5};
        static const VkDeviceSize ALLOCATION_SIZE{ALLOCATED_PAGES * PAGE_SIZE};
        static const int MAPPED_PAGES{4};
        static const VkDeviceSize MAPPED_SIZE{MAPPED_PAGES * PAGE_SIZE};
        static const int BUFFER_OFFSET{2};

        VkDevice _device;
        VkDeviceMemory _deviceMemoryBuffer;
        char *_pMappedBuffer;

        bool MakeBufferReadOnly() const {
            auto success = SetBufferProtectionLevel(PROT_READ) == 0;
            if (success) {
                Log::D(TAG, "Vulkan memory buffer set read-only");
            }

            return success;
        }

        bool MakeBufferWritable() const {
            auto success = SetBufferProtectionLevel(PROT_WRITE) == 0;
            if (success) {
                Log::D(TAG, "Vulkan memory buffer set read/write");
            }

            return success;
        }

        int SetBufferProtectionLevel(int protectionLevel) const {
            // mprotect requires the beginning of a page as its first parameter.
            // We also map a little more than 4 pages to make sure we can
            // attempt to write to the final page.
            if (mprotect(_pMappedBuffer - BUFFER_OFFSET, MAPPED_SIZE + 1,
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
    const auto ALL_BITS = 0xFFFULL;

    void OnProtectionViolation(int, siginfo_t *si, void *) {
        protectionViolationOccurred = true;
        mprotect((void *) ((size_t) si->si_addr & ~ALL_BITS), 1,
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
        return VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT ==
               (memoryType.propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
    }
}

EXPORT_ANCER_OPERATION(VulkanMprotectCheckOperation)

#pragma clang diagnostic pop
