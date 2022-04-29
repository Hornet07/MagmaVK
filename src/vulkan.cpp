#include "vulkan.h"

//Wrapper class
Vulkan::Vulkan(const char* const* instanceExtensions, uint32_t instanceExtensionCount) {
    VkApplicationInfo applicationInfo{};
    applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    applicationInfo.apiVersion = VK_API_VERSION_1_0;//3;
    applicationInfo.pEngineName = "Magma Engine";
    applicationInfo.engineVersion = VK_MAKE_VERSION(0, 0, 1);
    applicationInfo.pApplicationName = "Magma Tests";
    applicationInfo.applicationVersion = VK_MAKE_VERSION(0, 0, 0);

    VkInstanceCreateInfo instanceCreateInfo{};
    instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceCreateInfo.pApplicationInfo = &applicationInfo;
    instanceCreateInfo.enabledExtensionCount = instanceExtensionCount;
    instanceCreateInfo.ppEnabledExtensionNames = instanceExtensions;

    if (vkCreateInstance(&instanceCreateInfo, nullptr, &instance) != VK_SUCCESS)
        throw std::runtime_error("Failed to create instance");

    uint32_t physicalDeviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr);

    auto* rawPhysicalDevices = new VkPhysicalDevice[physicalDeviceCount];
    vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, rawPhysicalDevices);

    for (uint32_t i = 0; i < physicalDeviceCount; ++i)
        physicalDevices.emplace_back(rawPhysicalDevices[i]);

    delete[] rawPhysicalDevices;
}

Vulkan::~Vulkan() {
    fences.clear();
    semaphores.clear();
    commandPools.clear();
    graphicsPipelines.clear();
    pipelineLayouts.clear();
    shaderModules.clear();
    framebuffers.clear();
    renderPasses.clear();
    swapchains.clear();
    devices.clear();
    physicalDevices.clear();
    vkDestroyInstance(instance, nullptr);
}
