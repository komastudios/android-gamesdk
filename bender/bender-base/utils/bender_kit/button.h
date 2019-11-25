//
// Created by theoking on 11/25/2019.
//

#ifndef BENDER_BASE_UTILS_BENDER_KIT_BUTTON_H_
#define BENDER_BASE_UTILS_BENDER_KIT_BUTTON_H_

#include <functional>

class Button {
 public:
  Button(int x, int xExtent, int y, int yExtent, std::function<void()> newAction);
  void testHit(int x, int y);

 private:
  int xMin, yMin, xMax, yMax;
  std::function<void()> action;

};

#endif //BENDER_BASE_UTILS_BENDER_KIT_BUTTON_H_
