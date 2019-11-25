//
// Created by theoking on 11/25/2019.
//

#include "button.h"

Button::Button(int x, int xExtent, int y, int yExtent, const char *text){
  x_min = x;
  x_max = x + xExtent;
  y_min = y;
  y_max = y + yExtent;
  label = text;
  to_present = text;
  onUp = empty;
  onDown = empty;
  onHold = empty;
}

bool Button::testHit(int x, int y) {
  return x > x_min && x < x_max && y > y_min && y < y_max;
}