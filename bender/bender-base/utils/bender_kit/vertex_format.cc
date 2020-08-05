//
// Created by fcarucci on 10/25/2019.
//

#include "vertex_format.h"

using namespace benderkit;

constexpr VkFormat VertexElementToFormat(VertexElement element) {
  switch (element) {
    case VertexElement::float1: return VK_FORMAT_R32_SFLOAT;
    case VertexElement::float2: return VK_FORMAT_R32G32_SFLOAT;
    case VertexElement::float3: return VK_FORMAT_R32G32B32_SFLOAT;
    case VertexElement::float4: return VK_FORMAT_R32G32B32A32_SFLOAT;
    case VertexElement::unorm1: return VK_FORMAT_R16_UNORM;
    case VertexElement::unorm2: return VK_FORMAT_R16G16_UNORM;
    case VertexElement::unorm3: return VK_FORMAT_R16G16B16_UNORM;
    case VertexElement::unorm4: return VK_FORMAT_R16G16B16A16_UNORM;
    case VertexElement::snorm1: return VK_FORMAT_R16_SNORM;
    case VertexElement::snorm2: return VK_FORMAT_R16G16_SNORM;
    case VertexElement::snorm3: return VK_FORMAT_R16G16B16_SNORM;
    case VertexElement::snorm4: return VK_FORMAT_R16G16B16A16_SNORM;
  }

  return VK_FORMAT_UNDEFINED;
}

constexpr uint32_t VertexElementToSize(VertexElement element) {
  switch (element) {
    case VertexElement::float1: return sizeof(float) * 1;
    case VertexElement::float2: return sizeof(float) * 2;
    case VertexElement::float3: return sizeof(float) * 3;
    case VertexElement::float4: return sizeof(float) * 4;
    case VertexElement::unorm1: return sizeof(uint16_t) * 1;
    case VertexElement::unorm2: return sizeof(uint16_t) * 2;
    case VertexElement::unorm3: return sizeof(uint16_t) * 3;
    case VertexElement::unorm4: return sizeof(uint16_t) * 4;
    case VertexElement::snorm1: return sizeof(uint16_t) * 1;
    case VertexElement::snorm2: return sizeof(uint16_t) * 2;
    case VertexElement::snorm3: return sizeof(uint16_t) * 3;
    case VertexElement::snorm4: return sizeof(uint16_t) * 4;

  }

  return 0;
}

VertexFormat::VertexFormat(std::initializer_list<std::initializer_list<VertexElement>> format) {
  uint32_t binding = 0;

  for (auto stream : format) {
    uint32_t offset = 0;
    uint32_t location = 0;

    for (auto element : stream) {
        attributes_.push_back({
          .binding = binding,
          .location = location,
          .format = VertexElementToFormat(element),
          .offset = offset,
          });

      offset += VertexElementToSize(element);

      ++location;
    }

    bindings_.push_back({
      .binding = binding,
      .stride = offset,
      .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,  // TODO: support different input rates for instancing
      });

    ++binding;
  }

  FillVertexInputState();
}

void VertexFormat::FillVertexInputState() {
  vertex_input_state_ = {
          .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
          .vertexBindingDescriptionCount = static_cast<uint32_t>(bindings_.size()),
          .pVertexBindingDescriptions = bindings_.size() == 0 ? nullptr : bindings_.data(),
          .vertexAttributeDescriptionCount = static_cast<uint32_t>(attributes_.size()),
          .pVertexAttributeDescriptions = attributes_.size() == 0 ? nullptr : attributes_.data()
  };
}