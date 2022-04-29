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

//Swapchain
struct SwapchainCreateInfo {
    VkSwapchainCreateFlagsKHR flags = 0;
    uint32_t minImageCount = 1;
    VkFormat imageFormat = VK_FORMAT_R8G8B8A8_SRGB;
    VkColorSpaceKHR imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
    VkExtent2D imageExtent = {0, 0};
    uint32_t imageArrayLayers = 1;
    VkImageUsageFlags imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    VkSharingMode imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    uint32_t queueFamilyIndexCount = 0;
    uint32_t* queueFamilyIndices = nullptr;
    VkSurfaceTransformFlagBitsKHR preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    VkCompositeAlphaFlagBitsKHR compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    VkPresentModeKHR presentMode = VK_PRESENT_MODE_MAILBOX_KHR;
    VkBool32 clipped = VK_TRUE;
};

struct Swapchain_T {
public:
    Swapchain_T(const Swapchain_T&) = delete;

    Swapchain_T() = delete;
    Swapchain_T(VkDevice device, VkSurfaceKHR surface, SwapchainCreateInfo swapchainCreateInfo, size_t id);

    ~Swapchain_T();

    VkSurfaceKHR surface;
    VkSwapchainKHR swapchain = VK_NULL_HANDLE;

    uint32_t imageCount;
    VkFormat imageFormat;
    VkExtent2D imageExtent;

    std::vector<VkImage> images;
    std::vector<VkImageView> imageViews;

private:
    size_t id;
    VkSwapchainCreateInfoKHR swapchainCreateInfoCache{};
    VkDevice device;

public:
    void resize(VkExtent2D extent);
    void recreate(SwapchainCreateInfo swapchainCreateInfo);
};

typedef Swapchain_T* Swapchain;

//Render pass: attachments
struct AttachmentDescription {
    VkAttachmentDescriptionFlags flags = 0;
    VkFormat format = VK_FORMAT_UNDEFINED;
    VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT;
    VkAttachmentLoadOp loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
    VkAttachmentStoreOp storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    VkAttachmentLoadOp stencilLoadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
    VkAttachmentStoreOp stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
    VkImageLayout initialLayout = VK_IMAGE_LAYOUT_GENERAL;
    VkImageLayout finalLayout = VK_IMAGE_LAYOUT_GENERAL;

    VkAttachmentDescription serialize();
    static std::vector<VkAttachmentDescription> serialize(std::vector<AttachmentDescription>& attachments);
};

struct AttachmentReference {
    uint32_t attachment = UINT32_MAX;
    VkImageLayout layout = VK_IMAGE_LAYOUT_GENERAL;

    VkAttachmentReference serialize();
    static std::vector<VkAttachmentReference> serialize(std::vector<AttachmentReference>& references);
};

//Render pass: subpasses
struct SubpassDescription {
    VkSubpassDescriptionFlags flags = 0;
    VkPipelineBindPoint pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    std::vector<AttachmentReference> inputAttachments;
    std::vector<AttachmentReference> colorAttachments;
    std::vector<AttachmentReference> resolveAttachments;
    AttachmentReference depthStencilAttachment;
    std::vector<uint32_t> preserveAttachments;

    VkSubpassDescription serialize();
    static std::vector<VkSubpassDescription> serialize(std::vector<SubpassDescription>& descriptions);

private:
    std::vector<VkAttachmentReference> rawInputAttachments;
    std::vector<VkAttachmentReference> rawColorAttachments;
    std::vector<VkAttachmentReference> rawResolveAttachments;
    VkAttachmentReference rawDepthStencilAttachment;
};

struct SubpassDependency {
    uint32_t srcSubpass{};
    uint32_t dstSubpass{};
    VkPipelineStageFlags srcStageMask{};
    VkPipelineStageFlags dstStageMask{};
    VkAccessFlags srcAccessMask{};
    VkAccessFlags dstAccessMask{};
    VkDependencyFlags dependencyFlags = 0;

    VkSubpassDependency serialize();
    static std::vector<VkSubpassDependency> serialize(std::vector<SubpassDependency>& dependencies);
};

//Render pass
struct RenderPass_T {
public:
    RenderPass_T(const RenderPass_T&) = delete;

    RenderPass_T() = delete;
    RenderPass_T(VkDevice device, std::vector<AttachmentDescription>& attachments, std::vector<SubpassDescription>& subpasses, std::vector<SubpassDependency>& dependencies, size_t id);

    ~RenderPass_T();

    VkRenderPass renderPass = VK_NULL_HANDLE;

private:
    size_t id;
    VkDevice device;
};

typedef RenderPass_T* RenderPass;

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

//Pipeline: caches
//TODO Don't actually implement this here

//Pipeline: shaders
struct ShaderModule_T {
public:
    ShaderModule_T(const ShaderModule_T&) = delete;

    ShaderModule_T() = delete;
    ShaderModule_T(VkDevice device, const std::string& code, size_t id);

    ~ShaderModule_T();

    VkShaderModule shaderModule = VK_NULL_HANDLE;

private:
    size_t id;
    VkDevice device;
};

typedef ShaderModule_T* ShaderModule;

//Pipeline: layouts
struct PipelineLayout_T {
public:
    PipelineLayout_T(const PipelineLayout_T&) = delete;

    PipelineLayout_T() = delete;
    PipelineLayout_T(VkDevice device, std::vector<VkDescriptorSetLayout> setLayouts, std::vector<VkPushConstantRange> pushConstantRanges, size_t id);

    ~PipelineLayout_T();

    VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;

private:
    size_t id;
    VkDevice device;
};

typedef PipelineLayout_T* PipelineLayout;

//TODO Add bulk pipeline creation support

//Pipeline: graphics pipelines
//TODO
struct GraphicsPipelineCreateInfo {
    VkPipelineCreateFlags flags = 0;
    std::vector<VkPipelineShaderStageCreateInfo> stages;
    VkPipelineVertexInputStateCreateInfo vertexInputState{};
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyState{};
    VkPipelineTessellationStateCreateInfo tessellationState{};
    VkPipelineViewportStateCreateInfo viewportState{};
    VkPipelineRasterizationStateCreateInfo rasterizationState{};
    VkPipelineMultisampleStateCreateInfo multisampleState{};
    VkPipelineDepthStencilStateCreateInfo depthStencilState{};
    VkPipelineColorBlendStateCreateInfo colorBlendState{};
    VkPipelineDynamicStateCreateInfo dynamicState{.sType = static_cast<VkStructureType>(0)};
};

struct GraphicsPipeline_T {
public:
    GraphicsPipeline_T(const GraphicsPipeline_T&) = delete;

    GraphicsPipeline_T() = delete;
    GraphicsPipeline_T(VkDevice device, VkPipelineLayout pipelineLayout, VkRenderPass renderPass, uint32_t subpass, GraphicsPipelineCreateInfo graphicsPipelineCreateInfo, size_t id);

    ~GraphicsPipeline_T();

    //TODO Add pipeline cache here
    VkPipeline pipeline = VK_NULL_HANDLE;

private:
    size_t id;
    VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfoCache{};
    VkDevice device;

public:
    void recreate(GraphicsPipelineCreateInfo graphicsPipelineCreateInfo);
};

typedef GraphicsPipeline_T* GraphicsPipeline;

//Pipeline: compute pipelines
//TODO

//Pipeline: ray tracing pipelines
//TODO

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
