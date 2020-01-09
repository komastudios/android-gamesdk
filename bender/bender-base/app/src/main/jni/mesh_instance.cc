//
// Created by Matt Wang on 2020-01-08.
//

#include "mesh_instance.h"
#include "shader_bindings.h"

MeshInstance::MeshInstance(Renderer *renderer, Mesh &mesh, const std::shared_ptr<Material> material) :
    renderer_(renderer), mesh_(mesh), material_(material) {
    position_ = glm::vec3(0.0f, 0.0f, 0.0f);
    rotation_ = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    scale_ = glm::vec3(1.0f, 1.0f, 1.0f);

    mesh_buffer_ = std::make_unique<UniformBufferObject<ModelViewProjection>>(renderer_->GetDevice());
    CreateMeshDescriptorSetLayout();
    CreateMeshDescriptors();
}

MeshInstance::MeshInstance(const MeshInstance &other, const std::shared_ptr<Material> material) :
    renderer_(other.renderer_),
    mesh_(other.mesh_),
    material_(material),
    position_(other.position_),
    rotation_(other.rotation_),
    scale_(other.scale_) {
    CreateMeshDescriptorSetLayout();
    CreateMeshDescriptors();
}

MeshInstance::~MeshInstance() {
    Cleanup();
}

void MeshInstance::Cleanup() {
    mesh_buffer_.reset();
    vkDestroyDescriptorSetLayout(renderer_->GetVulkanDevice(), mesh_descriptors_layout_, nullptr);
}

void MeshInstance::OnResume(Renderer *renderer) {
    mesh_buffer_ = std::make_unique<UniformBufferObject<ModelViewProjection>>(renderer_->GetDevice());
    CreateMeshDescriptorSetLayout();
    CreateMeshDescriptors();
}

void MeshInstance::UpdatePipeline(VkRenderPass render_pass) {
    mesh_.UpdatePipeline(render_pass, material_);
}

void MeshInstance::CreateMeshDescriptors() {
    std::vector<VkDescriptorSetLayout> layouts(renderer_->GetDevice().GetDisplayImages().size(),
                                               mesh_descriptors_layout_);

    VkDescriptorSetAllocateInfo alloc_info = {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
            .descriptorPool = renderer_->GetDescriptorPool(),
            .descriptorSetCount = static_cast<uint32_t>(renderer_->GetDevice().GetDisplayImages().size()),
            .pSetLayouts = layouts.data(),
    };

    mesh_descriptor_sets_.resize(renderer_->GetDevice().GetDisplayImages().size());
    CALL_VK(vkAllocateDescriptorSets(renderer_->GetVulkanDevice(),
                                     &alloc_info,
                                     mesh_descriptor_sets_.data()));

    for (size_t i = 0; i < renderer_->GetDevice().GetDisplayImages().size(); i++) {
        VkDescriptorBufferInfo buffer_info = {
                .buffer = mesh_buffer_->GetBuffer(i),
                .offset = 0,
                .range = sizeof(ModelViewProjection),
        };

        std::array<VkWriteDescriptorSet, 1> descriptor_writes = {};

        descriptor_writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptor_writes[0].dstSet = mesh_descriptor_sets_[i];
        descriptor_writes[0].dstBinding = VERTEX_BINDING_MODEL_VIEW_PROJECTION;
        descriptor_writes[0].dstArrayElement = 0;
        descriptor_writes[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptor_writes[0].descriptorCount = 1;
        descriptor_writes[0].pBufferInfo = &buffer_info;

        vkUpdateDescriptorSets(renderer_->GetVulkanDevice(),
                               descriptor_writes.size(),
                               descriptor_writes.data(),
                               0,
                               nullptr);

    }
}

void MeshInstance::CreateMeshDescriptorSetLayout() {
    VkDescriptorSetLayoutBinding ubo_layout_binding = {
            .binding = VERTEX_BINDING_MODEL_VIEW_PROJECTION,
            .descriptorCount = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .pImmutableSamplers = nullptr,
            .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
    };

    std::array<VkDescriptorSetLayoutBinding, 1> bindings = {ubo_layout_binding};
    VkDescriptorSetLayoutCreateInfo layout_info = {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
            .bindingCount = bindings.size(),
            .pBindings = bindings.data(),
    };

    CALL_VK(vkCreateDescriptorSetLayout(renderer_->GetVulkanDevice(), &layout_info, nullptr,
                                        &mesh_descriptors_layout_));
}

void MeshInstance::Update(uint_t frame_index, glm::vec3 camera, glm::mat4 view, glm::mat4 proj) {
    glm::mat4 model = GetTransform();
    glm::mat4 mvp = proj * view * model;

    mesh_buffer_->Update(frame_index, [&mvp, &model](auto &ubo) {
        ubo.mvp = mvp;
        ubo.model = model;
        ubo.inv_transpose = glm::transpose(glm::inverse(model));
    });
}

void MeshInstance::SubmitDraw(VkCommandBuffer cmd_buffer, uint_t frame_index) const {
    mesh_.SubmitDraw(cmd_buffer, frame_index, );
}

void MeshInstance::Translate(glm::vec3 offset) {
    position_ += offset;
}

void MeshInstance::Rotate(glm::vec3 axis, float angle) {
    rotation_ *= glm::angleAxis(glm::radians(angle), axis);
    rotation_ = glm::normalize(rotation_);
}

void MeshInstance::Scale(glm::vec3 scaling) {
    scale_ *= scaling;
}

void MeshInstance::SetPosition(glm::vec3 position) {
    position_ = position;
}

void MeshInstance::SetRotation(glm::vec3 axis, float angle) {
    rotation_ = glm::angleAxis(glm::radians(angle), axis);
}

void MeshInstance::SetScale(glm::vec3 scale) {
    scale_ = scale;
}

glm::vec3 MeshInstance::GetPosition() const {
    return position_;
}

glm::quat MeshInstance::GetRotation() const {
    return rotation_;
}

glm::vec3 MeshInstance::GetScale() const {
    return scale_;
}

glm::mat4 MeshInstance::GetTransform() const {
    glm::mat4 position = glm::translate(glm::mat4(1.0), position_);
    glm::mat4 scale = glm::scale(glm::mat4(1.0), scale_);
    return position * glm::mat4(rotation_) * scale;
}

BoundingBox MeshInstance::GetBoundingBoxWorldSpace() const {
    glm::mat4 finalTransform = GetTransform();
    BoundingBox originalBox = mesh_.GetBoundingBox();

    glm::vec3 xMin = finalTransform[0] * (originalBox.min.x);
    glm::vec3 xMax = finalTransform[0] * (originalBox.max.x);

    glm::vec3 yMin = finalTransform[1] * (originalBox.min.y);
    glm::vec3 yMax = finalTransform[1] * (originalBox.max.y);

    glm::vec3 zMin = finalTransform[2] * (originalBox.min.z);
    glm::vec3 zMax = finalTransform[2] * (originalBox.max.z);

    return BoundingBox{
            .min = glm::min(xMin, xMax) + glm::min(yMin ,yMax) + glm::min(zMin, zMax) + glm::vec3(finalTransform[3]),
            .max = glm::max(xMin, xMax) + glm::max(yMin ,yMax) + glm::max(zMin, zMax) + glm::vec3(finalTransform[3]),
    };
}
