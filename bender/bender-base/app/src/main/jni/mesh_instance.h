//
// Created by Matt Wang on 2020-01-09.
//

#ifndef BENDER_BASE_MESH_INSTANCE_H
#define BENDER_BASE_MESH_INSTANCE_H

#include "mesh.h"
#include "material.h"
#include "uniform_buffer.h"
#include "vulkan_wrapper.h"
#include "glm/glm.hpp"

struct ModelViewProjection {
    alignas(16) glm::mat4 mvp;
    alignas(16) glm::mat4 model;
    alignas(16) glm::mat4 inv_transpose;
};

class MeshInstance {
public:
    MeshInstance(Renderer *renderer, Mesh& mesh, const std::shared_ptr<Material> material);

    MeshInstance(const MeshInstance &other, const std::shared_ptr<Material> material);

    MeshInstance(const MeshInstance &other, Mesh& mesh);

    ~MeshInstance();

    void Cleanup();

    void OnResume(Renderer *renderer);

    void UpdatePipeline(VkRenderPass render_pass) { mesh_.UpdatePipeline(render_pass, material_); }

    void Translate(glm::vec3 offset);
    void Rotate(glm::vec3 axis, float angle);
    void Scale(glm::vec3 scaling);

    void SetPosition(glm::vec3 position);
    void SetRotation(glm::vec3 axis, float angle);
    void SetScale(glm::vec3 scale);

    glm::vec3 GetPosition() const;
    glm::quat GetRotation() const;
    glm::vec3 GetScale() const;
    glm::mat4 GetTransform() const;

    BoundingBox GetBoundingBoxWorldSpace() const;
    int GetTrianglesCount() const { return mesh_.GetTrianglesCount(); }

    // TODO: consolidate camera, view, proj into a camera object
    void UpdateMVP(uint_t frame_index, glm::vec3 camera, glm::mat4 view, glm::mat4 proj);

    void SubmitDraw(uint_t frame_index) const;

private:
    Renderer *renderer_;
    Mesh &mesh_;
    const std::shared_ptr<Material> material_;
    std::unique_ptr<UniformBufferObject<ModelViewProjection>> mesh_buffer_;

    glm::vec3 position_;
    glm::quat rotation_;
    glm::vec3 scale_;

    std::vector<VkDescriptorSet> mesh_descriptor_sets_;

    void CreateMeshDescriptors();
};

#endif //BENDER_BASE_MESH_INSTANCE_H
