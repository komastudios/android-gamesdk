// Copyright 2016 Google Inc. All Rights Reserved.
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
#include <android/log.h>
#include <android_native_app_glue.h>
#include "vulkan_main.h"

int32_t input_cmd(android_app *app, AInputEvent *event) {
  Input *input = (Input *) app->userData;
  if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION) {

    if (AMotionEvent_getAction(event) == AMOTION_EVENT_ACTION_DOWN) {
      long currTapTime = AMotionEvent_getEventTime(event);
      long deltaTapTime = currTapTime - input->lastTapTime;
      if (deltaTapTime < input->doubleTapThresholdTime){
        input->doubleTapHold = true;
      }
      input->lastTapTime = currTapTime;
    }

    if (AMotionEvent_getPointerCount(event) == 1) {
      if (AMotionEvent_getAction(event) == AMOTION_EVENT_ACTION_DOWN || input->lastInputCount > 1) {
        input->lastX = AMotionEvent_getX(event, 0);
        input->lastY = AMotionEvent_getY(event, 0);
      }
      else if (AMotionEvent_getAction(event) == AMOTION_EVENT_ACTION_MOVE) {
        float currX = AMotionEvent_getX(event, 0);
        float currY = AMotionEvent_getY(event, 0);
        input->deltaX = currX - input->lastX;
        input->deltaY = currY - input->lastY;
        input->lastX = currX;
        input->lastY = currY;
      }
      input->lastInputCount = 1;
    }

    else if (AMotionEvent_getPointerCount(event) == 2) {
      if (AMotionEvent_getAction(event) == AMOTION_EVENT_ACTION_DOWN || input->lastInputCount < 2) {
        input->lastXPan = (AMotionEvent_getX(event, 0) + AMotionEvent_getX(event, 1)) / 2;
        input->lastYPan = (AMotionEvent_getY(event, 0) + AMotionEvent_getY(event, 1)) / 2;
      }
      else if (AMotionEvent_getAction(event) == AMOTION_EVENT_ACTION_MOVE) {
        float currX = (AMotionEvent_getX(event, 0) + AMotionEvent_getX(event, 1)) / 2;
        float currY = (AMotionEvent_getY(event, 0) + AMotionEvent_getY(event, 1)) / 2;
        input->deltaXPan = currX - input->lastXPan;
        input->deltaYPan = currY - input->lastYPan;
        input->lastXPan = currX;
        input->lastYPan = currY;
      }
      input->lastInputCount = 2;
    }

    if (AMotionEvent_getAction(event) == AMOTION_EVENT_ACTION_UP) {
      input->lastX = 0;
      input->lastY = 0;
      input->lastXPan = 0;
      input->lastYPan = 0;
      input->deltaX = 0;
      input->deltaY = 0;
      input->deltaXPan = 0;
      input->deltaYPan = 0;
      input->doubleTapHold = false;
    }
    return 1;
  }
  return 0;
}

// Process the next main command.
void handle_cmd(android_app *app, int32_t cmd) {
  switch (cmd) {
    case APP_CMD_INIT_WINDOW:
      // The window is being shown, get it ready.
      InitVulkan(app);
      break;
    case APP_CMD_TERM_WINDOW:
      // The window is being hidden or closed, clean it up.
      DeleteVulkan();
      break;
    default:
      __android_log_print(ANDROID_LOG_INFO, "Bender",
                          "event not handled: %d", cmd);
  }
}

void android_main(struct android_app *app) {
  // Set the callback to process system events
  app->onAppCmd = handle_cmd;
  app->onInputEvent = input_cmd;
  app->userData = (void *) new Input;

  // Used to poll the events in the main loop
  int events;
  android_poll_source *source;

  // Main loop
  do {
    if (ALooper_pollAll(IsVulkanReady() ? 1 : 0, nullptr,
                        &events, (void **) &source) >= 0) {
      if (source != NULL) source->process(app, source);
    }

    // render if vulkan is ready
    if (IsVulkanReady()) {
      VulkanDrawFrame((Input *)app->userData);
    }
  } while (app->destroyRequested == 0);
}
