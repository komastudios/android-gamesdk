//
// Created by Matt Wang on 2019-12-06.
//

#ifndef BENDER_BASE_USERINTERFACE_H
#define BENDER_BASE_USERINTERFACE_H

#include "device.h"
#include "font.h"
#include "button.h"
#include "input.h"
#include "polyhedron.h"

using namespace BenderKit;

class UserInterface {
public:
    UserInterface(android_app& app, Device& device, Renderer& renderer, std::vector<Mesh *>& meshes,
            std::vector<std::shared_ptr<Material>>& materials, const std::vector<int>& polyFaces,
            float& frameTime, glm::vec3& cameraPosition, glm::quat& cameraRotation);

    ~UserInterface();

    void drawMeshInfo(VkRenderPass& renderPass, int triangles) const;

    void drawFPS(VkRenderPass& renderPass, int fps, float frameTime) const;

    void drawButtons(VkRenderPass& renderPass) const;

private:
    Device& device_;
    Renderer& renderer_;
    Font *font_;
    std::vector<Mesh *>& meshes_;
    std::vector<std::shared_ptr<Material>>& baseline_materials_;
    const std::vector<int>& allowed_poly_faces_;

    float& frame_time_;
    glm::vec3& camera_position_;
    glm::quat& camera_rotation_;

    uint32_t materials_idx_;
    uint32_t poly_faces_idx_;

    void createButtons();
};


#endif //BENDER_BASE_USERINTERFACE_H
