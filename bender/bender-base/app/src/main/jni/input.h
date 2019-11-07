//
// Created by theoking on 11/6/2019.
//

#ifndef BENDER_BASE_APP_SRC_MAIN_JNI_INPUT_H_
#define BENDER_BASE_APP_SRC_MAIN_JNI_INPUT_H_

namespace Input{

struct Data {
  float lastX = 0, lastY = 0;
  float deltaX = 0, deltaY = 0;

  float lastXPan = 0, lastYPan = 0;
  float deltaXPan = 0, deltaYPan = 0;

  long lastTapTime = 0;
  long doubleTapThresholdTime = 200000000;
  bool doubleTapHoldUpper = false;
  bool doubleTapHoldLower = false;

  int lastInputCount = 0;
};

inline void testDoubleTapHold(android_app *app, AInputEvent *event, Data *input) {
  if (AMotionEvent_getAction(event) == AMOTION_EVENT_ACTION_DOWN) {
    long currTapTime = AMotionEvent_getEventTime(event);
    long deltaTapTime = currTapTime - input->lastTapTime;
    if (deltaTapTime < input->doubleTapThresholdTime) {
      if (AMotionEvent_getY(event, 0) < ANativeWindow_getHeight(app->window) / 2) {
        input->doubleTapHoldUpper = true;
      } else {
        input->doubleTapHoldLower = true;
      }
    }
    input->lastTapTime = currTapTime;
  }
}

inline void testRotate(android_app *app, AInputEvent *event, Data *input) {
  if (AMotionEvent_getAction(event) == AMOTION_EVENT_ACTION_DOWN || input->lastInputCount > 1) {
    input->lastX = AMotionEvent_getX(event, 0);
    input->lastY = AMotionEvent_getY(event, 0);
  } else if (AMotionEvent_getAction(event) == AMOTION_EVENT_ACTION_MOVE) {
    float currX = AMotionEvent_getX(event, 0);
    float currY = AMotionEvent_getY(event, 0);
    input->deltaX = currX - input->lastX;
    input->deltaY = currY - input->lastY;
    input->lastX = currX;
    input->lastY = currY;
  }
  input->lastInputCount = 1;
}

inline void testPan(android_app *app, AInputEvent *event, Data *input) {
  if (AMotionEvent_getAction(event) == AMOTION_EVENT_ACTION_DOWN || input->lastInputCount < 2) {
    input->lastXPan = (AMotionEvent_getX(event, 0) + AMotionEvent_getX(event, 1)) / 2;
    input->lastYPan = (AMotionEvent_getY(event, 0) + AMotionEvent_getY(event, 1)) / 2;
  } else if (AMotionEvent_getAction(event) == AMOTION_EVENT_ACTION_MOVE) {
    float currX = (AMotionEvent_getX(event, 0) + AMotionEvent_getX(event, 1)) / 2;
    float currY = (AMotionEvent_getY(event, 0) + AMotionEvent_getY(event, 1)) / 2;
    input->deltaXPan = currX - input->lastXPan;
    input->deltaYPan = currY - input->lastYPan;
    input->lastXPan = currX;
    input->lastYPan = currY;
  }
  input->lastInputCount = 2;
}

inline void clearInput(Data *input) {
  input->lastX = 0;
  input->lastY = 0;
  input->lastXPan = 0;
  input->lastYPan = 0;
  input->deltaX = 0;
  input->deltaY = 0;
  input->deltaXPan = 0;
  input->deltaYPan = 0;
  input->doubleTapHoldUpper = false;
  input->doubleTapHoldLower = false;
}

}


#endif //BENDER_BASE_APP_SRC_MAIN_JNI_INPUT_H_
