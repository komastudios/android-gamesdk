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

void UserInterface::RegisterButton(float x_center, float x_extent, float y_center, float y_extent,
        std::function<void(Button&)> initializer) {
    Button button = Button(x_center, x_extent, y_center, y_extent, "");
    initializer(button);
    buttons_.push_back(button);
}

void UserInterface::RegisterTextField(std::string text, float text_size, float x_corner, float y_corner,
        std::function<void(TextField&)> updater) {
    TextField field = {updater, text, text_size, x_corner, y_corner};
    text_fields_.push_back(field);
}

void UserInterface::DrawUserInterface(VkRenderPass render_pass) const {
    for (Button button : buttons_) {
        button.drawButton(render_pass, &font_, renderer_);
    }

    for (TextField text_field : text_fields_) {
        text_field.updater(text_field);
        font_.drawString(text_field.text,
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
    Input::clearInput(input_data);
}