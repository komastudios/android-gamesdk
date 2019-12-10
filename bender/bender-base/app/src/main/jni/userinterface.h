//
// Created by Matt Wang on 2019-12-09.
//

#ifndef BENDER_BASE_USERINTERFACE_H
#define BENDER_BASE_USERINTERFACE_H

#include "button.h"
#include "input.h"
#include "constants.h"

struct TextField {
    const char *text;
    float textSize;
    float xCorner;
    float yCorner;
};

class UserInterface {
public:
    UserInterface(Renderer *renderer, Font& font);

    uint32_t registerHandleForButton(float xCenter, float xExtent, float yCenter, float yExtent,
            const char *text) const;

    uint32_t registerHandleForTextField(float textSize, float xCorner, float yCorner);

    void updateTextField(uint32_t handle, const char *text);

    void drawUserInterface(VkRenderPass renderPass) const;

private:
    Renderer *renderer_;
    Font& font_;
    std::vector<TextField> text_fields_;
};

#endif //BENDER_BASE_USERINTERFACE_H
