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

#include "input.h"

namespace input {

void getPointerPosition(AInputEvent *event, int *outX, int *outY) {
  *outX = AMotionEvent_getX(event, 0);
  *outY = AMotionEvent_getY(event, 0);
}

void ClearInput(Data *input) {
  input->last_x = 0;
  input->last_y = 0;
  input->delta_x = 0;
  input->delta_y = 0;
  input->double_tap = false;
}

void TestDoubleTap(AInputEvent *event, Data *input) {
  if (AMotionEvent_getAction(event) == AMOTION_EVENT_ACTION_DOWN) {
    long curr_tap_time = AMotionEvent_getEventTime(event);
    long delta_tap_time = curr_tap_time - input->last_tap_time;
    if (delta_tap_time < input->double_tap_threshold_time) {
      input->double_tap = true;
    }
    input->last_tap_time = curr_tap_time;
  }
}

void UpdateInputData(AInputEvent *event, Data *input) {
  int new_x, new_y;
  getPointerPosition(event, &new_x, &new_y);
  if (AMotionEvent_getAction(event) == AMOTION_EVENT_ACTION_DOWN) {
    input->last_x = new_x;
    input->last_y = new_y;
  } else if (AMotionEvent_getAction(event) == AMOTION_EVENT_ACTION_MOVE) {
    input->delta_x = new_x - input->last_x;
    input->delta_y = new_y - input->last_y;
    input->last_x = new_x;
    input->last_y = new_y;
  }

  input->last_input_count = AMotionEvent_getPointerCount(event);
  TestDoubleTap(event, input);
}

}