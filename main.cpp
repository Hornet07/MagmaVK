#include <src/vulkan.h>
#include <src/initializers.h>
#include <GLFW/glfw3.h>
#include "utils.h"

//Debugging
#include <iostream>

int main() {
    glfwInit();

    //Create window
    glfwDefaultWindowHints();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    //glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);
    GLFWwindow* window = glfwCreateWindow(800, 600, "MagmaVK", nullptr, nullptr);
    glfwSetWindowOpacity(window, 0.9f);
    glfwSetWindowAttrib(window, GLFW_DECORATED, GLFW_FALSE);

    //Create instance
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    Vulkan vulkan = Vulkan(extensions.data(), extensions.size());
    auto physicalDevices = vulkan.getPhysicalDevices();

    //Create device
    DeviceCreateInfo deviceCreateInfo;
    deviceCreateInfo.graphicsQueueCount = 1;
    deviceCreateInfo.graphicsQueuePriorities = {1.0f};
    deviceCreateInfo.features = {.samplerAnisotropy = 16};
    deviceCreateInfo.extensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
    Device device = vulkan.createDevice(physicalDevices.back(), deviceCreateInfo);

    //Create window surface
    VkSurfaceKHR surface;
    glfwCreateWindowSurface(vulkan.instance, window, nullptr, &surface);

    //Create swapchain
    SwapchainCreateInfo swapchainCreateInfo;
    swapchainCreateInfo.minImageCount = 3;
    swapchainCreateInfo.imageExtent = {800, 600};
    swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchainCreateInfo.imageFormat = VK_FORMAT_R8G8B8A8_SRGB;
    swapchainCreateInfo.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
    Swapchain swapchain = vulkan.createSwapchain(device, surface, swapchainCreateInfo);

    //Create render pass
    AttachmentDescription colorAttachment;
    colorAttachment.format = swapchain->imageFormat;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    std::vector<AttachmentDescription> attachments = {colorAttachment};

    SubpassDescription subpassDescription;
    subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpassDescription.colorAttachments = {{0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL}};

    std::vector<SubpassDescription> subpasses = {subpassDescription};

    SubpassDependency subpassDependency;
    subpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    subpassDependency.dstSubpass = 0;
    subpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    subpassDependency.srcAccessMask = 0;
    subpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    subpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    std::vector<SubpassDependency> dependencies = {subpassDependency};

    RenderPass renderPass = vulkan.createRenderPass(device, attachments, subpasses, dependencies);

    //Create framebuffers
    FramebufferCreateInfo framebufferCreateInfo;
    framebufferCreateInfo.width = swapchain->imageExtent.width;
    framebufferCreateInfo.height = swapchain->imageExtent.height;

    std::vector<Framebuffer> framebuffers(swapchain->imageCount);
    for (uint32_t i = 0; i < swapchain->imageCount; ++i) {
        framebufferCreateInfo.attachments = {swapchain->imageViews[i]};
        framebuffers[i] = vulkan.createFramebuffer(device, renderPass, framebufferCreateInfo);
    }

    //Create graphics pipeline
    ShaderModule vertexShaderModule = vulkan.createShaderModule(device, readFile("../shaders/vertex.spv"));
    ShaderModule fragmentShaderModule = vulkan.createShaderModule(device, readFile("../shaders/fragment.spv"));

    VkPipelineShaderStageCreateInfo vertexShader{};
    vertexShader.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertexShader.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertexShader.module = vertexShaderModule->shaderModule;
    vertexShader.pName = "main";

    VkPipelineShaderStageCreateInfo fragmentShader{};
    fragmentShader.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragmentShader.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragmentShader.module = fragmentShaderModule->shaderModule;
    fragmentShader.pName = "main";

    GraphicsPipelineCreateInfo graphicsPipelineCreateInfo;
    graphicsPipelineCreateInfo.stages = {vertexShader, fragmentShader};
    graphicsPipelineCreateInfo.inputAssemblyState = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
            .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
            .primitiveRestartEnable = VK_FALSE
    };

    VkViewport viewport{};
    viewport.width = (float) swapchain->imageExtent.width;
    viewport.height = (float) swapchain->imageExtent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor{};
    scissor.extent = swapchain->imageExtent;

    graphicsPipelineCreateInfo.viewportState = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
            .viewportCount = 1,
            .pViewports = &viewport,
            .scissorCount = 1,
            .pScissors = &scissor
    };

    graphicsPipelineCreateInfo.rasterizationState = {
            .sType =  VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
            .polygonMode = VK_POLYGON_MODE_LINE,
            .cullMode =VK_CULL_MODE_NONE,
            .frontFace = VK_FRONT_FACE_CLOCKWISE,
            .lineWidth = 10.0f,
    };

    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;

    graphicsPipelineCreateInfo.colorBlendState = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
            .attachmentCount = 1,
            .pAttachments = &colorBlendAttachment,
    };

    PipelineLayout pipelineLayout = vulkan.createPipelineLayout(device, {}, {});
    GraphicsPipeline graphicsPipeline = vulkan.createGraphicsPipeline(device, pipelineLayout, renderPass, 0, graphicsPipelineCreateInfo);

    //Commands
    CommandPool drawCommandPool = vulkan.createCommandPool(device, device->graphicsFamily, VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
    auto drawCommandBuffers = drawCommandPool->allocateCommandBuffers(VK_COMMAND_BUFFER_LEVEL_PRIMARY, swapchain->imageCount);

    //Synchronization
    std::vector<Semaphore> imageAvailableSemaphores(swapchain->imageCount);
    std::vector<Semaphore> renderFinishedSemaphores(swapchain->imageCount);
    std::vector<Fence> inFlightFences(swapchain->imageCount);

    for (uint32_t i = 0; i < swapchain->imageCount; ++i) {
        imageAvailableSemaphores[i] = vulkan.createSemaphore(device);
        renderFinishedSemaphores[i] = vulkan.createSemaphore(device);
        inFlightFences[i] = vulkan.createFence(device, true);
    }

    //Rendering
    uint32_t currentFrame = 0;
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        if (glfwGetWindowAttrib(window, GLFW_ICONIFIED)) continue;

        const auto begin = std::chrono::high_resolution_clock::now();

        inFlightFences[currentFrame]->wait();
        inFlightFences[currentFrame]->reset();

        uint32_t imageIndex = 0;
        vkAcquireNextImageKHR(device->device, swapchain->swapchain, UINT64_MAX, imageAvailableSemaphores[currentFrame]->semaphore, VK_NULL_HANDLE, &imageIndex);

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(drawCommandBuffers[currentFrame], &beginInfo) != VK_SUCCESS)
            throw std::runtime_error("failed to begin recording command buffer!");

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = renderPass->renderPass;
        renderPassInfo.framebuffer = framebuffers[imageIndex]->framebuffer;
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = swapchain->imageExtent;

        VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 0.0f}}};
        renderPassInfo.clearValueCount = 1;
        renderPassInfo.pClearValues = &clearColor;

        vkCmdBeginRenderPass(drawCommandBuffers[currentFrame], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        vkCmdBindPipeline(drawCommandBuffers[currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline->pipeline);

        vkCmdDraw(drawCommandBuffers[currentFrame], 3, 1, 0, 0);

        vkCmdEndRenderPass(drawCommandBuffers[currentFrame]);

        if (vkEndCommandBuffer(drawCommandBuffers[currentFrame]) != VK_SUCCESS)
            throw std::runtime_error("failed to record command buffer!");

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &drawCommandBuffers[currentFrame];

        //FIXME This is a bad approach and does not work for multiple subpasses
        VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = &imageAvailableSemaphores[currentFrame]->semaphore;
        submitInfo.pWaitDstStageMask = waitStages;
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = &renderFinishedSemaphores[currentFrame]->semaphore;

        if (vkQueueSubmit(device->graphicsQueues[0], 1, &submitInfo, inFlightFences[currentFrame]->fence) != VK_SUCCESS)
            throw std::runtime_error("failed to submit draw command buffer!");

        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = &swapchain->swapchain;
        presentInfo.pImageIndices = &imageIndex;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = &renderFinishedSemaphores[currentFrame]->semaphore;

        vkQueuePresentKHR(device->graphicsQueues[0], &presentInfo);
        currentFrame = (currentFrame + 1) % swapchain->imageCount;

        const uint32_t fps = 145;
        auto end = std::chrono::high_resolution_clock::now();

        auto counter =  (end-begin);
        while (counter < std::chrono::microseconds(1000000/fps)) {
            end = std::chrono::high_resolution_clock::now();
            counter = (end - begin);
        }
    }

    device->wait();

    //Destroy window surface
    vkDestroySurfaceKHR(vulkan.instance, surface, nullptr);

    glfwTerminate();
    return 0;
}

//TODO primitive restart enable can actually be very useful
