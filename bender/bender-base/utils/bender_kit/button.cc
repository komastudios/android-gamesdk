//
// Created by theoking on 11/25/2019.
//

#include "button.h"

Button::Button(int x, int xExtent, int y, int yExtent, std::function<void()> newAction){
  xMin = x;
  xMax = x + xExtent;
  yMin = y;
  yMax = y + yExtent;
  action = newAction;
}

void Button::testHit(int x, int y) {
  if (x > xMin && x < xMax && y > yMin && y < yMax){
    action;
  }
}