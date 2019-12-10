//
// Created by Matt Wang on 2019-12-09.
//

#include "userinterface.h"

std::vector<Button> UserInterface::buttons_;

std::map<int, std::function<void(Data *, std::vector<Button> &)>> UserInterface::handlers_ = {
        {AMOTION_EVENT_ACTION_DOWN, UserInterface::ActionDownHandler},
        {AMOTION_EVENT_ACTION_UP, UserInterface::ActionUpHandler},
        {AMOTION_EVENT_ACTION_MOVE, UserInterface::ActionMoveHandler},
};

UserInterface::UserInterface(Renderer *renderer, Font& font) : renderer_(renderer), font_(font) {}

uint32_t UserInterface::RegisterHandleForButton(float x_center, float x_extent, float y_center, float y_extent,
        const char *text) {
    buttons_.push_back(Button(x_center, x_extent, y_center, y_extent, text));
    return buttons_.size() - 1;
}

uint32_t UserInterface::RegisterHandleForTextField(float text_size, float x_corner, float y_corner) {
    text_fields_.push_back({"", text_size, x_corner, y_corner});
    return text_fields_.size() - 1;
}

void UserInterface::UpdateTextField(uint32_t handle, const char *text) {
    assert(handle < text_fields_.size());
    text_fields_[handle].text = text;
}

void UserInterface::DrawUserInterface(VkRenderPass render_pass) const {
    for (Button button : buttons_) {
        button.drawButton(render_pass, &font_, renderer_);
    }

    for (TextField text_field : text_fields_) {
        font_.drawString(text_field.text,
                         text_field.text_size,
                         text_field.x_corner,
                         text_field.y_corner,
                         renderer_->getCurrentCommandBuffer(),
                         render_pass,
                         renderer_->getCurrentFrame());
    }
}

void UserInterface::SetButtonOnUp(uint32_t handle, std::function<void()> func) {
    if (handle >= buttons_.size()) { return; }
    buttons_[handle].onUp = func;
}

void UserInterface::SetButtonOnDown(uint32_t handle, std::function<void()> func) {
    if (handle >= buttons_.size()) { return; }
    buttons_[handle].onDown = func;
}

void UserInterface::SetButtonOnHold(uint32_t handle, std::function<void()> func) {
    if (handle >= buttons_.size()) { return; }
    buttons_[handle].onHold = func;
}

int32_t UserInterface::Handler(android_app *app, AInputEvent *event) {
    if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION) {
        Input::Data *input_data = (Input::Data *) app->userData;
        Input::updateInputData(event, input_data);

        if (input_data->lastButton != nullptr
            && !input_data->lastButton->testHit(input_data->lastX, input_data->lastY)) {
            input_data->lastButton->onButtonUp();
            input_data->lastButton = nullptr;
        }

        int action = AMotionEvent_getAction(event);
        if (handlers_.find(action) != handlers_.end()) {
            handlers_[action](input_data, buttons_);
        }
        return 1;
    }
    return 0;
}

void UserInterface::ActionDownHandler(Input::Data *input_data, std::vector<Button> &buttons) {
    for (auto &button : buttons) {
        if (button.testHit(input_data->lastX, input_data->lastY)) {
            input_data->lastButton = &button;
            button.onButtonDown();
        }
    }
}

void UserInterface::ActionMoveHandler(Input::Data *input_data, std::vector<Button> &buttons) {
    for (auto &button : buttons) {
        if (button.testHit(input_data->lastX, input_data->lastY)) {
            if (input_data->lastButton == nullptr) {
                button.onButtonDown();
            } else {
                button.onButtonHold();
            }
            input_data->lastButton = &button;
        }
    }
}

void UserInterface::ActionUpHandler(Input::Data *input_data, std::vector<Button> &buttons) {
    for (auto &button : buttons) {
        if (button.testHit(input_data->lastX, input_data->lastY)) {
            button.onButtonUp();
        }
    }
    Input::clearInput(input_data);
}