//
// Created by fcarucci on 10/25/2019.
//

#ifndef BENDER_VERTEX_FORMAT_H_
#define BENDER_VERTEX_FORMAT_H_

#include "vulkan_wrapper.h"
#include "bender_kit.h"

namespace BenderKit {
    enum class VertexElement { float1, float2, float3, float4 };

    class VertexFormat {
    public:
        VertexFormat(std::initializer_list<std::initializer_list<VertexElement>> format);

        VertexFormat(VertexFormat&& other)
                : bindings_(std::move(other.bindings_))
                , attributes_(std::move(other.attributes_)) {
          fillVertexInputState();
        }

        VertexFormat(const VertexFormat& other) {
          bindings_ = other.bindings_;
          attributes_ = other.attributes_;

          fillVertexInputState();
        }

        const VkPipelineVertexInputStateCreateInfo& getVertexInputState() { return vertex_input_state_; }

    private:
        VkPipelineVertexInputStateCreateInfo vertex_input_state_;

        std::vector<VkVertexInputBindingDescription> bindings_;
        std::vector<VkVertexInputAttributeDescription> attributes_;

        void fillVertexInputState();
    };
}

#endif // BENDER_VERTEX_FORMAT_H_
