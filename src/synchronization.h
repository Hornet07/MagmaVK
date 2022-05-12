#pragma once
#include <vulkan/vulkan.h>

//Synchronization: semaphores
struct Semaphore_T {
public:
    Semaphore_T(const Semaphore_T&) = delete;

    Semaphore_T() = delete;
    Semaphore_T(VkDevice device, size_t id);

    ~Semaphore_T();

    VkSemaphore semaphore = VK_NULL_HANDLE;

private:
    size_t id;
    VkDevice device;
};

typedef Semaphore_T* Semaphore;

//Synchronization: fences
struct Fence_T {
public:
    Fence_T(const Fence_T&) = delete;

    Fence_T() = delete;
    Fence_T(VkDevice device, bool signaled, size_t id);

    ~Fence_T();

    VkFence fence = VK_NULL_HANDLE;

private:
    size_t id;
    VkDevice device;

public:
    void wait(uint64_t timeout = UINT64_MAX);
    void reset();
};

typedef Fence_T* Fence;