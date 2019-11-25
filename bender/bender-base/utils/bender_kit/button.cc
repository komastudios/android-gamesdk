//
// Created by theoking on 11/25/2019.
//

#include "button.h"

Button::Button(float x,
               float x_extent,
               float y,
               float y_extent,
               int screen_width,
               int screen_height,
               const char *text) {
  x_min = x - x_extent / 2;
  x_max = x + x_extent / 2;
  x_center = x;
  y_min = y - y_extent / 2;
  y_max = y + y_extent / 2;
  y_center = y;
  screen_width_ = screen_width;
  screen_height_ = screen_height;
  default_label = text;
  current_label = text;
  onUp = empty;
  onDown = empty;
  onHold = empty;
}

bool Button::testHit(int x, int y) {
  int half_width = screen_width_ / 2;
  int half_height = screen_height_ / 2;
  return x > (x_min * half_width) + half_width && x < (x_max * half_width) + half_width
      && y > (y_min * half_height) + half_height
      && y < (y_max * half_height) + half_height;
}

void Button::drawButton(VkRenderPass render_pass, Font *font, Renderer *renderer) {
  font->drawString(current_label,
                   1.25f,
                   x_center - (current_label.size() / 2 * FONT_SIZE_RATIO_X),
                   y_center - (FONT_SIZE_RATIO_Y),
                   renderer->getCurrentCommandBuffer(),
                   render_pass,
                   renderer->getCurrentFrame());
}