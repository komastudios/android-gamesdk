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

#ifndef BENDER_BASE_UTILS_BENDER_KIT_TIMING_H_
#define BENDER_BASE_UTILS_BENDER_KIT_TIMING_H_

#include <chrono>
#include <vector>
#include <stack>
#include <map>

#define NS_TO_S 1.0E-9
#define NS_TO_MS 1.0E-6

namespace Timing {

enum EventType {
  MAIN_LOOP,
  START_FRAME,
  OTHER,
  EVENT_TYPE_COUNT
};

struct Event {
  const char *name;
  int level, number = -1;
  EventType type;
  std::chrono::high_resolution_clock::duration start_time, duration;
  Event *parent_event = nullptr;
  std::stack<Event *> sub_events;
};

void printEvent(Event &event);

class EventTiming {
 public:
  EventTiming();
  Event *getLastMajorEvent();
  void getFramerate(int numFrames, int mostRecentFrame, int *fps, float *frameTime);
  void time(const char * name, EventType type, std::function<void()> eventToTime) {
    startEvent(name, type);
    eventToTime();
    stopEvent();
  }

 private:
  void startEvent(const char *name, EventType type);
  void stopEvent();

  std::vector<Event *> major_events;
  std::vector<std::vector<Event *>> event_buckets;
  int current_major_event_num_ = 0;
  Event *current_event = nullptr;
  std::chrono::high_resolution_clock::time_point application_start_time_;
  std::vector<Event> eventPool;

};

extern EventTiming timer;
}

#endif //BENDER_BASE_UTILS_BENDER_KIT_TIMING_H_
