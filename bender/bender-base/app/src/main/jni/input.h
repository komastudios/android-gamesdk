//
// Created by theoking on 11/6/2019.
//

#ifndef BENDER_BASE_APP_SRC_MAIN_JNI_INPUT_H_
#define BENDER_BASE_APP_SRC_MAIN_JNI_INPUT_H_

#include <button.h>
namespace Input{

struct Data {
  float lastX = 0, lastY = 0;
  float deltaX = 0, deltaY = 0;

  long lastTapTime = 0;
  long doubleTapThresholdTime = 200000000;
  bool doubleTap = false;

  int lastInputCount = 0;
  Button *lastButton = nullptr;
};

inline void getPointerPosition(AInputEvent *event, int *outX, int *outY) {
  *outX = AMotionEvent_getX(event, 0);
  *outY = AMotionEvent_getY(event, 0);
}

inline void testDoubleTap(AInputEvent *event, Data *input) {
  if (AMotionEvent_getAction(event) == AMOTION_EVENT_ACTION_DOWN) {
    long currTapTime = AMotionEvent_getEventTime(event);
    long deltaTapTime = currTapTime - input->lastTapTime;
    if (deltaTapTime < input->doubleTapThresholdTime) {
      input->doubleTap = true;
    }
    input->lastTapTime = currTapTime;
  }
}

inline void updateInputData(AInputEvent *event, Data *input){
  int newX, newY;
  getPointerPosition(event, &newX, &newY);
  if (AMotionEvent_getAction(event) == AMOTION_EVENT_ACTION_DOWN){
    input->lastX = newX;
    input->lastY = newY;
  }
  else if (AMotionEvent_getAction(event) == AMOTION_EVENT_ACTION_MOVE){
    input->deltaX = newX - input->lastX;
    input->deltaY = newY - input->lastY;
    input->lastX = newX;
    input->lastY = newY;
  }

  input->lastInputCount = AMotionEvent_getPointerCount(event);
  testDoubleTap(event, input);
}

inline void clearInput(Data *input) {
  input->lastX = 0;
  input->lastY = 0;
  input->deltaX = 0;
  input->deltaY = 0;
  input->doubleTap = false;
}

}


#endif //BENDER_BASE_APP_SRC_MAIN_JNI_INPUT_H_
