// VulkanContext.cpp
#include "VulkanContext.h"
#include <stdexcept>
#include <iostream>
#include <algorithm> // Per std::max, std::vector, ecc.

VulkanContext::VulkanContext()
    : instance(VK_NULL_HANDLE), debugMessenger(VK_NULL_HANDLE),
      physicalDevice(VK_NULL_HANDLE), device(VK_NULL_HANDLE),
      graphicsQueue(VK_NULL_HANDLE), validationEnabled(true)
{
    validationLayers.push_back("VK_LAYER_KHRONOS_validation");
}

VulkanContext::~VulkanContext()
{
}

void VulkanContext::init(VkSurfaceKHR surface, bool enableValidation)
{
    validationEnabled = enableValidation;
    createInstance();
    if (validationEnabled) {
        setupDebugMessenger();
    }
    // Il surface viene passato dall'esterno (non lo creiamo qui).
    pickPhysicalDevice();
    createLogicalDevice();
}

void VulkanContext::cleanup()
{
    vkDestroyDevice(device, nullptr);
    if (validationEnabled) {
        DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
    }
    vkDestroyInstance(instance, nullptr);
}

void VulkanContext::createInstance()
{
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Generic Vulkan Engine";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "Custom Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;
    
    // Estensioni necessarie per Xlib e debug
    std::vector<const char*> extensions = {
        "VK_KHR_surface", 
        "VK_KHR_xlib_surface" // <-- Importante per creare surface su Xlib
    };
    if (validationEnabled) {
        extensions.push_back("VK_EXT_debug_utils");
    }
    
    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();
    
    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
    if (validationEnabled) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
        populateDebugMessengerCreateInfo(debugCreateInfo);
        createInfo.pNext = &debugCreateInfo;
    } else {
        createInfo.enabledLayerCount = 0;
        createInfo.pNext = nullptr;
    }
    
    if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
        throw std::runtime_error("failed to create Vulkan instance!");
    }
}

void VulkanContext::setupDebugMessenger()
{
    VkDebugUtilsMessengerCreateInfoEXT createInfo{};
    populateDebugMessengerCreateInfo(createInfo);

    if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
        throw std::runtime_error("failed to set up debug messenger!");
    }
}

void VulkanContext::pickPhysicalDevice()
{
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
    if (deviceCount == 0) {
        throw std::runtime_error("failed to find GPUs with Vulkan support!");
    }
    
    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());
    
    // Per semplicità, scegliamo il primo dispositivo
    physicalDevice = devices[0];
}

void VulkanContext::createLogicalDevice()
{
    // In un motore completo, qui andrebbe trovata la famiglia di queue corretta
    uint32_t queueFamilyIndex = 0; // Semplificato
    float queuePriority = 1.0f;
    
    VkDeviceQueueCreateInfo queueCreateInfo{};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = queueFamilyIndex;
    queueCreateInfo.queueCount = 1;
    queueCreateInfo.pQueuePriorities = &queuePriority;
    
    std::vector<const char*> deviceExtensions = { "VK_KHR_swapchain" };
    
    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.queueCreateInfoCount = 1;
    createInfo.pQueueCreateInfos = &queueCreateInfo;
    createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
    createInfo.ppEnabledExtensionNames = deviceExtensions.data();
    
    // Non usiamo device layers (deprecati) e lasciamo i feature default
    if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS) {
        throw std::runtime_error("failed to create logical device!");
    }
    
    // Otteniamo la graphics queue
    vkGetDeviceQueue(device, queueFamilyIndex, 0, &graphicsQueue);
}

uint32_t VulkanContext::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) &&
            (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }
    throw std::runtime_error("failed to find suitable memory type!");
}

// ------------------ Funzioni helper per il debug messenger ------------------

VkResult VulkanContext::CreateDebugUtilsMessengerEXT(
    VkInstance instance, 
    const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, 
    const VkAllocationCallbacks* pAllocator, 
    VkDebugUtilsMessengerEXT* pDebugMessenger)
{
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)
        vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");

    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    } else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void VulkanContext::DestroyDebugUtilsMessengerEXT(
    VkInstance instance, 
    VkDebugUtilsMessengerEXT debugMessenger, 
    const VkAllocationCallbacks* pAllocator)
{
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)
        vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");

    if (func != nullptr) {
        func(instance, debugMessenger, pAllocator);
    }
}

void VulkanContext::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
{
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = 
          VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
        | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
        | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType =
          VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
        | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
        | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = [](VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                    VkDebugUtilsMessageTypeFlagsEXT messageType,
                                    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                    void* pUserData) -> VkBool32
    {
        std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
        return VK_FALSE;
    };
}
