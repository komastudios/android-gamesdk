// Copyright 2019 Google Inc. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "timing.h"
#include "../../app/src/main/jni/trace.h"

#include <string>

static const char *kTAG = "BenderKit";
#define LOGI(...) \
  ((void)__android_log_print(ANDROID_LOG_INFO, kTAG, __VA_ARGS__))

namespace timing {

EventTiming timer;

EventTiming::EventTiming() : event_pool_(4000) {
  application_start_time_ = std::chrono::high_resolution_clock::now();
  event_buckets_.resize(EVENT_TYPE_COUNT, CircularBuffer<Event *>(4000));
  Time("Application Start", OTHER, [](){});
}

Event& EventTiming::StartEvent(const char *name, EventType type) {
  TRACE_BEGIN_SECTION(name);
  event_pool_.PushBack(Event(name, ++current_event_level_, current_event_number_++, type,
                           std::chrono::high_resolution_clock::now() -
                           application_start_time_,
                           std::chrono::high_resolution_clock::duration::zero()));
  event_buckets_[type].PushBack(&event_pool_.Back());

  return event_pool_.Back();
}

void EventTiming::StopEvent(Event &event) {
  TRACE_END_SECTION();
    event.duration = std::chrono::high_resolution_clock::now() -
                     (event.start_time + application_start_time_);

  current_event_level_ = event.level - 1;
}


void EventTiming::GetFramerate(int num_frames, int *fps, float *frame_time) {
  int most_recent_frameMain = event_buckets_[MAIN_LOOP].Size() - 2;
  int most_recent_frame = event_buckets_[START_FRAME].Size() - 2;
  int frames_count = most_recent_frame < num_frames ? most_recent_frame : num_frames;

  float sum_fps = 0;
  float sum_frame_time = 0;
  for (int i = 0; i < frames_count; i++) {
    sum_fps += event_buckets_[MAIN_LOOP][most_recent_frameMain - i]->duration.count();
    sum_frame_time += event_buckets_[START_FRAME][most_recent_frame - i]->duration.count();
  }

  *fps = frames_count / (sum_fps * NS_TO_S);
  *frame_time = (sum_frame_time * NS_TO_MS) / frames_count;
}

void EventTiming::PrintLastEvent() {
  int fps;
  float frame_time;
  timer.GetFramerate(100, &fps, &frame_time);

  int current_major_event = -1;
  for (int i = event_pool_.Size() - 1; i > -1; i--) {
      if (event_pool_[i].level == 0) {
          current_major_event = i;
          break;
      }
  }

  LOGI("Event %u - %s: Duration: %.3f ms Framerate: %d FPS Average Frame Time %.3f ms",
       event_pool_[current_major_event].number,
       event_pool_[current_major_event].name,
       event_pool_[current_major_event].duration.count() * NS_TO_MS,
       fps,
       frame_time);

  current_major_event++;
  while (current_major_event < event_pool_.Size() && event_pool_[current_major_event].level != 0) {
    LOGI("%s%s: \tStart Time: %.6f s \tDuration: %.3f ms",
         std::string(event_pool_[current_major_event].level, '\t').c_str(),
         event_pool_[current_major_event].name,
         event_pool_[current_major_event].start_time.count() * NS_TO_S,
         event_pool_[current_major_event].duration.count() * NS_TO_MS);
      current_major_event++;
  }
}

void EventTiming::ResetEvents() {
    event_pool_.Clear();
    for (auto &buffer : event_buckets_)
        buffer.Clear();
}
}