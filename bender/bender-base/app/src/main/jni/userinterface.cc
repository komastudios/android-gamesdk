//
// Created by Matt Wang on 2019-12-09.
//

#include "userinterface.h"

std::vector<Button> UserInterface::buttons;

std::map<int, std::function<void(Data *, std::vector<Button> &)>> UserInterface::handlers = {
        {AMOTION_EVENT_ACTION_DOWN, UserInterface::actionDownHandler},
        {AMOTION_EVENT_ACTION_UP, UserInterface::actionUpHandler},
        {AMOTION_EVENT_ACTION_MOVE, UserInterface::actionMoveHandler},
};

UserInterface::UserInterface(Renderer *renderer, Font& font) : renderer_(renderer), font_(font) {}

uint32_t UserInterface::registerHandleForButton(float xCenter, float xExtent, float yCenter, float yExtent,
        const char *text) {
    Button button(xCenter, xExtent, yCenter, yExtent, text);
    buttons.push_back(button);
    return buttons.size() - 1;
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
    for (Button button : buttons) {
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

void UserInterface::setButtonOnUp(uint32_t handle, std::function<void()> func) {
    if (handle >= buttons.size()) { return; }
    buttons[handle].onUp = func;
}

void UserInterface::setButtonOnDown(uint32_t handle, std::function<void()> func) {
    if (handle >= buttons.size()) { return; }
    buttons[handle].onDown = func;
}

void UserInterface::setButtonOnHold(uint32_t handle, std::function<void()> func) {
    if (handle >= buttons.size()) { return; }
    buttons[handle].onHold = func;
}

int32_t UserInterface::handler(android_app *app, AInputEvent *event) {
    if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION) {
        Input::Data *inputData = (Input::Data *) app->userData;
        Input::updateInputData(event, inputData);

        if (inputData->lastButton != nullptr
            && !inputData->lastButton->testHit(inputData->lastX, inputData->lastY)) {
            inputData->lastButton->onButtonUp();
            inputData->lastButton = nullptr;
        }

        int action = AMotionEvent_getAction(event);
        if (handlers.find(action) != handlers.end()) {
            handlers[action](inputData, buttons);
        }
        return 1;
    }
    return 0;
}

void UserInterface::actionDownHandler(Input::Data *inputData, std::vector<Button> &buttons) {
    for (auto &button : buttons) {
        if (button.testHit(inputData->lastX, inputData->lastY)) {
            inputData->lastButton = &button;
            button.onButtonDown();
        }
    }
}

void UserInterface::actionMoveHandler(Input::Data *inputData, std::vector<Button> &buttons) {
    for (auto &button : buttons) {
        if (button.testHit(inputData->lastX, inputData->lastY)) {
            if (inputData->lastButton == nullptr) {
                button.onButtonDown();
            } else {
                button.onButtonHold();
            }
            inputData->lastButton = &button;
        }
    }
}

void UserInterface::actionUpHandler(Input::Data *inputData, std::vector<Button> &buttons) {
    for (auto &button : buttons) {
        if (button.testHit(inputData->lastX, inputData->lastY)) {
            button.onButtonUp();
        }
    }
    Input::clearInput(inputData);
}