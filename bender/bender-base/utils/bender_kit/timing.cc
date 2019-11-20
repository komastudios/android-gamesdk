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
  application_start_time_ = std::chrono::high_resolution_clock::now();
  event_buckets.resize(EVENT_TYPE_COUNT);
  time("Application Start", OTHER, [](){});
}

void EventTiming::startEvent(const char *name, EventType type) {
  TRACE_BEGIN_SECTION(name);
  Event newEvent;
  newEvent.start_time = std::chrono::high_resolution_clock::now() - application_start_time_;
  newEvent.duration = std::chrono::high_resolution_clock::duration::zero();
  newEvent.name = name;
  newEvent.type = type;
  newEvent.parent_event = current_event;
  if (current_event == nullptr) {
    newEvent.level = 0;
    newEvent.number = current_major_event_num_++;
    eventPool.push_back(newEvent);
    major_events.push_back(&eventPool.back());
    current_event = major_events.back();
  } else {
    newEvent.level = current_event->level + 1;
    eventPool.push_back(newEvent);
    current_event->sub_events.push(&eventPool.back());
    current_event = current_event->sub_events.top();
  }
  event_buckets[newEvent.type].push_back(&eventPool.back());
}

void EventTiming::stopEvent() {
  if (!current_event)
    return;

  TRACE_END_SECTION();
  current_event->duration = std::chrono::high_resolution_clock::now()
      - (current_event->start_time + application_start_time_);
  current_event = current_event->parent_event;
}

Event *EventTiming::getLastMajorEvent() {
  return major_events.back();
}

void EventTiming::getFramerate(int numFrames, int mostRecentFrame, int *fps, float *frameTime) {
  int mostRecentFrameMain = event_buckets[MAIN_LOOP].size() - 2;
  mostRecentFrame = (int)event_buckets[START_FRAME].size() - 2 < mostRecentFrame ? (int)event_buckets[START_FRAME].size() - 2: mostRecentFrame;
  int framesCount = mostRecentFrame < numFrames ? mostRecentFrame : numFrames;

  float sumFPS = 0;
  float sumFrameTime = 0;
  for (int x = 0; x < framesCount; x++) {
    sumFPS += event_buckets[MAIN_LOOP][mostRecentFrameMain - x]->duration.count();
    sumFrameTime += event_buckets[START_FRAME][mostRecentFrame - x]->duration.count();
  }

  *fps = framesCount / (sumFPS * NS_TO_S);
  *frameTime = (sumFrameTime * NS_TO_MS) / framesCount;
}

void printEvent(Event &event) {
  int fps;
  float frameTime;
  timer.getFramerate(100, event.number, &fps, &frameTime);
  LOGI("Event %u - %s: Duration: %.3f ms Framerate: %d FPS Average Frame Time %.3f ms",
       event.number,
       event.name,
       event.duration.count() * NS_TO_MS,
       fps,
       frameTime);

  std::stack<Event *> currentList;
  while (!event.sub_events.empty()){
    currentList.push(event.sub_events.top());
    event.sub_events.pop();
  }

  while (!currentList.empty()) {
    Event *currEvent = currentList.top();
    currentList.pop();
    LOGI("%s%s: \tStart Time: %.6f s \tDuration: %.3f ms",
         std::string(currEvent->level, '\t').c_str(),
         currEvent->name,
         currEvent->start_time.count() * NS_TO_S,
         currEvent->duration.count() * NS_TO_MS);
    for (int x = 0; x < currEvent->sub_events.size(); x++) {
      currentList.push(currEvent->sub_events.top());
      currEvent->sub_events.pop();
    }
  }
}
}