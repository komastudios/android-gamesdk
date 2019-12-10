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

UserInterface::UserInterface(Renderer *renderer, Font *font) : renderer_(renderer), font_(font) {}

void UserInterface::RegisterButton(std::function<void(Button&)> updater) {
    Button button;
    button.updater = updater;
    button.update();
    buttons_.push_back(button);
}

void UserInterface::RegisterTextField(std::function<void(TextField&)> updater) {
    TextField field;
    field.updater = updater;
    field.update();
    text_fields_.push_back(field);
}

void UserInterface::DrawUserInterface(VkRenderPass render_pass) {
    for (auto& button : buttons_) {
        button.update();
        button.drawButton(render_pass, font_, renderer_);
    }

    for (auto& text_field : text_fields_) {
        text_field.update();
        font_->drawString(text_field.text,
                         text_field.text_size,
                         text_field.x_corner,
                         text_field.y_corner,
                         renderer_->getCurrentCommandBuffer(),
                         render_pass,
                         renderer_->getCurrentFrame());
    }
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
    input_data->lastButton = nullptr;
    Input::clearInput(input_data);
}

void UserInterface::OnResume(Renderer *newRenderer, Font *font) {
  renderer_ = newRenderer;
  font_ = font;
}