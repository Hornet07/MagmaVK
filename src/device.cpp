#include "vulkan.h"

//Physical device
PhysicalDevice_T::PhysicalDevice_T(VkPhysicalDevice physicalDevice) : physicalDevice(physicalDevice) {
    vkGetPhysicalDeviceFeatures(physicalDevice, &features);
    vkGetPhysicalDeviceProperties(physicalDevice, &properties);
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);
}

std::vector<PhysicalDevice> Vulkan::getPhysicalDevices() {
    std::vector<PhysicalDevice> list;
    for (auto& physicalDevice: physicalDevices)
        list.push_back(&physicalDevice);
    return list;
}

//Logical device
Device_T::Device_T(PhysicalDevice physicalDevice, DeviceCreateInfo deviceCreateInfo, size_t id) : physicalDevice(physicalDevice), id(id) {
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice->physicalDevice, &queueFamilyCount, nullptr);

    auto* queueFamilies = new VkQueueFamilyProperties[queueFamilyCount];
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice->physicalDevice, &queueFamilyCount, queueFamilies);

    for (uint32_t i = 0; i < queueFamilyCount; ++i)
        if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) graphicsFamily = i;
        else if (queueFamilies[i].queueFlags & VK_QUEUE_COMPUTE_BIT) computeFamily = i;
        else if (queueFamilies[i].queueFlags & VK_QUEUE_TRANSFER_BIT) transferFamily = i;

    delete[] queueFamilies;

    VkDeviceQueueCreateInfo graphicsQueueCreateInfo{};
    graphicsQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    graphicsQueueCreateInfo.queueFamilyIndex = graphicsFamily;
    graphicsQueueCreateInfo.queueCount = deviceCreateInfo.graphicsQueueCount;
    graphicsQueueCreateInfo.pQueuePriorities = deviceCreateInfo.graphicsQueuePriorities.data();

    VkDeviceQueueCreateInfo computeQueueCreateInfo{};
    computeQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    computeQueueCreateInfo.queueFamilyIndex = computeFamily;
    computeQueueCreateInfo.queueCount = deviceCreateInfo.computeQueueCount;
    computeQueueCreateInfo.pQueuePriorities = deviceCreateInfo.computeQueuePriorities.data();

    VkDeviceQueueCreateInfo transferQueueCreateInfo{};
    transferQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    transferQueueCreateInfo.queueFamilyIndex = transferFamily;
    transferQueueCreateInfo.queueCount = deviceCreateInfo.transferQueueCount;
    transferQueueCreateInfo.pQueuePriorities = deviceCreateInfo.transferQueuePriorities.data();

    VkDeviceQueueCreateInfo queueCreateInfos[] = {graphicsQueueCreateInfo, computeQueueCreateInfo, transferQueueCreateInfo};

    VkDeviceCreateInfo rawDeviceCreateInfo{};
    rawDeviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    rawDeviceCreateInfo.pQueueCreateInfos = queueCreateInfos;
    rawDeviceCreateInfo.queueCreateInfoCount = countof(queueCreateInfos);
    rawDeviceCreateInfo.pEnabledFeatures = &deviceCreateInfo.features;
    rawDeviceCreateInfo.enabledExtensionCount = deviceCreateInfo.extensions.size();
    rawDeviceCreateInfo.ppEnabledExtensionNames = deviceCreateInfo.extensions.data();

    if (vkCreateDevice(physicalDevice->physicalDevice, &rawDeviceCreateInfo, nullptr, &device) != VK_SUCCESS)
        throw std::runtime_error("Failed to create device");

    graphicsQueues = new VkQueue[deviceCreateInfo.graphicsQueueCount];
    computeQueues = new VkQueue[deviceCreateInfo.computeQueueCount];
    transferQueues = new VkQueue[deviceCreateInfo.transferQueueCount];

    for (uint32_t i = 0; i < deviceCreateInfo.graphicsQueueCount; ++i)
        vkGetDeviceQueue(device, graphicsFamily, i, &graphicsQueues[i]);
    for (uint32_t i = 0; i < deviceCreateInfo.computeQueueCount; ++i)
        vkGetDeviceQueue(device, computeFamily, i, &computeQueues[i]);
    for (uint32_t i = 0; i < deviceCreateInfo.transferQueueCount; ++i)
        vkGetDeviceQueue(device, transferFamily, i, &transferQueues[i]);
}

Device_T::~Device_T() {
    delete[] graphicsQueues;
    delete[] computeQueues;
    delete[] transferQueues;

    vkDestroyDevice(device, nullptr);
}

Device Vulkan::createDevice(PhysicalDevice physicalDevice, DeviceCreateInfo deviceCreateInfo) {
    devices.emplace(std::piecewise_construct, std::forward_as_tuple(idCounter), std::forward_as_tuple(physicalDevice, deviceCreateInfo, idCounter));
    return &devices.at(idCounter++);
}

void Device_T::wait() {
    vkDeviceWaitIdle(device);
}
