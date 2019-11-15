//
// Created by fcarucci on 10/25/2019.
//

#include "vertex_format.h"

constexpr VkFormat vertexElementToFormat(VertexElement element) {
  switch (element) {
    case VertexElement::float1: return VK_FORMAT_R32_SFLOAT;
    case VertexElement::float2: return VK_FORMAT_R32G32_SFLOAT;
    case VertexElement::float3: return VK_FORMAT_R32G32B32_SFLOAT;
    case VertexElement::float4: return VK_FORMAT_R32G32B32A32_SFLOAT;
  }

  return VK_FORMAT_UNDEFINED;
}

constexpr uint32_t vertexElementToSize(VertexElement element) {
  switch (element) {
    case VertexElement::float1: return sizeof(float) * 1;
    case VertexElement::float2: return sizeof(float) * 2;
    case VertexElement::float3: return sizeof(float) * 3;
    case VertexElement::float4: return sizeof(float) * 4;
  }

  return 0;
}

VertexFormat::VertexFormat(std::initializer_list<std::initializer_list<VertexElement>> format) {
  uint32_t binding = 0;

  for (auto stream : format) {
    uint32_t offset = 0;
    uint32_t location = 0;

    for (auto element: stream) {
        attributes_.push_back({
          .binding = binding,
          .location = location,
          .format = vertexElementToFormat(element),
          .offset = offset,
          });

      offset += vertexElementToSize(element);

      ++location;
    }

    bindings_.push_back({
      .binding = binding,
      .stride = offset,
      .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,  // TODO: support different input rates for instancing
      });

    ++binding;
  }

  fillVertexInputState();
}

void VertexFormat::fillVertexInputState() {
  vertex_input_state_ = {
          .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
          .vertexBindingDescriptionCount = static_cast<u_int32_t>(bindings_.size()),
          .pVertexBindingDescriptions = bindings_.data(),
          .vertexAttributeDescriptionCount = static_cast<u_int32_t>(attributes_.size()),
          .pVertexAttributeDescriptions = attributes_.data()
  };
}