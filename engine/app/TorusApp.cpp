#include "TorusApp.h"
#include <stdexcept>
#include <iostream>
#include <thread>
#include <cstring>
#include <chrono>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

// Inizializza la Surface Xlib usando l'istanza Vulkan valida
void TorusApp::createSurface() {
    VkXlibSurfaceCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR;
    createInfo.dpy = window->getDisplay();
    createInfo.window = window->getWindow();
    if (vkCreateXlibSurfaceKHR(vulkanContext.getInstance(), &createInfo, nullptr, &surface) != VK_SUCCESS) {
        throw std::runtime_error("failed to create Xlib surface!");
    }
}

TorusApp::TorusApp(WindowX11* window)
    : window(window), surface(VK_NULL_HANDLE), swapchain(nullptr), renderPass(nullptr),
      depthResources(nullptr), pipeline(nullptr), uniformBuffer(VK_NULL_HANDLE),
      uniformBufferMemory(VK_NULL_HANDLE), descriptorSetLayout(VK_NULL_HANDLE),
      descriptorPool(VK_NULL_HANDLE), descriptorSet(VK_NULL_HANDLE),
      commandPool(VK_NULL_HANDLE), imageAvailableSemaphore(VK_NULL_HANDLE),
      renderFinishedSemaphore(VK_NULL_HANDLE), inFlightFence(VK_NULL_HANDLE)
{
}

TorusApp::~TorusApp()
{
    // Assicurati di chiamare cleanup() prima di uscire.
}

void TorusApp::init() {
    // 1) Inizializza contesto Vulkan
    vulkanContext.init(VK_NULL_HANDLE, true);

    // 2) Crea la Surface
    createSurface();

    // 3) Crea lo Swapchain
    swapchain = new Swapchain(vulkanContext.getPhysicalDevice(), vulkanContext.getDevice(), surface, 1600, 1200);
    swapchain->create();

    // 4) Crea DepthResources
    depthResources = new DepthResources(vulkanContext.getDevice(), vulkanContext.getPhysicalDevice(), 1600, 1200);
    depthResources->create();

    // 5) Crea RenderPass
    renderPass = new RenderPass(vulkanContext.getDevice(), swapchain->getImageFormat(), depthResources->getFormat());
    renderPass->create();

    // 6) Crea Framebuffer
    swapchain->createFramebuffers(renderPass->get(), depthResources->getImageView());

    // 7) Crea il descriptor set layout per l'uniform buffer
    {
        VkDescriptorSetLayoutBinding uboLayoutBinding{};
        uboLayoutBinding.binding = 0;
        uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        uboLayoutBinding.descriptorCount = 1;
        uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        uboLayoutBinding.pImmutableSamplers = nullptr;

        VkDescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = 1;
        layoutInfo.pBindings = &uboLayoutBinding;

        if (vkCreateDescriptorSetLayout(vulkanContext.getDevice(), &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) {
            throw std::runtime_error("failed to create descriptor set layout!");
        }
    }

    // 8) Crea la pipeline (usa il descriptorSetLayout)
    pipeline = new Pipeline(vulkanContext.getDevice(), renderPass->get(), descriptorSetLayout, swapchain->getExtent());
    pipeline->create();

    // 9) Crea Command Pool
    createCommandPool();

    // 10) Genera la mesh e crea i buffer GPU
    mesh.generateTorus(1.0f, 2.0f, 128, 64);
    mesh.createBuffers(vulkanContext.getDevice(), vulkanContext.getPhysicalDevice(), vulkanContext.getGraphicsQueue(), commandPool);

    // 11) Crea l'uniform buffer e il descriptor set
    createUniformBuffer();
    createDescriptorPool();
    createDescriptorSet();

    // 12) Crea i command buffer e la sincronizzazione
    createCommandBuffers();
    createSyncObjects();
}

void TorusApp::update() {
    // Processa gli eventi X11
    if (!window->pollEvents()) return;

    // Aggiorna input
    int x = window->getMouseX();
    int y = window->getMouseY();
    bool left = window->isLeftButtonDown();
    bool right = window->isRightButtonDown();
    inputHandler.update(x, y, left, right);

    std::cout << "Current Input State: "
              << "Rotation(" << inputHandler.getRotationX() << ", " << inputHandler.getRotationY() << "), "
              << "Translation(" << inputHandler.getTranslationX() << ", " << inputHandler.getTranslationY() << "), "
              << "Zoom(" << inputHandler.getZoom() << ")" << std::endl;

    // Aggiorna uniform buffer con i dati di trasformazione
    updateUniformBuffer();
}

void TorusApp::render() {
    drawFrame();
    std::this_thread::sleep_for(std::chrono::milliseconds(16));
}

void TorusApp::drawFrame() {
    vkWaitForFences(vulkanContext.getDevice(), 1, &inFlightFence, VK_TRUE, UINT64_MAX);
    vkResetFences(vulkanContext.getDevice(), 1, &inFlightFence);

    uint32_t imageIndex;
    vkAcquireNextImageKHR(vulkanContext.getDevice(), swapchain->getSwapchain(), UINT64_MAX,
                          imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);

    vkResetCommandBuffer(commandBuffers[imageIndex], 0);
    recordCommandBuffer(commandBuffers[imageIndex], imageIndex);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    VkSemaphore waitSemaphores[] = { imageAvailableSemaphore };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffers[imageIndex];
    VkSemaphore signalSemaphores[] = { renderFinishedSemaphore };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if (vkQueueSubmit(vulkanContext.getGraphicsQueue(), 1, &submitInfo, inFlightFence) != VK_SUCCESS) {
        throw std::runtime_error("failed to submit draw command buffer!");
    }

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;
    VkSwapchainKHR swapChains[] = { swapchain->getSwapchain() };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;

    vkQueuePresentKHR(vulkanContext.getGraphicsQueue(), &presentInfo);
}

void TorusApp::recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex) {
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    VkClearValue clearValues[2];
    clearValues[0].color = { {0.0f, 0.0f, 0.0f, 1.0f} };
    clearValues[1].depthStencil = {1.0f, 0};

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = renderPass->get();
    renderPassInfo.framebuffer = swapchain->getFramebuffers()[imageIndex];
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = swapchain->getExtent();
    renderPassInfo.clearValueCount = 2;
    renderPassInfo.pClearValues = clearValues;

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->getPipeline());

    // Bind del vertex buffer e index buffer
    VkBuffer vertexBuffers[] = { mesh.getVertexBuffer() };
    VkDeviceSize offsets[] = { 0 };
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(commandBuffer, mesh.getIndexBuffer(), 0, VK_INDEX_TYPE_UINT32);

    // Bind del descriptor set (uniform buffer)
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            pipeline->getPipelineLayout(), 0, 1, &descriptorSet, 0, nullptr);

    // Draw
    vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(mesh.getIndices().size()), 1, 0, 0, 0);

    vkCmdEndRenderPass(commandBuffer);

    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to record command buffer!");
    }
}

