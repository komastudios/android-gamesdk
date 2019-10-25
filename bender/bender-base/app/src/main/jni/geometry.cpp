//
// Created by mattkw on 10/25/2019.
//

#include "geometry.hpp"
#include <vector>

Geometry::Geometry(BenderKit::Device* device, std::vector<float> vertexData, std::vector<uint16_t> indexData) {
    device_ = device;

    createVertexBuffer(vertexData, indexData);
}

Geometry::~Geometry() {
    vkDestroyBuffer(device_->getDevice(), vertexBuf_, nullptr);
    vkFreeMemory(device_->getDevice(), vertexBufferDeviceMemory_, nullptr);
    vkDestroyBuffer(device_->getDevice(), indexBuf_, nullptr);
    vkFreeMemory(device_->getDevice(), indexBufferDeviceMemory_, nullptr);
}

// A helper function for selecting the correct memory type for a buffer
uint32_t Geometry::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties,
        VkPhysicalDevice gpuDevice) {
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(gpuDevice, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) &&
            (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }

    LOGE("failed to find suitable memory type!");
    return -1;
}

// Generalized helper function for creating different types of buffers
void Geometry::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties,
                  VkBuffer &buffer, VkDeviceMemory &bufferMemory) {
    VkBufferCreateInfo bufferInfo = {
            .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
            .pNext = nullptr,
            .size = size,
            .usage = usage,
            .flags = 0,
            .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
            .queueFamilyIndexCount = 1,
    };

    CALL_VK(vkCreateBuffer(device_->getDevice(), &bufferInfo, nullptr, &buffer));

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device_->getDevice(), buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo = {
            .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
            .pNext = nullptr,
            .allocationSize = memRequirements.size,
            .memoryTypeIndex = Geometry::findMemoryType(memRequirements.memoryTypeBits,
                                              properties, device_->getPhysicalDevice())
    };

    // Allocate memory for the buffer
    CALL_VK(vkAllocateMemory(device_->getDevice(), &allocInfo, nullptr, &bufferMemory));
    CALL_VK(vkBindBufferMemory(device_->getDevice(), buffer, bufferMemory, 0));
}

// Create buffers for vertex data
void Geometry::createVertexBuffer(std::vector<float> vertexData, std::vector<uint16_t> indexData) {
    vertexCount_ = vertexData.size();
    indexCount_ = indexData.size();

    VkDeviceSize bufferSizeVertex = sizeof(vertexData[0]) * vertexData.size();
    createBuffer(bufferSizeVertex, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                 vertexBuf_, vertexBufferDeviceMemory_);

    VkDeviceSize bufferSizeIndex = sizeof(indexData[0]) * indexData.size();
    createBuffer(bufferSizeIndex, VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                 indexBuf_, indexBufferDeviceMemory_);


    void* data;
    vkMapMemory(device_->getDevice(), vertexBufferDeviceMemory_, 0, bufferSizeVertex, 0, &data);
    memcpy(data, vertexData.data(), bufferSizeVertex);
    vkUnmapMemory(device_->getDevice(), vertexBufferDeviceMemory_);

    vkMapMemory(device_->getDevice(), indexBufferDeviceMemory_, 0, bufferSizeIndex, 0, &data);
    memcpy(data, indexData.data(), bufferSizeIndex);
    vkUnmapMemory(device_->getDevice(), indexBufferDeviceMemory_);
}
