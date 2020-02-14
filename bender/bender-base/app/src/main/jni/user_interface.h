//
// Created by Matt Wang on 2019-12-09.
//

#ifndef BENDER_BASE_USERINTERFACE_H
#define BENDER_BASE_USERINTERFACE_H

#include "button.h"
#include "input.h"
#include "constants.h"

using namespace input;

struct TextField {
    std::function<void(TextField&)> updater = [] (TextField&) {};
    std::string text;
    float text_size;
    float x_corner;
    float y_corner;

    void update() { updater(*this); }
};

class UserInterface {
public:

    UserInterface(Renderer *renderer, Font *font);

    void RegisterButton(std::function<void(Button&)> updater);

    void RegisterTextField(std::function<void(TextField&)> updater);

    void DrawUserInterface(VkRenderPass render_pass);

    static int32_t Handler(android_app *app, AInputEvent *event);
    static void ActionDownHandler(input::Data *input_data, std::vector<Button> &buttons);
    static void ActionMoveHandler(input::Data *input_data, std::vector<Button> &buttons);
    static void ActionUpHandler(input::Data *input_data, std::vector<Button> &buttons);

    void RunHeldButtons();

    void OnResume(Renderer *new_renderer, Font *font);

private:
    Renderer *renderer_;
    Font *font_;
    std::vector<TextField> text_fields_;

    static std::vector<Button> buttons_;
    static std::map<int, std::function<void(Data *, std::vector<Button> &)>> handlers_;
};

#endif //BENDER_BASE_USERINTERFACE_H
