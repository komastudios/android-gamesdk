//
// Created by Matt Wang on 2019-12-06.
//

#include "userinterface.h"

UserInterface::UserInterface(android_app& app, Device& device, Renderer& renderer, std::vector<Mesh *>& meshes,
        std::vector<std::shared_ptr<Material>>& materials, const std::vector<int>& polyFaces,
        float& frameTime, glm::vec3& cameraPosition, glm::quat& cameraRotation) :
    device_(device),
    renderer_(renderer),
    meshes_(meshes),
    baseline_materials_(materials),
    allowed_poly_faces_(polyFaces),
    frame_time_(frameTime),
    camera_position_(cameraPosition),
    camera_rotation_(cameraRotation) {
    font_ = new Font(renderer, app, FONT_SDF_PATH, FONT_INFO_PATH);
    createButtons();
}

UserInterface::~UserInterface() {
    delete font_;
}

void UserInterface::drawMeshInfo(VkRenderPass& renderPass, int triangles) const {
    char output_string[50];
    const char *meshNoun = meshes_.size() == 1 ? "mesh" : "meshes";
    const char *polyNoun = "faces/polyhedron";
    const char *triangleNoun = triangles == 1 ? "triangle" : "triangles";

    sprintf(output_string, "%d %s, %d %s, %d %s", (int) meshes_.size(), meshNoun,
            allowed_poly_faces_[poly_faces_idx_], polyNoun, triangles, triangleNoun);
    font_->drawString(output_string,
                     1.0f,
                     -.98f,
                     -.98f,
                     renderer_.getCurrentCommandBuffer(),
                     renderPass,
                     renderer_.getCurrentFrame());
}

void UserInterface::drawFPS(VkRenderPass& renderPass, int fps, float frameTime) const {
    char fpsString[50];
    sprintf(fpsString, "%2.d FPS  %.3f ms", fps, frameTime);
    font_->drawString(fpsString,
                     1.0f,
                     -0.98f,
                     -0.88f,
                     renderer_.getCurrentCommandBuffer(),
                     renderPass,
                     renderer_.getCurrentFrame());
}

void UserInterface::drawButtons(VkRenderPass& renderPass) const {
    for (Button button : Input::buttons) {
        button.drawButton(renderPass, font_, &renderer_);
    }
}

void moveForward(float frameTime, glm::vec3& cameraPosition, glm::quat& cameraRotation) {
    glm::vec3 forward = glm::normalize(cameraRotation * glm::vec3(0.0f, 0.0f, -1.0f));
    cameraPosition += forward * 2.0f * frameTime;
}
void moveBackward(float frameTime, glm::vec3& cameraPosition, glm::quat& cameraRotation) {
    glm::vec3 forward = glm::normalize(cameraRotation * glm::vec3(0.0f, 0.0f, -1.0f));
    cameraPosition -= forward * 2.0f * frameTime;
}
void strafeLeft(uint32_t displayWidth, glm::vec3& cameraPosition, glm::quat& cameraRotation) {
    glm::vec3 right = glm::normalize(cameraRotation * glm::vec3(1.0f, 0.0f, 0.0f));
    cameraPosition -= right * (20.0f / displayWidth);
}
void strafeRight(uint32_t displayWidth, glm::vec3& cameraPosition, glm::quat& cameraRotation) {
    glm::vec3 right = glm::normalize(cameraRotation * glm::vec3(1.0f, 0.0f, 0.0f));
    cameraPosition += right * (20.0f / displayWidth);
}
void strafeUp(uint32_t displayHeight, glm::vec3& cameraPosition, glm::quat& cameraRotation) {
    glm::vec3 up = glm::normalize(cameraRotation * glm::vec3(0.0f, 1.0f, 0.0f));
    cameraPosition += up * (20.0f / displayHeight);
}
void strafeDown(uint32_t displayHeight, glm::vec3& cameraPosition, glm::quat& cameraRotation) {
    glm::vec3 up = glm::normalize(cameraRotation * glm::vec3(0.0f, 1.0f, 0.0f));
    cameraPosition -= up * (20.0f / displayHeight);
}
void createInstance(Renderer& renderer, std::vector<Mesh *>& meshes, std::shared_ptr<Material>& material, int numFaces) {
    meshes.push_back(createPolyhedron(renderer, material, numFaces));
    meshes[meshes.size() - 1]->translate(glm::vec3(rand() % 3, rand() % 3, rand() % 3));
}
void deleteInstance(std::vector<Mesh *>& meshes) {
    if (meshes.size() > 0) {
        meshes.pop_back();
    }
}
void changePolyhedralComplexity(std::vector<Mesh *>& meshes, const std::vector<int>& polyFaces,
        uint32_t& polyFacesIdx) {
    polyFacesIdx = (polyFacesIdx + 1) % polyFaces.size();
    for (uint32_t i = 0; i < meshes.size(); i++) {
        swapPolyhedron(*meshes[i], polyFaces[polyFacesIdx]);
    }
}
void changeMaterialComplexity(std::vector<Mesh *>& meshes, std::vector<std::shared_ptr<Material>>& materials,
        uint32_t& materialsIdx) {
    materialsIdx = (materialsIdx + 1) % materials.size();
    for (uint32_t i = 0; i < meshes.size(); i++) {
        meshes[i]->swapMaterial(materials[materialsIdx]);
    }
}

void UserInterface::createButtons() {
    Button::setScreenResolution(device_.getDisplaySizeOriented());

    Button button0(-.7, .2, .7, .2, "<--");
    button0.onHold = [this] {strafeLeft(device_.getDisplaySize().width, camera_position_, camera_rotation_);};
    Button button1(-.2, .2, .7, .2, "-->");
    button1.onHold = [this] {strafeRight(device_.getDisplaySize().width, camera_position_, camera_rotation_);};
    Button button2(-.47, .2, .6, .2, "^");
    button2.onHold = [this] {strafeUp(device_.getDisplaySize().height, camera_position_, camera_rotation_);};
    Button button3(-.47, .2, .85, .2, "O");
    button3.onHold = [this] {strafeDown(device_.getDisplaySize().width, camera_position_, camera_rotation_);};
    Button button4(.43, .2, .65, .2, "Forward");
    button4.onHold = [this] {moveForward(frame_time_, camera_position_, camera_rotation_);};
    Button button5(.43, .2, .85, .2, "Backward");
    button5.onHold = [this] {moveBackward(frame_time_, camera_position_, camera_rotation_);};
    Button button6(-.2, .2, .4, .2, "+1 Mesh");
    button6.onUp = [this] {createInstance(renderer_, meshes_, baseline_materials_[materials_idx_], allowed_poly_faces_[poly_faces_idx_]);};
    Button button7(-.7, .2, .4, .2, "-1 Mesh");
    button7.onUp = [this] {deleteInstance(meshes_);};
    Button button8(.5, .2, .2, .2, "Poly Switch");
    button8.onUp = [this] {changePolyhedralComplexity(meshes_, allowed_poly_faces_, poly_faces_idx_);};
    Button button9(.5, .2, .4, .2, "Tex Switch");
    button9.onUp = [this] {changeMaterialComplexity(meshes_, baseline_materials_, materials_idx_);};

    Input::buttons.push_back(button0);
    Input::buttons.push_back(button1);
    Input::buttons.push_back(button2);
    Input::buttons.push_back(button3);
    Input::buttons.push_back(button4);
    Input::buttons.push_back(button5);
    Input::buttons.push_back(button6);
    Input::buttons.push_back(button7);
    Input::buttons.push_back(button8);
    Input::buttons.push_back(button9);
}
