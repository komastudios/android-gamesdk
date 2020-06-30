/*
 * Copyright 2018 The Android Open Source Project
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

#include "prong.h"

#define LOG_TAG "TuningFork"
#include <sstream>
#include <string>

#include "Log.h"

namespace tuningfork {

Prong::Prong(InstrumentationKey instrumentation_key,
             const SerializedAnnotation& annotation,
             const Settings::Histogram& histogram_settings, bool loading)
    : instrumentation_key_(instrumentation_key),
      annotation_(annotation),
      histogram_(histogram_settings, loading),
      last_time_(TimePoint::min()),
      duration_(Duration::zero()),
      loading_(loading) {}

void Prong::Tick(TimePoint t) {
    if (last_time_ != std::chrono::steady_clock::time_point::min())
        Trace(t - last_time_);
    last_time_ = t;
}

void Prong::Trace(Duration dt) {
    // The histogram stores millisecond values as doubles
    histogram_.Add(
        double(
            std::chrono::duration_cast<std::chrono::nanoseconds>(dt).count()) /
        1000000);
    duration_ += dt;
}

void Prong::Clear() {
    last_time_ = std::chrono::steady_clock::time_point::min();
    histogram_.Clear();
}

size_t Prong::Count() const { return histogram_.Count(); }

void Prong::SetInstrumentKey(InstrumentationKey key) {
    instrumentation_key_ = key;
}

}  // namespace tuningfork
