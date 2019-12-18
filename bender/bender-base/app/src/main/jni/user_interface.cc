//
// Created by Matt Wang on 2019-12-09.
//

#include "user_interface.h"

std::vector<Button> UserInterface::buttons_;

std::map<int, std::function<void(Data *, std::vector<Button> &)>> UserInterface::handlers_ = {
        {AMOTION_EVENT_ACTION_DOWN, UserInterface::ActionDownHandler},
        {AMOTION_EVENT_ACTION_UP, UserInterface::ActionUpHandler},
        {AMOTION_EVENT_ACTION_MOVE, UserInterface::ActionMoveHandler},
};

UserInterface::UserInterface(Renderer *renderer, Font *font) : renderer_(renderer), font_(font) {}

void UserInterface::RegisterButton(std::function<void(Button&)> updater) {
    Button button;
    button.updater_ = updater;
    button.Update();
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
        button.Update();
        button.DrawButton(render_pass, font_, renderer_);
    }

    for (auto& text_field : text_fields_) {
        text_field.update();
        font_->DrawString(text_field.text,
                         text_field.text_size,
                         text_field.x_corner,
                         text_field.y_corner,
                         renderer_->GetCurrentCommandBuffer(),
                         render_pass,
                         renderer_->GetCurrentFrame());
    }
}

int32_t UserInterface::Handler(android_app *app, AInputEvent *event) {
    if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION) {
        input::Data *input_data = (input::Data *) app->userData;
        input::UpdateInputData(event, input_data);

        if (input_data->last_button != nullptr
            && !input_data->last_button->TestHit(input_data->last_x, input_data->last_y)) {
            input_data->last_button->OnButtonUp();
            input_data->last_button = nullptr;
        }

        int action = AMotionEvent_getAction(event);
        if (handlers_.find(action) != handlers_.end()) {
            handlers_[action](input_data, buttons_);
        }
        return 1;
    }
    return 0;
}

void UserInterface::ActionDownHandler(input::Data *input_data, std::vector<Button> &buttons) {
    for (auto &button : buttons) {
        if (button.TestHit(input_data->last_x, input_data->last_y)) {
            input_data->last_button = &button;
            button.OnButtonDown();
        }
    }
}

void UserInterface::ActionMoveHandler(input::Data *input_data, std::vector<Button> &buttons) {
    for (auto &button : buttons) {
        if (button.TestHit(input_data->last_x, input_data->last_y)) {
            if (input_data->last_button == nullptr) {
                button.OnButtonDown();
            } else {
                button.OnButtonHold();
            }
            input_data->last_button = &button;
        }
    }
}

void UserInterface::ActionUpHandler(input::Data *input_data, std::vector<Button> &buttons) {
    for (auto &button : buttons) {
        if (button.TestHit(input_data->last_x, input_data->last_y)) {
            button.OnButtonUp();
        }
    }
    input_data->last_button = nullptr;
    input::ClearInput(input_data);
}

void UserInterface::OnResume(Renderer *newRenderer, Font *font) {
  renderer_ = newRenderer;
  font_ = font;
}