bool TorusApp::shouldClose() const {
    return window->shouldClose();
}

void TorusApp::createCommandPool() {
    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = 0; // Semplificato
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    if (vkCreateCommandPool(vulkanContext.getDevice(), &poolInfo, nullptr, &commandPool) != VK_SUCCESS) {
        throw std::runtime_error("failed to create command pool!");
    }
}

void TorusApp::createCommandBuffers() {
    size_t framebufferCount = swapchain->getImageViews().size();
    commandBuffers.resize(framebufferCount);

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = static_cast<uint32_t>(framebufferCount);

    if (vkAllocateCommandBuffers(vulkanContext.getDevice(), &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate command buffers!");
    }
}

void TorusApp::createSyncObjects() {
    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    if (vkCreateSemaphore(vulkanContext.getDevice(), &semaphoreInfo, nullptr, &imageAvailableSemaphore) != VK_SUCCESS ||
        vkCreateSemaphore(vulkanContext.getDevice(), &semaphoreInfo, nullptr, &renderFinishedSemaphore) != VK_SUCCESS ||
        vkCreateFence(vulkanContext.getDevice(), &fenceInfo, nullptr, &inFlightFence) != VK_SUCCESS) {
        throw std::runtime_error("failed to create synchronization objects!");
    }
}

// -------------------- Creazione e gestione dell'uniform buffer --------------------
void TorusApp::createUniformBuffer() {
    VkDeviceSize bufferSize = sizeof(UniformBufferObject);

    // Creiamo un buffer CPU-visibile per aggiornare le matrici a ogni frame
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = bufferSize;
    bufferInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(vulkanContext.getDevice(), &bufferInfo, nullptr, &uniformBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to create uniform buffer!");
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(vulkanContext.getDevice(), uniformBuffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;

    // Trova un tipo di memoria host visible
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(vulkanContext.getPhysicalDevice(), &memProperties);

    bool found = false;
    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((memRequirements.memoryTypeBits & (1 << i)) &&
            (memProperties.memoryTypes[i].propertyFlags & (VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)) ==
            (VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)) {
            allocInfo.memoryTypeIndex = i;
            found = true;
            break;
        }
    }
    if (!found) {
        throw std::runtime_error("failed to find suitable memory type for uniform buffer!");
    }

    if (vkAllocateMemory(vulkanContext.getDevice(), &allocInfo, nullptr, &uniformBufferMemory) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate uniform buffer memory!");
    }

    vkBindBufferMemory(vulkanContext.getDevice(), uniformBuffer, uniformBufferMemory, 0);
}

void TorusApp::createDescriptorPool() {
    // Avremo un solo descriptor set
    VkDescriptorPoolSize poolSize{};
    poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSize.descriptorCount = 1;

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = 1;
    poolInfo.pPoolSizes = &poolSize;
    poolInfo.maxSets = 1;

    if (vkCreateDescriptorPool(vulkanContext.getDevice(), &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor pool!");
    }
}

void TorusApp::createDescriptorSet() {
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptorPool;
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = &descriptorSetLayout;

    if (vkAllocateDescriptorSets(vulkanContext.getDevice(), &allocInfo, &descriptorSet) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate descriptor set!");
    }

    VkDescriptorBufferInfo bufferInfo{};
    bufferInfo.buffer = uniformBuffer;
    bufferInfo.offset = 0;
    bufferInfo.range = sizeof(UniformBufferObject);

    VkWriteDescriptorSet descriptorWrite{};
    descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrite.dstSet = descriptorSet;
    descriptorWrite.dstBinding = 0;
    descriptorWrite.dstArrayElement = 0;
    descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptorWrite.descriptorCount = 1;
    descriptorWrite.pBufferInfo = &bufferInfo;

    vkUpdateDescriptorSets(vulkanContext.getDevice(), 1, &descriptorWrite, 0, nullptr);
}

void TorusApp::updateUniformBuffer() {
    // Crea la struttura UBO
    UniformBufferObject ubo{};

    // Costruisci la matrice model in base all'input
    ubo.model = glm::translate(glm::mat4(1.0f),
                               glm::vec3(inputHandler.getTranslationX(),
                                         inputHandler.getTranslationY(),
                                         0.0f))
              * glm::rotate(glm::mat4(1.0f), inputHandler.getRotationX(), glm::vec3(1.0f, 0.0f, 0.0f))
              * glm::rotate(glm::mat4(1.0f), inputHandler.getRotationY(), glm::vec3(0.0f, 1.0f, 0.0f))
              * glm::scale(glm::mat4(1.0f), glm::vec3(inputHandler.getZoom()));

    // View: camera che guarda l'origine dall'asse Z
    ubo.view = glm::lookAt(glm::vec3(0.0f, 0.0f, 10.0f),
                           glm::vec3(0.0f, 0.0f, 0.0f),
                           glm::vec3(0.0f, 1.0f, 0.0f));

    // Proiezione prospettica
    ubo.proj = glm::perspective(glm::radians(45.0f),
                                swapchain->getExtent().width / (float) swapchain->getExtent().height,
                                0.1f, 100.0f);
    ubo.proj[1][1] *= -1; // Correzione per Vulkan

    // (Facoltativo) log di debug
    std::cout << "Model Matrix:\n" << glm::to_string(ubo.model) << std::endl;

    // Copia il dato nell'uniform buffer
    void* data;
    vkMapMemory(vulkanContext.getDevice(), uniformBufferMemory, 0, sizeof(ubo), 0, &data);
    memcpy(data, &ubo, sizeof(ubo));
    vkUnmapMemory(vulkanContext.getDevice(), uniformBufferMemory);
}

void TorusApp::cleanup() {
    vkDeviceWaitIdle(vulkanContext.getDevice());

    // Distruggi uniform buffer
    vkDestroyBuffer(vulkanContext.getDevice(), uniformBuffer, nullptr);
    vkFreeMemory(vulkanContext.getDevice(), uniformBufferMemory, nullptr);

    vkDestroyDescriptorPool(vulkanContext.getDevice(), descriptorPool, nullptr);
    vkDestroyDescriptorSetLayout(vulkanContext.getDevice(), descriptorSetLayout, nullptr);

    vkDestroyCommandPool(vulkanContext.getDevice(), commandPool, nullptr);
    vkDestroySemaphore(vulkanContext.getDevice(), renderFinishedSemaphore, nullptr);
    vkDestroySemaphore(vulkanContext.getDevice(), imageAvailableSemaphore, nullptr);
    vkDestroyFence(vulkanContext.getDevice(), inFlightFence, nullptr);

    delete pipeline;
    delete renderPass;
    delete depthResources;

    swapchain->cleanup();
    delete swapchain;

    vkDestroySurfaceKHR(vulkanContext.getInstance(), surface, nullptr);
    vulkanContext.cleanup();
}
