//
// Created by Matt Wang on 2019-12-09.
//

#include "userinterface.h"

UserInterface::UserInterface(Renderer *renderer, Font& font) : renderer_(renderer), font_(font) {}

uint32_t UserInterface::registerHandleForButton(float xCenter, float xExtent, float yCenter, float yExtent,
        const char *text) const {
    Button button(xCenter, xExtent, yCenter, yExtent, text);
    Input::buttons.push_back(button);
    return Input::buttons.size() - 1;
}

uint32_t UserInterface::registerHandleForTextField(float textSize, float xCorner, float yCorner) {
    text_fields_.push_back({"", textSize, xCorner, yCorner});
    return text_fields_.size() - 1;
}

void UserInterface::updateTextField(uint32_t handle, const char *text) {
    assert(handle < text_fields_.size());
    text_fields_[handle].text = text;
}

void UserInterface::drawUserInterface(VkRenderPass renderPass) const {
    for (Button button : Input::buttons) {
        button.drawButton(renderPass, &font_, renderer_);
    }

    for (TextField textField : text_fields_) {
        font_.drawString(textField.text,
                         textField.textSize,
                         textField.xCorner,
                         textField.yCorner,
                         renderer_->getCurrentCommandBuffer(),
                         renderPass,
                         renderer_->getCurrentFrame());
    }
}