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
  Input::Data *inputData = (Input::Data *) app->userData;
  if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION) {
    Input::testDoubleTapHold(app, event, inputData);
    if (AMotionEvent_getPointerCount(event) == 1) {
      Input::testRotate(app, event, inputData);
    }
    else if (AMotionEvent_getPointerCount(event) == 2) {
      Input::testPan(app, event, inputData);
    }
    if (AMotionEvent_getAction(event) == AMOTION_EVENT_ACTION_UP) {
      Input::clearInput(inputData);
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
    case APP_CMD_CONFIG_CHANGED:
      ScreenChange();
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
  app->userData = (void *) new Input::Data;

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
      VulkanDrawFrame((Input::Data *)app->userData);
    }
  } while (app->destroyRequested == 0);
}
