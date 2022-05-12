#pragma once
#define countof(arr) (sizeof(arr)/sizeof((arr)[0]))
#include <vulkan/vulkan.h>

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <stdexcept>
#include <thread>

//Debugging
#include <iostream>

//Link
#include "device.h"
#include "swapchain.h"
#include "renderpass.h"
#include "framebuffer.h"
#include "pipeline.h"
#include "commands.h"
#include "synchronization.h"

//Wrapper class
class Vulkan {
public:
    Vulkan(const Vulkan&) = delete;

    Vulkan() = delete;
    Vulkan(const char* const* instanceExtensions, uint32_t instanceExtensionCount);

    ~Vulkan();

    VkInstance instance = VK_NULL_HANDLE;
    std::vector<PhysicalDevice_T> physicalDevices;
    std::unordered_map<size_t, Device_T> devices;
    std::unordered_map<size_t, Swapchain_T> swapchains;
    std::unordered_map<size_t, RenderPass_T> renderPasses;
    std::unordered_map<size_t, Framebuffer_T> framebuffers;
    std::unordered_map<size_t, ShaderModule_T> shaderModules;
    std::unordered_map<size_t, PipelineLayout_T> pipelineLayouts;
    std::unordered_map<size_t, GraphicsPipeline_T> graphicsPipelines;
    std::unordered_map<size_t, CommandPool_T> commandPools;
    std::unordered_map<size_t, Semaphore_T> semaphores;
    std::unordered_map<size_t, Fence_T> fences;

public:
    std::vector<PhysicalDevice> getPhysicalDevices();
    Device createDevice(PhysicalDevice physicalDevice, DeviceCreateInfo deviceCreateInfo);
    Swapchain createSwapchain(Device device, VkSurfaceKHR surface, SwapchainCreateInfo swapchainCreateInfo);
    RenderPass createRenderPass(Device device, std::vector<AttachmentDescription>& attachments, std::vector<SubpassDescription>& subpasses, std::vector<SubpassDependency>& dependencies);
    Framebuffer createFramebuffer(Device device, RenderPass renderPass, FramebufferCreateInfo framebufferCreateInfo);
    ShaderModule createShaderModule(Device device, const std::string& code);
    PipelineLayout createPipelineLayout(Device device, std::vector<VkDescriptorSetLayout/*TODO DescriptorSetLayout*/> setLayouts, std::vector<VkPushConstantRange> pushConstantRanges);
    GraphicsPipeline createGraphicsPipeline(Device device, PipelineLayout pipelineLayout, RenderPass renderPass, uint32_t subpass, GraphicsPipelineCreateInfo graphicsPipelineCreateInfo);
    CommandPool createCommandPool(Device device, uint32_t queueFamilyIndex, VkCommandPoolCreateFlags flags);
    Semaphore createSemaphore(Device device);
    Fence createFence(Device device, bool signaled = false);

private:
    size_t idCounter = 0;
    //VkPipelineCache pipelineCache; TODO
};
