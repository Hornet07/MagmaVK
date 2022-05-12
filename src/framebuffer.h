#pragma once
#include <vulkan/vulkan.h>
#include <vector>

//Framebuffer
struct FramebufferCreateInfo {
    VkFramebufferCreateFlags flags = 0;
    std::vector<VkImageView> attachments;
    uint32_t width = 0;
    uint32_t height = 0;
    uint32_t layers = 1;
};

struct Framebuffer_T {
public:
    Framebuffer_T(const Framebuffer_T&) = delete;

    Framebuffer_T() = delete;
    Framebuffer_T(VkDevice device, VkRenderPass renderPass, FramebufferCreateInfo framebufferCreateInfo, size_t id);

    ~Framebuffer_T();

    VkFramebuffer framebuffer = VK_NULL_HANDLE;

private:
    size_t id;
    VkDevice device;
};

typedef Framebuffer_T* Framebuffer;
