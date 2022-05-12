#pragma once
#include <vulkan/vulkan.h>
#include <vector>

//Physical device
struct PhysicalDevice_T {
    PhysicalDevice_T() = delete;
    explicit PhysicalDevice_T(VkPhysicalDevice physicalDevice);

    ~PhysicalDevice_T() = default;

    VkPhysicalDevice physicalDevice;

    VkPhysicalDeviceFeatures features{};
    VkPhysicalDeviceProperties properties{};
    VkPhysicalDeviceMemoryProperties memoryProperties{};
};

typedef PhysicalDevice_T* PhysicalDevice;

//Logical device
struct DeviceCreateInfo {
    uint32_t graphicsQueueCount = 0, computeQueueCount = 0, transferQueueCount = 0;
    std::vector<float> graphicsQueuePriorities, computeQueuePriorities, transferQueuePriorities;

    VkPhysicalDeviceFeatures features = {};
    std::vector<const char*> extensions;
};

struct Device_T {
public:
    Device_T(const Device_T&) = delete;

    Device_T() = delete;
    Device_T(PhysicalDevice physicalDevice, DeviceCreateInfo deviceCreateInfo, size_t id);

    ~Device_T();

    PhysicalDevice physicalDevice;
    VkDevice device = VK_NULL_HANDLE;

    uint32_t graphicsFamily, computeFamily, transferFamily;
    VkQueue* graphicsQueues, * computeQueues, * transferQueues;

private:
    size_t id;

public:
    void wait();
};

typedef Device_T* Device;