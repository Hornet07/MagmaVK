#pragma once
#include <vulkan/vulkan.h>
#include <vector>

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