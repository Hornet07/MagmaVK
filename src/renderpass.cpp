#include "vulkan.h"

//Render pass: attachments
VkAttachmentDescription AttachmentDescription::serialize() {
    return {
            .flags = flags,
            .format = format,
            .samples = samples,
            .loadOp = loadOp,
            .storeOp = storeOp,
            .stencilLoadOp = stencilLoadOp,
            .stencilStoreOp = stencilStoreOp,
            .initialLayout = initialLayout,
            .finalLayout = finalLayout
    };
}

std::vector<VkAttachmentDescription> AttachmentDescription::serialize(std::vector<AttachmentDescription>& attachments) {
    std::vector<VkAttachmentDescription> raw(attachments.size());
    for (size_t i = 0; i < attachments.size(); ++i)
        raw[i] = attachments[i].serialize();
    return raw;
}

VkAttachmentReference AttachmentReference::serialize() {
    return {
            .attachment = attachment,
            .layout = layout
    };
}

std::vector<VkAttachmentReference> AttachmentReference::serialize(std::vector<AttachmentReference>& references) {
    std::vector<VkAttachmentReference> raw(references.size());
    for (size_t i = 0; i < references.size(); ++i)
        raw[i] = references[i].serialize();
    return raw;
}

//Render pass: subpasses
VkSubpassDescription SubpassDescription::serialize() {
    rawInputAttachments = AttachmentReference::serialize(inputAttachments);
    rawColorAttachments = AttachmentReference::serialize(colorAttachments);
    rawResolveAttachments = AttachmentReference::serialize(resolveAttachments);
    rawDepthStencilAttachment = depthStencilAttachment.serialize();
    return {
            .flags = flags,
            .pipelineBindPoint = pipelineBindPoint,
            .inputAttachmentCount = static_cast<uint32_t>(inputAttachments.size()),
            .pInputAttachments = rawInputAttachments.data(),
            .colorAttachmentCount = static_cast<uint32_t>(colorAttachments.size()),
            .pColorAttachments = rawColorAttachments.data(),
            .pResolveAttachments = rawResolveAttachments.data(),
            .pDepthStencilAttachment = (depthStencilAttachment.attachment == UINT32_MAX || depthStencilAttachment.layout == VK_IMAGE_LAYOUT_UNDEFINED) ? nullptr : &rawDepthStencilAttachment,
            .preserveAttachmentCount = static_cast<uint32_t>(preserveAttachments.size()),
            .pPreserveAttachments = preserveAttachments.data()
    };
}

std::vector<VkSubpassDescription> SubpassDescription::serialize(std::vector<SubpassDescription>& descriptions) {
    std::vector<VkSubpassDescription> raw(descriptions.size());
    for (size_t i = 0; i < descriptions.size(); ++i)
        raw[i] = descriptions[i].serialize();
    return raw;
}

VkSubpassDependency SubpassDependency::serialize() {
    return {
            .srcSubpass = srcSubpass,
            .dstSubpass = dstSubpass,
            .srcStageMask = srcStageMask,
            .dstStageMask = dstStageMask,
            .srcAccessMask = srcAccessMask,
            .dstAccessMask = dstAccessMask,
            .dependencyFlags = dependencyFlags
    };
}

std::vector<VkSubpassDependency> SubpassDependency::serialize(std::vector<SubpassDependency>& dependencies) {
    std::vector<VkSubpassDependency> raw(dependencies.size());
    for (size_t i = 0; i < dependencies.size(); ++i)
        raw[i] = dependencies[i].serialize();
    return raw;
}

//Render pass
RenderPass_T::RenderPass_T(VkDevice device, std::vector<AttachmentDescription>& attachments, std::vector<SubpassDescription>& subpasses, std::vector<SubpassDependency>& dependencies, size_t id) : device(device), id(id) {
    auto rawAttachments = AttachmentDescription::serialize(attachments);
    auto rawSubpasses = SubpassDescription::serialize(subpasses);
    auto rawDependencies = SubpassDependency::serialize(dependencies);

    VkRenderPassCreateInfo renderPassCreateInfo{};
    renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassCreateInfo.attachmentCount = attachments.size();
    renderPassCreateInfo.pAttachments = rawAttachments.data();
    renderPassCreateInfo.subpassCount = subpasses.size();
    renderPassCreateInfo.pSubpasses = rawSubpasses.data();
    renderPassCreateInfo.dependencyCount = dependencies.size();
    renderPassCreateInfo.pDependencies = rawDependencies.data();

    if (vkCreateRenderPass(device, &renderPassCreateInfo, nullptr, &renderPass) != VK_SUCCESS)
        throw std::runtime_error("Failed to create render pass");
}

RenderPass_T::~RenderPass_T() {
    vkDestroyRenderPass(device, renderPass, nullptr);
}

RenderPass Vulkan::createRenderPass(Device device, std::vector<AttachmentDescription>& attachments, std::vector<SubpassDescription>& subpasses, std::vector<SubpassDependency>& dependencies) {
    renderPasses.emplace(std::piecewise_construct, std::forward_as_tuple(idCounter), std::forward_as_tuple(device->device, attachments, subpasses, dependencies, idCounter));
    return &renderPasses.at(idCounter++);
}