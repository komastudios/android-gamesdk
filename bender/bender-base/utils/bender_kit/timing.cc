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
#include <string>
#include "../../app/src/main/jni/trace.h"

static const char *kTAG = "BenderKit";
#define LOGI(...) \
  ((void)__android_log_print(ANDROID_LOG_INFO, kTAG, __VA_ARGS__))

namespace Timing {

EventTiming timer;

EventTiming::EventTiming() {
  application_start_time = std::chrono::high_resolution_clock::now();
}

void EventTiming::startEvent(const char *name) {
  TRACE_BEGIN_SECTION(name);
  Event newSection;
  newSection.start_time = std::chrono::high_resolution_clock::now() - application_start_time;
  newSection.name = name;
  newSection.parent_event = current_event;
  if (current_event == nullptr) {
    newSection.level = 0;
    newSection.number = current_major_event_num++;
    major_events.push_back(newSection);
    current_event = &major_events.back();
  } else {
    newSection.level = current_event->level + 1;
    current_event->sub_events.push_back(newSection);
    current_event = &current_event->sub_events.back();
  }
}

void EventTiming::stopEvent() {
  if (!current_event)
    return;

  TRACE_END_SECTION();
  current_event->duration = std::chrono::high_resolution_clock::now()
      - (current_event->start_time + application_start_time);
  current_event = current_event->parent_event;
}

Event EventTiming::getLastMajorEvent() {
  return major_events.back();
}

float EventTiming::getFramerate(int numFrames, int mostRecentFrame) {
  mostRecentFrame = major_events.size() < mostRecentFrame ? major_events.size() : mostRecentFrame;
  int framesCount = mostRecentFrame < numFrames ? mostRecentFrame : numFrames;

  float sum = 0;
  for (int x = 0; x < framesCount; x++) {
    sum += major_events[mostRecentFrame - x].duration.count();
  }

  return framesCount / (sum * NS_TO_S);
}

void printEvent(Event event) {
  LOGI("Event %u - %s: Duration: %fms Framerate: %fFPS",
       event.number,
       event.name,
       event.duration.count() * NS_TO_MS,
       timer.getFramerate(100, event.number));

  std::vector<Event> currentList(event.sub_events.rbegin(), event.sub_events.rend());
  while (!currentList.empty()) {
    Event currEvent = currentList.back();
    currentList.pop_back();
    LOGI("%s%s: \tStart Time: %f s \tDuration: %f ms",
         std::string(currEvent.level, '\t').c_str(),
         currEvent.name,
         currEvent.start_time.count() * NS_TO_S,
         currEvent.duration.count() * NS_TO_MS);
    currentList.insert(currentList.end(),
                       currEvent.sub_events.rbegin(),
                       currEvent.sub_events.rend());
  }
}
}