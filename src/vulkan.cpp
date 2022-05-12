#include "vulkan.h"

//Wrapper class
Vulkan::Vulkan(const char* const* instanceExtensions, uint32_t instanceExtensionCount) {
    VkApplicationInfo applicationInfo{};
    applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    applicationInfo.apiVersion = VK_API_VERSION_1_3;
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

std::vector<PhysicalDevice> Vulkan::getPhysicalDevices() {
    std::vector<PhysicalDevice> list;
    for (auto& physicalDevice: physicalDevices)
        list.push_back(&physicalDevice);
    return list;
}

Device Vulkan::createDevice(PhysicalDevice physicalDevice, DeviceCreateInfo deviceCreateInfo) {
    devices.emplace(std::piecewise_construct, std::forward_as_tuple(idCounter), std::forward_as_tuple(physicalDevice, deviceCreateInfo, idCounter));
    return &devices.at(idCounter++);
}

Swapchain Vulkan::createSwapchain(Device device, VkSurfaceKHR surface, SwapchainCreateInfo swapchainCreateInfo) {
    swapchains.emplace(std::piecewise_construct, std::forward_as_tuple(idCounter), std::forward_as_tuple(device->device, surface, swapchainCreateInfo, idCounter));
    return &swapchains.at(idCounter++);
}

RenderPass Vulkan::createRenderPass(Device device, std::vector<AttachmentDescription>& attachments, std::vector<SubpassDescription>& subpasses, std::vector<SubpassDependency>& dependencies) {
    renderPasses.emplace(std::piecewise_construct, std::forward_as_tuple(idCounter), std::forward_as_tuple(device->device, attachments, subpasses, dependencies, idCounter));
    return &renderPasses.at(idCounter++);
}

Framebuffer Vulkan::createFramebuffer(Device device, RenderPass renderPass, FramebufferCreateInfo framebufferCreateInfo) {
    framebuffers.emplace(std::piecewise_construct, std::forward_as_tuple(idCounter), std::forward_as_tuple(device->device, renderPass->renderPass, framebufferCreateInfo, idCounter));
    return &framebuffers.at(idCounter++);
}

GraphicsPipeline Vulkan::createGraphicsPipeline(Device device, PipelineLayout pipelineLayout, RenderPass renderPass, uint32_t subpass, GraphicsPipelineCreateInfo graphicsPipelineCreateInfo) {
    graphicsPipelines.emplace(std::piecewise_construct, std::forward_as_tuple(idCounter), std::forward_as_tuple(device->device, pipelineLayout->pipelineLayout, renderPass->renderPass, subpass, graphicsPipelineCreateInfo, idCounter));
    return &graphicsPipelines.at(idCounter++);
}

CommandPool Vulkan::createCommandPool(Device device, uint32_t queueFamilyIndex, VkCommandPoolCreateFlags flags) {
    commandPools.emplace(std::piecewise_construct, std::forward_as_tuple(idCounter), std::forward_as_tuple(device->device, flags, queueFamilyIndex, idCounter));
    return &commandPools.at(idCounter++);
}

Semaphore Vulkan::createSemaphore(Device device) {
    semaphores.emplace(std::piecewise_construct, std::forward_as_tuple(idCounter), std::forward_as_tuple(device->device, idCounter));
    return &semaphores.at(idCounter++);
}

Fence Vulkan::createFence(Device device, bool signaled) {
    fences.emplace(std::piecewise_construct, std::forward_as_tuple(idCounter),
                   std::forward_as_tuple(device->device, signaled, idCounter));
    return &fences.at(idCounter++);
}
