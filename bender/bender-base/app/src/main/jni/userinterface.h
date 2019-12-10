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
        float textSize;
        float xCorner;
        float yCorner;
    };

    UserInterface(Renderer *renderer, Font& font);

    uint32_t registerHandleForButton(float xCenter, float xExtent, float yCenter, float yExtent,
            const char *text);

    uint32_t registerHandleForTextField(float textSize, float xCorner, float yCorner);

    void updateTextField(uint32_t handle, const char *text);

    void drawUserInterface(VkRenderPass renderPass) const;

    void setButtonOnUp(uint32_t handle, std::function<void()> func);

    void setButtonOnDown(uint32_t handle, std::function<void()> func);

    void setButtonOnHold(uint32_t handle, std::function<void()> func);

    static int32_t handler(android_app *app, AInputEvent *event);
    static void actionDownHandler(Input::Data *inputData, std::vector<Button> &buttons);
    static void actionMoveHandler(Input::Data *inputData, std::vector<Button> &buttons);
    static void actionUpHandler(Input::Data *inputData, std::vector<Button> &buttons);

private:
    Renderer *renderer_;
    Font& font_;
    std::vector<TextField> text_fields_;

    static std::vector<Button> buttons;
    static std::map<int, std::function<void(Data *, std::vector<Button> &)>> handlers;
};

#endif //BENDER_BASE_USERINTERFACE_H
