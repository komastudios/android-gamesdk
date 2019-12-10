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
        const char *text;
        float text_size;
        float x_corner;
        float y_corner;
    };

    UserInterface(Renderer *renderer, Font& font);

    uint32_t RegisterHandleForButton(float x_center, float x_extent, float y_center, float y_extent,
            const char *text);

    uint32_t RegisterHandleForTextField(float text_size, float x_corner, float y_corner);

    void UpdateTextField(uint32_t handle, const char *text);

    void DrawUserInterface(VkRenderPass render_pass) const;

    void SetButtonOnUp(uint32_t handle, std::function<void()> func);

    void SetButtonOnDown(uint32_t handle, std::function<void()> func);

    void SetButtonOnHold(uint32_t handle, std::function<void()> func);

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
