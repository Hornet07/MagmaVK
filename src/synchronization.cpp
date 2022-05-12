#include "vulkan.h"

//Synchronization: semaphores
Semaphore_T::Semaphore_T(VkDevice device, size_t id) : device(device), id(id) {
    VkSemaphoreCreateInfo semaphoreCreateInfo{};
    semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &semaphore);
}

Semaphore_T::~Semaphore_T() {
    vkDestroySemaphore(device, semaphore, nullptr);
}

//Synchronization: fences
Fence_T::Fence_T(VkDevice device, bool signaled, size_t id) : device(device), id(id) {
    VkFenceCreateInfo fenceCreateInfo{};
    fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceCreateInfo.flags = signaled ? VK_FENCE_CREATE_SIGNALED_BIT : 0;

    vkCreateFence(device, &fenceCreateInfo, nullptr, &fence);
}

Fence_T::~Fence_T() {
    vkDestroyFence(device, fence, nullptr);
}

void Fence_T::wait(uint64_t timeout) {
    vkWaitForFences(device, 1, &fence, VK_TRUE, timeout);
}

void Fence_T::reset() {
    vkResetFences(device, 1, &fence);
}
