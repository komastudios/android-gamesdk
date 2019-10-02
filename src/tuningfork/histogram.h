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

#pragma once

#include "tuningfork_internal.h"

#include <inttypes.h>
#include <vector>
#include <string>

namespace tuningfork {

class Histogram {
  public:

    enum class Mode {
        HISTOGRAM = 0, // Store in the buckets directly
        AUTO_RANGE = 1, // Store a buffer of events until they fill samples_, then bucket
        EVENTS_ONLY = 2 // Store a circular buffer of events and never bucket them
    };

  private:

    typedef double Sample;
    static constexpr int kAutoSizeNumStdDev = 3;
    static constexpr double kAutoSizeMinBucketSizeMs = 0.1;
    Mode mode_;
    Sample start_ms_, end_ms_, bucket_dt_ms_;
    uint32_t num_buckets_;
    std::vector<uint32_t> buckets_;
    std::vector<Sample> samples_;
    size_t count_;
    size_t next_event_index_;

  public:

    static constexpr int kDefaultNumBuckets = 30;

    explicit Histogram(float start_ms = 0, float end_ms = 0, int num_buckets_between = kDefaultNumBuckets,
                       bool never_bucket = false);
    explicit Histogram(const TFHistogram&, bool never_bucket = false);

    // Add a sample delta time
    void Add(Sample dt_ms);

    // Reset the histogram
    void Clear();

    // Get the total number of samples added so far
    size_t Count() const { return count_; }

    // Get the histogram as a JSON object, for testing
    std::string ToDebugJSON() const;

    // Use the data we have to construct the bucket ranges. This is called automatically after
    //  sizeAtWhichToRange samples have been collected, if we are auto-ranging.
    void CalcBucketsFromSamples();

    // Only to be used for testing
    void SetCounts(const std::vector<uint32_t>& counts) {
        buckets_ = counts;
    }

    TFErrorCode AddCounts(const std::vector<uint32_t>& counts);

    bool operator==(const Histogram& h) const;

    const std::vector<uint32_t>& buckets() const { return buckets_;}

    const std::vector<Sample>& samples() const { return samples_;}

    Mode GetMode() const { return mode_; }
    Sample StartMs() const { return start_ms_; }
    Sample EndMs() const { return end_ms_; }

    friend class ClearcutSerializer;
};

} // namespace tuningfork {
