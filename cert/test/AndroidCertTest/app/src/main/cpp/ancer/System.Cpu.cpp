/*
 * Copyright 2020 The Android Open Source Project
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

#include "System.hpp"

#include <cpu-features.h>
#include <device_info/device_info.h>

#include "util/Error.hpp"
#include "util/JNIHelpers.hpp"
#include "util/Json.hpp"
#include "util/Log.hpp"

using namespace ancer;

//==============================================================================

namespace {
Log::Tag TAG{"ancer::system"};
}

//==============================================================================

namespace {
// -1 for little core, 0 for medium core, 1 for big core
using AffinityOffset = int;
[[nodiscard]] constexpr AffinityOffset ToAffinityOffset(ThreadAffinity aff) {
  assert (aff!=ThreadAffinity::kAll);
  return aff==ThreadAffinity::kLittleCore ? -1 :
         aff==ThreadAffinity::kMiddleCore ? 0 :
         1;
}

[[nodiscard]] constexpr ThreadAffinity FromAffinityOffset(AffinityOffset aff) {
  assert(-1 <= aff && aff <= 1);
  return (ThreadAffinity) ((int) ThreadAffinity::kLittleCore + aff);
}

const auto core_info = [] {
  struct {
    std::array<int, 4> num_cores;
    std::vector<ThreadAffinity> core_sizes;
  } info;

  androidgamesdk_deviceinfo_GameSdkDeviceInfoWithErrors proto;
  androidgamesdk_deviceinfo::ProtoDataHolder data_holder;
  createProto(proto, data_holder);

  const auto num_cpus = std::min((int) data_holder.cpuFreqs.size,
                                 android_getCpuCount());
  if (data_holder.cpuFreqs.size!=android_getCpuCount()) {
    Log::E(TAG, "GameSDK reports %d CPUs, but cpu-features reports %d.",
           data_holder.cpuFreqs.size, android_getCpuCount());
  }

  info.num_cores[(int) ThreadAffinity::kAll] = num_cpus;
  info.core_sizes.reserve(num_cpus);

  auto min_freq = data_holder.cpuFreqs.data[0];
  auto max_freq = data_holder.cpuFreqs.data[0];

  for (int i = 0; i < num_cpus; ++i) {
    const auto freq = data_holder.cpuFreqs.data[i];

    // Found a frequency outside of existing big/little range
    if (freq < min_freq || max_freq < freq) {
      // Determine which end we're replacing.
      const auto adj = ToAffinityOffset(
          freq < min_freq ? ThreadAffinity::kLittleCore
                          : ThreadAffinity::kBigCore);
      constexpr auto kMiddleIndex = (int) ThreadAffinity::kMiddleCore;
      if (min_freq==max_freq) {
        // All data so far as been recorded as middle. Take that
        // data and place it on the opposite end now that we've
        // found an opposite frequency for a big/little pair.
        std::swap(info.num_cores[kMiddleIndex],
                  info.num_cores[kMiddleIndex - adj]);
        for (auto &size : info.core_sizes) {
          size = (ThreadAffinity) ((int) size - adj);
        }
      } else {
        // Since we've found a new frequency for big/little, take
        // what we thought was the big/little end before and merge
        // it with whatever's already in the middle.
        info.num_cores[kMiddleIndex] += info.num_cores[kMiddleIndex + adj];
        info.num_cores[kMiddleIndex + adj] = 0;
        for (auto &size : info.core_sizes) {
          if (size==FromAffinityOffset(adj)) {
            size = ThreadAffinity::kMiddleCore;
          }
        }
      }
    }

    min_freq = std::min(min_freq, freq);
    max_freq = std::max(max_freq, freq);

    const auto which =
        ((min_freq==max_freq) ||
            (min_freq < freq && freq < max_freq))
        ? ThreadAffinity::kMiddleCore
        : freq==min_freq ? ThreadAffinity::kLittleCore
                         : ThreadAffinity::kBigCore;

    Log::D(TAG, "Core %d @ %d", i, freq);

    ++info.num_cores[(int) which];
    info.core_sizes.push_back(which);
  }

  Log::I(TAG, "%d cores detected : %d little @ %ld, %d big @ %ld, & %d middle)",
         info.num_cores[(int) ThreadAffinity::kAll],
         info.num_cores[(int) ThreadAffinity::kLittleCore], min_freq,
         info.num_cores[(int) ThreadAffinity::kBigCore], max_freq,
         info.num_cores[(int) ThreadAffinity::kMiddleCore]);

  return info;
}();
}

int ancer::NumCores(ThreadAffinity affinity) {
  return core_info.num_cores[(int) affinity];
}

bool ancer::SetThreadAffinity(int index, ThreadAffinity affinity) {
  cpu_set_t cpuset;
  CPU_ZERO(&cpuset);

  // i is real CPU index, j is the index restricted to only the given affinity
  for (int i = 0, j = 0; i < std::size(core_info.core_sizes); ++i) {
    if (affinity!=ThreadAffinity::kAll &&
        affinity!=core_info.core_sizes[i]) {
      continue;
    }
    if (j++==index || index==-1) {
      CPU_SET(i, &cpuset);
      if (index!=-1)
        break;
    }
  }

  if (sched_setaffinity(0, sizeof(cpu_set_t), &cpuset)==-1) {
    std::string err;
    switch (errno) {
      case EFAULT:err = "EFAULT";
        break;
      case EINVAL:err = "EINVAL";
        break;
      case EPERM:err = "EPERM";
        break;
      case ESRCH:err = "ESRCH";
        break;
      default:err = "(errno: " + std::to_string(errno) + ")";
        break;
    }
    Log::E(TAG, "SetThreadAffinity() - unable to set; error: " + err);
    return false;
  }
  return true;
}

bool ancer::SetThreadAffinity(ThreadAffinity affinity) {
  return SetThreadAffinity(-1, affinity);
}

//------------------------------------------------------------------------------


// TODO(tmillican@google.com): Not really the right place for this. Our system /
//  etc. code could really use a good refactoring...
std::string ancer::GetCpuInfo() {
  Json report;
  static constexpr const char *kAffinityNames[] = {
      "little_cores", "middle_cores", "big_cores"
  };
  for (int affinity = 0; affinity < (int) ThreadAffinity::kAll;
       ++affinity) {
    std::vector<int> cpus;
    for (int cpu = 0; cpu < core_info.core_sizes.size(); ++cpu) {
      if (core_info.core_sizes[cpu]==(ThreadAffinity) affinity) {
        cpus.push_back(cpu);
      }
    }
    report[kAffinityNames[affinity]] = {
        {"total", NumCores((ThreadAffinity) affinity)},
        {"cpus", cpus}
    };
  }
  return report.dump();
}
