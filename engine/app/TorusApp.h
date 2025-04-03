#ifndef TORUS_APP_H
#define TORUS_APP_H

#include "Application.h"        // Se hai una classe base per l'app
#include "WindowX11.h"
#include "VulkanContext.h"
#include "Swapchain.h"
#include "RenderPass.h"
#include "DepthResources.h"
#include "Pipeline.h"
#include "Mesh.h"
#include "InputHandler.h"

#include <vulkan/vulkan.h>
#include <glm/glm.hpp>
#include <chrono>
#include <vector>

// Struttura corrispondente al layout(binding = 0) uniform
struct UniformBufferObject {
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
};

class TorusApp : public Application {
public:
    TorusApp(WindowX11* window);
    ~TorusApp();

    void init() override;
    void update() override;
    void render() override;
    void cleanup() override;

    bool shouldClose() const;

private:
    WindowX11* window;
    VulkanContext vulkanContext;
    VkSurfaceKHR surface;

    Swapchain* swapchain;
    RenderPass* renderPass;
    DepthResources* depthResources;
    Pipeline* pipeline;
    Mesh mesh;

    InputHandler inputHandler;

    // Gestione uniform buffer
    VkBuffer uniformBuffer;
    VkDeviceMemory uniformBufferMemory;
    VkDescriptorSetLayout descriptorSetLayout;
    VkDescriptorPool descriptorPool;
    VkDescriptorSet descriptorSet;

    // Per il command pool e command buffer
    VkCommandPool commandPool;
    std::vector<VkCommandBuffer> commandBuffers;

    // Sincronizzazione
    VkSemaphore imageAvailableSemaphore;
    VkSemaphore renderFinishedSemaphore;
    VkFence inFlightFence;

    // Metodi per la pipeline di inizializzazione
    void createSurface();
    void createCommandPool();
    void createCommandBuffers();
    void createSyncObjects();

    // Metodi aggiuntivi per gestire uniform buffer e descriptor
    void createUniformBuffer();
    void createDescriptorPool();
    void createDescriptorSet();
    void updateUniformBuffer();

    // Render loop
    void drawFrame();
    void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
};

#endif // TORUS_APP_H
