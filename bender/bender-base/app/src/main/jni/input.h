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

#ifndef BENDER_BASE_APP_SRC_MAIN_JNI_INPUT_H_
#define BENDER_BASE_APP_SRC_MAIN_JNI_INPUT_H_

#include <map>
#include <vector>
#include <functional>

#include "button.h"

namespace Input {

struct Data {
  float lastX = 0, lastY = 0;
  float deltaX = 0, deltaY = 0;

  long lastTapTime = 0;
  long doubleTapThresholdTime = 200000000;
  bool doubleTap = false;

  int lastInputCount = 0;
  Button *lastButton = nullptr;
};

void getPointerPosition(AInputEvent *event, int *outX, int *outY);
void testDoubleTap(AInputEvent *event, Data *input);
void updateInputData(AInputEvent *event, Data *input);
void clearInput(Data *input);

int32_t handler(android_app *app, AInputEvent *event);
}

#endif //BENDER_BASE_APP_SRC_MAIN_JNI_INPUT_H_
