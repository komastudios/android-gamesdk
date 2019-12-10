//
// Created by theoking on 11/25/2019.
//

#ifndef BENDER_BASE_UTILS_BENDER_KIT_BUTTON_H_
#define BENDER_BASE_UTILS_BENDER_KIT_BUTTON_H_

#include <functional>
#include <string>
#include "../../app/src/main/jni/font.h"

#define FONT_SIZE_RATIO_X .05f
#define FONT_SIZE_RATIO_Y .05f

class Button {
 public:
  Button();

  Button(float x_center,
         float x_extent,
         float y_center,
         float y_extent,
         const char *text);

  bool testHit(int x, int y);
  void drawButton(VkRenderPass render_pass, Font *font, Renderer *renderer);

  static void setScreenResolution(VkExtent2D resolution) {
    screen_width_ = resolution.width;
    screen_height_ = resolution.height;
  }

  void onButtonDown(){
    pressed_ = true;
    onDown();
  }

  void onButtonHold(){
    pressed_ = true;
    onHold();
  }

  void onButtonUp(){
    pressed_ = false;
    onUp();
  }

  void update() { updater(*this); }

  void setLabel(std::string text);

  void setPosition(float x_center, float x_extent, float y_center, float y_extent);

  std::function<void()> onDown;
  std::function<void()> onUp;
  std::function<void()> onHold;

  std::function<void(Button&)> updater;

 private:
  float x_min_, y_min_, x_max_, y_max_, x_center_, y_center_;
  static inline int screen_width_, screen_height_;
  bool pressed_;
  std::string default_label_;
  const std::string pressed_label_ = "X";

};

#endif //BENDER_BASE_UTILS_BENDER_KIT_BUTTON_H_
