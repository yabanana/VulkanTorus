// BufferHelper.h
#ifndef BUFFER_HELPER_H
#define BUFFER_HELPER_H

#include <vulkan/vulkan.h>
#include <stdexcept>

namespace BufferHelper {
    void createBuffer(VkDevice device, VkPhysicalDevice physicalDevice, VkDeviceSize size, VkBufferUsageFlags usage,
                      VkMemoryPropertyFlags properties, VkBuffer &buffer, VkDeviceMemory &bufferMemory);
    
    void copyBuffer(VkDevice device, VkQueue graphicsQueue, VkCommandPool commandPool,
                    VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
}

#endif // BUFFER_HELPER_H
