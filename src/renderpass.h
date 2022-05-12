#pragma once
#include <vulkan/vulkan.h>
#include <vector>

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
