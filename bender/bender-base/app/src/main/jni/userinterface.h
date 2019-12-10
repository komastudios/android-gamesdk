//
// Created by Matt Wang on 2019-12-09.
//

#ifndef BENDER_BASE_USERINTERFACE_H
#define BENDER_BASE_USERINTERFACE_H

#include "button.h"
#include "input.h"
#include "constants.h"

using namespace Input;

class UserInterface {
public:
    struct TextField {
        std::function<std::string()> text_generator;
        float text_size;
        float x_corner;
        float y_corner;
    };

    UserInterface(Renderer *renderer, Font& font);

    void RegisterButton(float x_center, float x_extent, float y_center, float y_extent,
            const char *text, std::function<void(Button&)> func);

    void RegisterTextField(float text_size, float x_corner, float y_corner, std::function<std::string()> func);

    void DrawUserInterface(VkRenderPass render_pass) const;

    static int32_t Handler(android_app *app, AInputEvent *event);
    static void ActionDownHandler(Input::Data *input_data, std::vector<Button> &buttons);
    static void ActionMoveHandler(Input::Data *input_data, std::vector<Button> &buttons);
    static void ActionUpHandler(Input::Data *input_data, std::vector<Button> &buttons);

private:
    Renderer *renderer_;
    Font& font_;
    std::vector<TextField> text_fields_;

    static std::vector<Button> buttons_;
    static std::map<int, std::function<void(Data *, std::vector<Button> &)>> handlers_;
};

#endif //BENDER_BASE_USERINTERFACE_H
