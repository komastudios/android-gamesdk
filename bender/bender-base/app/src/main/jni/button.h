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
    current_label = "X";
    onDown();
  }

  void onButtonHold(){
    onHold();
  }

  void onButtonUp(){
    current_label = default_label;
    onUp();
  }

  void setLabel(std::string text) {
    default_label = text;
    current_label = default_label;
  }

  std::function<void()> onDown;
  std::function<void()> onUp;
  std::function<void()> onHold;

 private:
  float x_min, y_min, x_max, y_max, x_center, y_center;
  static inline int screen_width_, screen_height_;
  std::string default_label;
  std::string current_label;
  static void empty(){ }

};

#endif //BENDER_BASE_UTILS_BENDER_KIT_BUTTON_H_
