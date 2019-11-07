//
// Created by theoking on 11/6/2019.
//

#ifndef BENDER_BASE_APP_SRC_MAIN_JNI_INPUT_H_
#define BENDER_BASE_APP_SRC_MAIN_JNI_INPUT_H_

struct Input {
  float lastX = 0, lastY = 0;
  float deltaX = 0, deltaY = 0;

  float lastXPan = 0, lastYPan = 0;
  float deltaXPan = 0, deltaYPan = 0;

  long lastTapTime = 0;
  long doubleTapThresholdTime = 190000000;
  bool doubleTapHold = false;

  int lastInputCount = 0;
};

#endif //BENDER_BASE_APP_SRC_MAIN_JNI_INPUT_H_
