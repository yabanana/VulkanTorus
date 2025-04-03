// DepthResources.cpp
#include "DepthResources.h"
#include <stdexcept>

DepthResources::DepthResources(VkDevice device, VkPhysicalDevice physicalDevice, uint32_t width, uint32_t height)
    : device(device), physicalDevice(physicalDevice), width(width), height(height),
      depthImage(VK_NULL_HANDLE), depthImageMemory(VK_NULL_HANDLE), depthImageView(VK_NULL_HANDLE)
{
}

DepthResources::~DepthResources()
{
    vkDestroyImageView(device, depthImageView, nullptr);
    vkDestroyImage(device, depthImage, nullptr);
    vkFreeMemory(device, depthImageMemory, nullptr);
}

void DepthResources::create()
{
    depthFormat = findDepthFormat();
    createImage(width, height, depthFormat, VK_IMAGE_TILING_OPTIMAL,
                VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                depthImage, depthImageMemory);
    
    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = depthImage;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = depthFormat;
    viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;
    
    if (vkCreateImageView(device, &viewInfo, nullptr, &depthImageView) != VK_SUCCESS)
        throw std::runtime_error("failed to create depth image view!");
}

VkFormat DepthResources::findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features)
{
    for (VkFormat format : candidates) {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &props);
        if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)
            return format;
        else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features)
            return format;
    }
    throw std::runtime_error("failed to find supported format!");
}

VkFormat DepthResources::findDepthFormat()
{
    return findSupportedFormat(
        { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
        VK_IMAGE_TILING_OPTIMAL,
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
    );
}

void DepthResources::createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling,
                                 VkImageUsageFlags usage, VkMemoryPropertyFlags properties,
                                 VkImage& image, VkDeviceMemory& imageMemory)
{
    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = width;
    imageInfo.extent.height = height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.format = format;
    imageInfo.tiling = tiling;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = usage;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    
    if (vkCreateImage(device, &imageInfo, nullptr, &image) != VK_SUCCESS)
        throw std::runtime_error("failed to create image!");
    
    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(device, image, &memRequirements);
    
    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);
    uint32_t memoryTypeIndex = 0;
    bool found = false;
    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((memRequirements.memoryTypeBits & (1 << i)) &&
            (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            memoryTypeIndex = i;
            found = true;
            break;
        }
    }
    if (!found)
        throw std::runtime_error("failed to find suitable memory type!");
    
    allocInfo.memoryTypeIndex = memoryTypeIndex;
    
    if (vkAllocateMemory(device, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS)
        throw std::runtime_error("failed to allocate image memory!");
    
    vkBindImageMemory(device, image, imageMemory, 0);
}
