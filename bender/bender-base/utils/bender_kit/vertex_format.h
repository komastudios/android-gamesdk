//
// Created by fcarucci on 10/25/2019.
//

#ifndef BENDER_VERTEX_FORMAT_H_
#define BENDER_VERTEX_FORMAT_H_

#include "vulkan_wrapper.h"
#include "bender_kit.h"

#include <vector>

namespace benderkit {
    enum class VertexElement { float1, float2, float3, float4 };

    class VertexFormat {
    public:
        VertexFormat(std::initializer_list<std::initializer_list<VertexElement>> format,
                     std::initializer_list<VkVertexInputRate> input_rates);

        VertexFormat(VertexFormat&& other)
                : bindings_(std::move(other.bindings_))
                , attributes_(std::move(other.attributes_)) {
          FillVertexInputState();
        }

        VertexFormat(const VertexFormat& other) {
          bindings_ = other.bindings_;
          attributes_ = other.attributes_;

          FillVertexInputState();
        }

        const VkPipelineVertexInputStateCreateInfo& GetVertexInputState() { return vertex_input_state_; }

    private:
        VkPipelineVertexInputStateCreateInfo vertex_input_state_;

        std::vector<VkVertexInputBindingDescription> bindings_;
        std::vector<VkVertexInputAttributeDescription> attributes_;

        void FillVertexInputState();
    };
}

#endif // BENDER_VERTEX_FORMAT_H_
