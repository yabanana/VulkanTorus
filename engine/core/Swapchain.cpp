// Swapchain.cpp
#include "Swapchain.h"
#include <stdexcept>
#include <algorithm>
#include <array>
#include <vulkan/vulkan.h>

Swapchain::Swapchain(VkPhysicalDevice physicalDevice, VkDevice device, VkSurfaceKHR surface, uint32_t width, uint32_t height)
    : physicalDevice(physicalDevice), device(device), surface(surface), width(width), height(height),
      swapChain(VK_NULL_HANDLE)
{
}

Swapchain::~Swapchain()
{
    cleanup();
}

void Swapchain::create()
{
    VkSurfaceCapabilitiesKHR capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &capabilities);
    uint32_t minImageCount = capabilities.minImageCount;
    uint32_t desiredImageCount = std::max(2u, minImageCount);

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = surface;
    createInfo.minImageCount = desiredImageCount;
    createInfo.imageFormat = VK_FORMAT_B8G8R8A8_SRGB;
    createInfo.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
    createInfo.imageExtent = { width, height };
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VK_NULL_HANDLE;

    if (vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapChain) != VK_SUCCESS)
        throw std::runtime_error("failed to create swap chain!");

    uint32_t imageCount;
    vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);
    images.resize(imageCount);
    vkGetSwapchainImagesKHR(device, swapChain, &imageCount, images.data());

    imageFormat = createInfo.imageFormat;
    extent = createInfo.imageExtent;

    createImageViews();
}

void Swapchain::createImageViews()
{
    imageViews.resize(images.size());
    for (size_t i = 0; i < images.size(); i++) {
        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = images[i];
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = imageFormat;
        viewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;

        if (vkCreateImageView(device, &viewInfo, nullptr, &imageViews[i]) != VK_SUCCESS)
            throw std::runtime_error("failed to create image views!");
    }
}

void Swapchain::createFramebuffers(VkRenderPass renderPass, VkImageView depthImageView)
{
    framebuffers.resize(imageViews.size());
    for (size_t i = 0; i < imageViews.size(); i++) {
        std::array<VkImageView, 2> attachments = { imageViews[i], depthImageView };
        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = renderPass;
        framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        framebufferInfo.pAttachments = attachments.data();
        framebufferInfo.width = extent.width;
        framebufferInfo.height = extent.height;
        framebufferInfo.layers = 1;
        if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &framebuffers[i]) != VK_SUCCESS)
            throw std::runtime_error("failed to create framebuffer!");
    }
}

void Swapchain::cleanup()
{
    for (auto framebuffer : framebuffers)
        vkDestroyFramebuffer(device, framebuffer, nullptr);
    for (auto imageView : imageViews)
        vkDestroyImageView(device, imageView, nullptr);
    if (swapChain != VK_NULL_HANDLE) {
        vkDestroySwapchainKHR(device, swapChain, nullptr);
        swapChain = VK_NULL_HANDLE;
    }
}
