// DepthResources.h
#ifndef DEPTH_RESOURCES_H
#define DEPTH_RESOURCES_H

#include <vulkan/vulkan.h>
#include <vector>

class DepthResources {
public:
    DepthResources(VkDevice device, VkPhysicalDevice physicalDevice, uint32_t width, uint32_t height);
    ~DepthResources();

    void create();
    VkImageView getImageView() const { return depthImageView; }
    VkFormat getFormat() const { return depthFormat; }

private:
    VkDevice device;
    VkPhysicalDevice physicalDevice;
    uint32_t width, height;
    VkImage depthImage;
    VkDeviceMemory depthImageMemory;
    VkImageView depthImageView;
    VkFormat depthFormat;

    VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
    VkFormat findDepthFormat();
    void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling,
                     VkImageUsageFlags usage, VkMemoryPropertyFlags properties,
                     VkImage& image, VkDeviceMemory& imageMemory);
};

#endif // DEPTH_RESOURCES_H
