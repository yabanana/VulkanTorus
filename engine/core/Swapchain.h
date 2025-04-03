// Swapchain.h
#ifndef SWAPCHAIN_H
#define SWAPCHAIN_H

#include <vulkan/vulkan.h>
#include <vector>

class Swapchain {
public:
    Swapchain(VkPhysicalDevice physicalDevice, VkDevice device, VkSurfaceKHR surface, uint32_t width, uint32_t height);
    ~Swapchain();

    void create();
    void cleanup();

    VkSwapchainKHR getSwapchain() const { return swapChain; }
    VkFormat getImageFormat() const { return imageFormat; }
    VkExtent2D getExtent() const { return extent; }
    const std::vector<VkImageView>& getImageViews() const { return imageViews; }
    const std::vector<VkFramebuffer>& getFramebuffers() const { return framebuffers; }

    // Crea i framebuffer dati render pass e depth image view
    void createFramebuffers(VkRenderPass renderPass, VkImageView depthImageView);

private:
    VkPhysicalDevice physicalDevice;
    VkDevice device;
    VkSurfaceKHR surface;
    uint32_t width, height;

    VkSwapchainKHR swapChain;
    VkFormat imageFormat;
    VkExtent2D extent;
    std::vector<VkImage> images;
    std::vector<VkImageView> imageViews;
    std::vector<VkFramebuffer> framebuffers;

    void createSwapchain();
    void createImageViews();
};

#endif // SWAPCHAIN_H
