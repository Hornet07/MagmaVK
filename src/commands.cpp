#include "vulkan.h"

//Commands: command pools
CommandPool_T::CommandPool_T(VkDevice device, VkCommandPoolCreateFlags flags, uint32_t queueFamilyIndex, size_t id) : device(device), id(id) {
    VkCommandPoolCreateInfo commandPoolCreateInfo{};
    commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    commandPoolCreateInfo.flags = flags;
    commandPoolCreateInfo.queueFamilyIndex = queueFamilyIndex;

    if (vkCreateCommandPool(device, &commandPoolCreateInfo, nullptr, &commandPool) != VK_SUCCESS)
        throw std::runtime_error("Failed to create command pool");
}

CommandPool_T::~CommandPool_T() {
    vkDestroyCommandPool(device, commandPool, nullptr);
}

CommandPool Vulkan::createCommandPool(Device device, uint32_t queueFamilyIndex, VkCommandPoolCreateFlags flags) {
    commandPools.emplace(std::piecewise_construct, std::forward_as_tuple(idCounter), std::forward_as_tuple(device->device, flags, queueFamilyIndex, idCounter));
    return &commandPools.at(idCounter++);
}

//Commands: command buffers
VkCommandBuffer CommandPool_T::allocateCommandBuffer(VkCommandBufferLevel level) {
    VkCommandBuffer commandBuffer;

    VkCommandBufferAllocateInfo commandBufferAllocateInfo{};
    commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    commandBufferAllocateInfo.commandPool = commandPool;
    commandBufferAllocateInfo.level = level;
    commandBufferAllocateInfo.commandBufferCount = 1;

    vkAllocateCommandBuffers(device, &commandBufferAllocateInfo, &commandBuffer);

    return commandBuffer;
}

std::vector<VkCommandBuffer> CommandPool_T::allocateCommandBuffers(VkCommandBufferLevel level, uint32_t count) {
    std::vector<VkCommandBuffer> commandBuffers(count);

    VkCommandBufferAllocateInfo commandBufferAllocateInfo{};
    commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    commandBufferAllocateInfo.commandPool = commandPool;
    commandBufferAllocateInfo.level = level;
    commandBufferAllocateInfo.commandBufferCount = count;

    vkAllocateCommandBuffers(device, &commandBufferAllocateInfo, commandBuffers.data());

    return commandBuffers;
}

void CommandPool_T::resetCommandBuffers(VkCommandPoolResetFlags flags) {
    vkResetCommandPool(device, commandPool, flags);
}

void CommandPool_T::freeCommandBuffer(VkCommandBuffer commandBuffer) {
    vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
}

void CommandPool_T::freeCommandBuffers(std::vector<VkCommandBuffer> commandBuffers) {
    vkFreeCommandBuffers(device, commandPool, commandBuffers.size(), commandBuffers.data());
}
