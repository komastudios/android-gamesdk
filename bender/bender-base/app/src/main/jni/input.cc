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

namespace Input {

std::vector<Button> buttons;

std::map<int, std::function<void(Data *, std::vector<Button> &)>> handlers = {
    {AMOTION_EVENT_ACTION_DOWN, actionDownHandler},
    {AMOTION_EVENT_ACTION_UP, actionUpHandler},
    {AMOTION_EVENT_ACTION_MOVE, actionMoveHandler},
};

int32_t handler(android_app *app, AInputEvent *event) {
  if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION) {
    Input::Data *inputData = (Input::Data *) app->userData;
    Input::updateInputData(event, inputData);

    if (inputData->lastButton != nullptr
        && !inputData->lastButton->testHit(inputData->lastX, inputData->lastY)) {
      inputData->lastButton->onButtonUp();
      inputData->lastButton = nullptr;
    }

    int action = AMotionEvent_getAction(event);
    if (Input::handlers.find(action) != Input::handlers.end()) {
      Input::handlers[action](inputData, buttons);
    }
    return 1;
  }
  return 0;
}

void getPointerPosition(AInputEvent *event, int *outX, int *outY) {
  *outX = AMotionEvent_getX(event, 0);
  *outY = AMotionEvent_getY(event, 0);
}

void clearInput(Data *input) {
  input->lastX = 0;
  input->lastY = 0;
  input->deltaX = 0;
  input->deltaY = 0;
  input->doubleTap = false;
}

void testDoubleTap(AInputEvent *event, Data *input) {
  if (AMotionEvent_getAction(event) == AMOTION_EVENT_ACTION_DOWN) {
    long currTapTime = AMotionEvent_getEventTime(event);
    long deltaTapTime = currTapTime - input->lastTapTime;
    if (deltaTapTime < input->doubleTapThresholdTime) {
      input->doubleTap = true;
    }
    input->lastTapTime = currTapTime;
  }
}

void updateInputData(AInputEvent *event, Data *input) {
  int newX, newY;
  getPointerPosition(event, &newX, &newY);
  if (AMotionEvent_getAction(event) == AMOTION_EVENT_ACTION_DOWN) {
    input->lastX = newX;
    input->lastY = newY;
  } else if (AMotionEvent_getAction(event) == AMOTION_EVENT_ACTION_MOVE) {
    input->deltaX = newX - input->lastX;
    input->deltaY = newY - input->lastY;
    input->lastX = newX;
    input->lastY = newY;
  }

  input->lastInputCount = AMotionEvent_getPointerCount(event);
  testDoubleTap(event, input);
}

void actionDownHandler(Input::Data *inputData, std::vector<Button> &buttons) {
  for (auto &button : buttons) {
    if (button.testHit(inputData->lastX, inputData->lastY)) {
      inputData->lastButton = &button;
      button.onButtonDown();
    }
  }
}

void actionMoveHandler(Input::Data *inputData, std::vector<Button> &buttons) {
  for (auto &button : buttons) {
    if (button.testHit(inputData->lastX, inputData->lastY)) {
      if (inputData->lastButton == nullptr) {
        button.onButtonDown();
      } else {
        button.onButtonHold();
      }
      inputData->lastButton = &button;
    }
  }
}

void actionUpHandler(Input::Data *inputData, std::vector<Button> &buttons) {
  for (auto &button : buttons) {
    if (button.testHit(inputData->lastX, inputData->lastY)) {
      button.onButtonUp();
    }
  }
  Input::clearInput(inputData);
}

}