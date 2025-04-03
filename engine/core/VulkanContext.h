// VulkanContext.h
#ifndef VULKAN_CONTEXT_H
#define VULKAN_CONTEXT_H

#include <vulkan/vulkan.h>
#include <vector>
#include <string>

class VulkanContext {
public:
    VulkanContext();
    ~VulkanContext();

    // Inizializza l'istanza, il debug messenger, seleziona physical device e crea il logical device.
    // Il surface viene creato esternamente.
    void init(VkSurfaceKHR surface, bool enableValidation);
    void cleanup();

    VkInstance getInstance() const { return instance; }
    VkDevice getDevice() const { return device; }
    VkPhysicalDevice getPhysicalDevice() const { return physicalDevice; }
    VkQueue getGraphicsQueue() const { return graphicsQueue; }

    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

private:
    VkInstance instance;
    VkDebugUtilsMessengerEXT debugMessenger;
    VkPhysicalDevice physicalDevice;
    VkDevice device;
    VkQueue graphicsQueue;

    bool validationEnabled;
    std::vector<const char*> validationLayers;

    void createInstance();
    void setupDebugMessenger();
    void pickPhysicalDevice();
    void createLogicalDevice();

    // Funzioni helper per il debug messenger
    VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, 
        const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, 
        const VkAllocationCallbacks* pAllocator, 
        VkDebugUtilsMessengerEXT* pDebugMessenger);
    void DestroyDebugUtilsMessengerEXT(VkInstance instance, 
        VkDebugUtilsMessengerEXT debugMessenger, 
        const VkAllocationCallbacks* pAllocator);
    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
};

#endif // VULKAN_CONTEXT_H
