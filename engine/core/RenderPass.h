// RenderPass.h
#ifndef RENDER_PASS_H
#define RENDER_PASS_H

#include <vulkan/vulkan.h>

class RenderPass {
public:
    RenderPass(VkDevice device, VkFormat swapChainImageFormat, VkFormat depthFormat);
    ~RenderPass();

    void create();
    VkRenderPass get() const { return renderPass; }

private:
    VkDevice device;
    VkFormat swapChainImageFormat;
    VkFormat depthFormat;
    VkRenderPass renderPass;
};

#endif // RENDER_PASS_H
