//
// Created by theoking on 11/25/2019.
//

#ifndef BENDER_BASE_BUTTON_H_
#define BENDER_BASE_BUTTON_H_

#include "font.h"

#include <functional>
#include <string>

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

  bool TestHit(int x, int y);
  void DrawButton(VkRenderPass render_pass, Font *font, Renderer *renderer);

  static void SetScreenResolution(VkExtent2D resolution) {
    screen_width_ = resolution.width;
    screen_height_ = resolution.height;
  }

  void OnButtonDown(){
    pressed_ = true;
    on_down_();
  }

  void OnButtonHold(){
    on_hold_();
  }

  void OnButtonUp(){
    pressed_ = false;
    held_ = false;
    on_up_();
  }

  void StartHold() { held_ = true; }
  bool IsHeld() { return held_; }

  void Update() { updater_(*this); }

  void SetLabel(std::string text);

  void SetPosition(float x_center, float x_extent, float y_center, float y_extent);

  std::function<void()> on_down_;
  std::function<void()> on_up_;
  std::function<void()> on_hold_;

  std::function<void(Button&)> updater_;

 private:
  float x_min_, y_min_, x_max_, y_max_, x_center_, y_center_;
  static inline int screen_width_, screen_height_;
  bool pressed_, held_;
  std::string default_label_;
};

#endif //BENDER_BASE_BUTTON_H_
