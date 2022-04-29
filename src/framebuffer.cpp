#include "vulkan.h"

//Framebuffer
Framebuffer_T::Framebuffer_T(VkDevice device, VkRenderPass renderPass, FramebufferCreateInfo framebufferCreateInfo, size_t id) : device(device), id(id) {
    VkFramebufferCreateInfo rawFramebufferCreateInfo{};
    rawFramebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    rawFramebufferCreateInfo.flags = framebufferCreateInfo.flags;
    rawFramebufferCreateInfo.renderPass = renderPass;
    rawFramebufferCreateInfo.attachmentCount = framebufferCreateInfo.attachments.size();
    rawFramebufferCreateInfo.pAttachments = framebufferCreateInfo.attachments.data();
    rawFramebufferCreateInfo.width = framebufferCreateInfo.width;
    rawFramebufferCreateInfo.height = framebufferCreateInfo.height;
    rawFramebufferCreateInfo.layers = framebufferCreateInfo.layers;

    if (vkCreateFramebuffer(device, &rawFramebufferCreateInfo, nullptr, &framebuffer) != VK_SUCCESS)
        throw std::runtime_error("Failed to create framebuffer");
}

Framebuffer_T::~Framebuffer_T() {
    vkDestroyFramebuffer(device, framebuffer, nullptr);
}

Framebuffer Vulkan::createFramebuffer(Device device, RenderPass renderPass, FramebufferCreateInfo framebufferCreateInfo) {
    framebuffers.emplace(std::piecewise_construct, std::forward_as_tuple(idCounter), std::forward_as_tuple(device->device, renderPass->renderPass, framebufferCreateInfo, idCounter));
    return &framebuffers.at(idCounter++);
}