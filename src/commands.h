#pragma once
#include <vulkan/vulkan.h>

//Commands: command pools
struct CommandPool_T {
public:
    CommandPool_T(const CommandPool_T&) = delete;

    CommandPool_T() = delete;
    CommandPool_T(VkDevice device, VkCommandPoolCreateFlags flags, uint32_t queueFamilyIndex, size_t id);

    ~CommandPool_T();

    VkCommandPool commandPool = VK_NULL_HANDLE;

private:
    size_t id;
    VkDevice device;

public:
    VkCommandBuffer allocateCommandBuffer(VkCommandBufferLevel level);
    std::vector<VkCommandBuffer> allocateCommandBuffers(VkCommandBufferLevel level, uint32_t count);

    void resetCommandBuffers(VkCommandPoolResetFlags flags = 0);
    void freeCommandBuffer(VkCommandBuffer commandBuffer);
    void freeCommandBuffers(std::vector<VkCommandBuffer> commandBuffers);
};

typedef CommandPool_T* CommandPool;

//Commands: command buffers
//NULL
