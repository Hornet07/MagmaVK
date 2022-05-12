#include "vulkan.h"

//Pipeline: shaders
ShaderModule_T::ShaderModule_T(VkDevice device, const std::string& code, size_t id) : device(device), id(id) {
    VkShaderModuleCreateInfo shaderModuleCreateInfo{};
    shaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    shaderModuleCreateInfo.codeSize = code.size();
    shaderModuleCreateInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

    if (vkCreateShaderModule(device, &shaderModuleCreateInfo, nullptr, &shaderModule) != VK_SUCCESS)
        throw std::runtime_error("Failed to create shader module");
}

ShaderModule_T::~ShaderModule_T() {
    vkDestroyShaderModule(device, shaderModule, nullptr);
}

ShaderModule Vulkan::createShaderModule(Device device, const std::string& code) {
    shaderModules.emplace(std::piecewise_construct, std::forward_as_tuple(idCounter), std::forward_as_tuple(device->device, code, idCounter));
    return &shaderModules.at(idCounter++);
}

//Pipeline: layouts
PipelineLayout_T::PipelineLayout_T(VkDevice device, std::vector<VkDescriptorSetLayout> setLayouts, std::vector<VkPushConstantRange> pushConstantRanges, size_t id) : device(device), id(id) {
    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
    pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutCreateInfo.setLayoutCount = setLayouts.size();
    pipelineLayoutCreateInfo.pSetLayouts = setLayouts.data();
    pipelineLayoutCreateInfo.pushConstantRangeCount = pushConstantRanges.size();
    pipelineLayoutCreateInfo.pPushConstantRanges = pushConstantRanges.data();

    if (vkCreatePipelineLayout(device, &pipelineLayoutCreateInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
        throw std::runtime_error("Failed to create pipeline layout");
}

PipelineLayout_T::~PipelineLayout_T() {
    vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
}

PipelineLayout Vulkan::createPipelineLayout(Device device, std::vector<VkDescriptorSetLayout> setLayouts, std::vector<VkPushConstantRange> pushConstantRanges) {
    pipelineLayouts.emplace(std::piecewise_construct, std::forward_as_tuple(idCounter), std::forward_as_tuple(device->device, setLayouts, pushConstantRanges, idCounter));
    return &pipelineLayouts.at(idCounter++);
}

//Pipeline: graphics pipelines
GraphicsPipeline_T::GraphicsPipeline_T(VkDevice device, VkPipelineLayout pipelineLayout, VkRenderPass renderPass, uint32_t subpass, GraphicsPipelineCreateInfo graphicsPipelineCreateInfo, size_t id) : device(device), id(id) {
    graphicsPipelineCreateInfoCache.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    graphicsPipelineCreateInfoCache.flags = graphicsPipelineCreateInfo.flags;
    graphicsPipelineCreateInfoCache.stageCount = graphicsPipelineCreateInfo.stages.size();
    graphicsPipelineCreateInfoCache.pStages = graphicsPipelineCreateInfo.stages.data();
    graphicsPipelineCreateInfoCache.pVertexInputState = &graphicsPipelineCreateInfo.vertexInputState;
    graphicsPipelineCreateInfoCache.pInputAssemblyState = &graphicsPipelineCreateInfo.inputAssemblyState;
    graphicsPipelineCreateInfoCache.pTessellationState = &graphicsPipelineCreateInfo.tessellationState;
    graphicsPipelineCreateInfoCache.pViewportState = &graphicsPipelineCreateInfo.viewportState;
    graphicsPipelineCreateInfoCache.pRasterizationState = &graphicsPipelineCreateInfo.rasterizationState;
    graphicsPipelineCreateInfoCache.pMultisampleState = &graphicsPipelineCreateInfo.multisampleState;
    graphicsPipelineCreateInfoCache.pDepthStencilState = &graphicsPipelineCreateInfo.depthStencilState;
    graphicsPipelineCreateInfoCache.pColorBlendState = &graphicsPipelineCreateInfo.colorBlendState;
    graphicsPipelineCreateInfoCache.pDynamicState = graphicsPipelineCreateInfo.dynamicState.sType == 0 ? nullptr : &graphicsPipelineCreateInfo.dynamicState;
    graphicsPipelineCreateInfoCache.layout = pipelineLayout;
    graphicsPipelineCreateInfoCache.renderPass = renderPass;
    graphicsPipelineCreateInfoCache.subpass = subpass;

    if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE /*TODO*/, 1, &graphicsPipelineCreateInfoCache, nullptr, &pipeline) != VK_SUCCESS)
        throw std::runtime_error("Failed to create graphics pipeline");
}

GraphicsPipeline_T::~GraphicsPipeline_T() {
    vkDestroyPipeline(device, pipeline, nullptr);
}

//TODO Implement
void GraphicsPipeline_T::recreate(GraphicsPipelineCreateInfo graphicsPipelineCreateInfo) {}

//Pipeline: compute pipelines
//TODO

//Pipeline: ray tracing pipelines
//TODO
