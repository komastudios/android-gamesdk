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

EventTiming::EventTiming() {
  application_start_time_ = std::chrono::high_resolution_clock::now();
  event_buckets_.resize(EVENT_TYPE_COUNT);
  Time("Application Start", OTHER, [](){});
}

void EventTiming::StartEvent(const char *name, EventType type) {
  TRACE_BEGIN_SECTION(name);
  Event new_event;
  new_event.start_time = std::chrono::high_resolution_clock::now() - application_start_time_;
  new_event.duration = std::chrono::high_resolution_clock::duration::zero();
  new_event.name = name;
  new_event.type = type;
  new_event.parent_event = current_event_;
  if (current_event_ == nullptr) {
    new_event.level = 0;
    new_event.number = current_major_event_num_++;
    event_pool_.push_back(new_event);
    major_events_.push_back(&event_pool_.back());
    current_event_ = major_events_.back();
  } else {
    new_event.level = current_event_->level + 1;
    event_pool_.push_back(new_event);
    current_event_->sub_events.push(&event_pool_.back());
    current_event_ = current_event_->sub_events.top();
  }
  event_buckets_[new_event.type].push_back(&event_pool_.back());
}

void EventTiming::StopEvent() {
  if (!current_event_)
    return;

  TRACE_END_SECTION();
  current_event_->duration = std::chrono::high_resolution_clock::now()
      - (current_event_->start_time + application_start_time_);
  current_event_ = current_event_->parent_event;
}

Event *EventTiming::GetLastMajorEvent() {
  return major_events_.back();
}

void EventTiming::GetFramerate(int num_frames, int most_recent_frame, int *fps, float *frame_time) {
  int most_recent_frameMain = event_buckets_[MAIN_LOOP].size() - 2;
  most_recent_frame = (int)event_buckets_[START_FRAME].size() - 2 < most_recent_frame ? (int)event_buckets_[START_FRAME].size() - 2: most_recent_frame;
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

void PrintEvent(Event &event) {
  int fps;
  float frame_time;
  timer.GetFramerate(100, event.number, &fps, &frame_time);
  LOGI("Event %u - %s: Duration: %.3f ms Framerate: %d FPS Average Frame Time %.3f ms",
       event.number,
       event.name,
       event.duration.count() * NS_TO_MS,
       fps,
       frame_time);

  std::stack<Event *> current_list;
  while (!event.sub_events.empty()){
    current_list.push(event.sub_events.top());
    event.sub_events.pop();
  }

  while (!current_list.empty()) {
    Event *curr_event = current_list.top();
    current_list.pop();
    LOGI("%s%s: \tStart Time: %.6f s \tDuration: %.3f ms",
         std::string(curr_event->level, '\t').c_str(),
         curr_event->name,
         curr_event->start_time.count() * NS_TO_S,
         curr_event->duration.count() * NS_TO_MS);
    for (int i = 0; i < curr_event->sub_events.size(); i++) {
      current_list.push(curr_event->sub_events.top());
      curr_event->sub_events.pop();
    }
  }
}
}