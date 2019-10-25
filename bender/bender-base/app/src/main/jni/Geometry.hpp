//
// Created by mattkw on 10/25/2019.
//

#ifndef BENDER_BASE_GEOMETRY_HPP
#define BENDER_BASE_GEOMETRY_HPP

#include "vulkan_wrapper.h"
#include "bender_kit.hpp"

class Geometry {
public:
    Geometry(BenderKit::Device *device);

    int getVertexCount() { return vertexCount_; }
    int getIndexCount() { return indexCount_; }
    VkBuffer* getVertexBuffer() { return &vertexBuf_; }
    VkBuffer* getIndexBuffer() { return &indexBuf_; }

    void cleanup();

private:
    BenderKit::Device *device_;

    std::vector<float> vertexData;
    std::vector<uint16_t> indexData;

    int vertexCount_;
    VkBuffer vertexBuf_;
    VkDeviceMemory vertexBufferDeviceMemory_;

    int indexCount_;
    VkBuffer indexBuf_;
    VkDeviceMemory indexBufferDeviceMemory_;

    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties,
                                VkPhysicalDevice gpuDevice);

    void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties,
                                VkBuffer &buffer, VkDeviceMemory &bufferMemory);

    void createVertexBuffer(void);
};

#endif //BENDER_BASE_GEOMETRY_HPP
