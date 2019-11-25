//
// Created by theoking on 11/25/2019.
//

#ifndef BENDER_BASE_UTILS_BENDER_KIT_BUTTON_H_
#define BENDER_BASE_UTILS_BENDER_KIT_BUTTON_H_

#include <functional>
#include <string>

class Button {
 public:
  Button(int x, int xExtent, int y, int yExtent, const char *text);
  bool testHit(int x, int y);

  std::string getLabel() const { return to_present; }
  float getNormalizedX(int deviceX) const { return (x_min - (deviceX / 2)) / (float)(deviceX / 2); }
  float getNormalizedY(int deviceY) const { return (y_min - (deviceY / 2)) / (float)(deviceY / 2); }
  int getX() const { return x_min; }
  int getY() const { return y_min; }

  void onButtonDown(){
    to_present = "X";
    onDown();
  }

  void onButtonHold(){
    onHold();
  }

  void onButtonUp(){
    to_present = label;
    onUp();
  }

  std::function<void()> onDown;
  std::function<void()> onUp;
  std::function<void()> onHold;

 private:
  int x_min, y_min, x_max, y_max;
  std::string label;
  std::string to_present;
  static void empty(){ }

};

#endif //BENDER_BASE_UTILS_BENDER_KIT_BUTTON_H_
