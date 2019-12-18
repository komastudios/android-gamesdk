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

#ifndef BENDER_BASE_INPUT_H_
#define BENDER_BASE_INPUT_H_

#include "button.h"

#include <map>
#include <vector>
#include <functional>

namespace input {

struct Data {
  float last_x = 0, last_y = 0;
  float delta_x = 0, delta_y = 0;

  long last_tap_time = 0;
  long double_tap_threshold_time = 200000000;
  bool double_tap = false;

  int last_input_count = 0;
  Button *last_button = nullptr;
};

void GetPointerPosition(AInputEvent *event, int *outX, int *outY);
void TestDoubleTap(AInputEvent *event, Data *input);
void UpdateInputData(AInputEvent *event, Data *input);
void ClearInput(Data *input);

int32_t Handler(android_app *app, AInputEvent *event);
}

#endif //BENDER_BASE_INPUT_H_
