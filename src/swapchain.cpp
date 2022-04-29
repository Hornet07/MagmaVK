#include "vulkan.h"

//Swapchain
Swapchain_T::Swapchain_T(VkDevice device, VkSurfaceKHR surface, SwapchainCreateInfo swapchainCreateInfo, size_t id) : device(device), surface(surface), swapchain(VK_NULL_HANDLE), /*imageCount(swapchainCreateInfo.minImageCount),*/ imageFormat(swapchainCreateInfo.imageFormat), imageExtent(swapchainCreateInfo.imageExtent), id(id) {
    swapchainCreateInfoCache.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchainCreateInfoCache.flags = swapchainCreateInfo.flags;
    swapchainCreateInfoCache.surface = surface;
    swapchainCreateInfoCache.minImageCount = swapchainCreateInfo.minImageCount;
    swapchainCreateInfoCache.imageFormat = swapchainCreateInfo.imageFormat;
    swapchainCreateInfoCache.imageColorSpace = swapchainCreateInfo.imageColorSpace;
    swapchainCreateInfoCache.imageExtent = swapchainCreateInfo.imageExtent;
    swapchainCreateInfoCache.imageArrayLayers = swapchainCreateInfo.imageArrayLayers;
    swapchainCreateInfoCache.imageUsage = swapchainCreateInfo.imageUsage;
    swapchainCreateInfoCache.imageSharingMode = swapchainCreateInfo.imageSharingMode;
    swapchainCreateInfoCache.queueFamilyIndexCount = swapchainCreateInfo.queueFamilyIndexCount;
    swapchainCreateInfoCache.pQueueFamilyIndices = swapchainCreateInfo.queueFamilyIndices;
    swapchainCreateInfoCache.preTransform = swapchainCreateInfo.preTransform;
    swapchainCreateInfoCache.compositeAlpha = swapchainCreateInfo.compositeAlpha;
    swapchainCreateInfoCache.presentMode = swapchainCreateInfo.presentMode;
    swapchainCreateInfoCache.clipped = swapchainCreateInfo.clipped;
    swapchainCreateInfoCache.oldSwapchain = swapchain;

    if (vkCreateSwapchainKHR(device, &swapchainCreateInfoCache, nullptr, &swapchain) != VK_SUCCESS)
        throw std::runtime_error("Failed to create swapchain");

    vkGetSwapchainImagesKHR(device, swapchain, &imageCount, nullptr);

    images.resize(imageCount);
    imageViews.resize(imageCount);

    vkGetSwapchainImagesKHR(device, swapchain, &imageCount, images.data());

    for (uint32_t i = 0; i < imageCount; ++i) {
        VkImageViewCreateInfo imageViewCreateInfo{};
        imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        imageViewCreateInfo.image = images[i];
        imageViewCreateInfo.viewType = swapchainCreateInfo.imageArrayLayers > 1 ? VK_IMAGE_VIEW_TYPE_2D_ARRAY : VK_IMAGE_VIEW_TYPE_2D;
        imageViewCreateInfo.format = imageFormat;
        imageViewCreateInfo.subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, swapchainCreateInfo.imageArrayLayers};

        if (vkCreateImageView(device, &imageViewCreateInfo, nullptr, &imageViews[i]) != VK_SUCCESS)
            throw std::runtime_error("Failed to create swapchain image view");
    }
}

Swapchain_T::~Swapchain_T() {
    for (uint32_t i = 0; i < imageCount; ++i)
        vkDestroyImageView(device, imageViews[i], nullptr);
    vkDestroySwapchainKHR(device, swapchain, nullptr);
}

void Swapchain_T::resize(VkExtent2D extent) {
    for (uint32_t i = 0; i < imageCount; ++i)
        vkDestroyImageView(device, imageViews[i], nullptr);

    imageExtent = extent;
    swapchainCreateInfoCache.imageExtent = imageExtent;
    swapchainCreateInfoCache.oldSwapchain = swapchain;

    if (vkCreateSwapchainKHR(device, &swapchainCreateInfoCache, nullptr, &swapchain) != VK_SUCCESS)
        throw std::runtime_error("Failed to create swapchain");

    uint32_t temp = imageCount;
    vkGetSwapchainImagesKHR(device, swapchain, &temp, images.data());

    for (uint32_t i = 0; i < imageCount; ++i) {
        VkImageViewCreateInfo imageViewCreateInfo{};
        imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        imageViewCreateInfo.image = images[i];
        imageViewCreateInfo.viewType = swapchainCreateInfoCache.imageArrayLayers > 1 ? VK_IMAGE_VIEW_TYPE_2D_ARRAY : VK_IMAGE_VIEW_TYPE_2D;
        imageViewCreateInfo.format = imageFormat;
        imageViewCreateInfo.subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, swapchainCreateInfoCache.imageArrayLayers};

        if (vkCreateImageView(device, &imageViewCreateInfo, nullptr, &imageViews[i]) != VK_SUCCESS)
            throw std::runtime_error("Failed to create swapchain image view");
    }
}

//TODO Implement
void Swapchain_T::recreate(SwapchainCreateInfo swapchainCreateInfo) {}

Swapchain Vulkan::createSwapchain(Device device, VkSurfaceKHR surface, SwapchainCreateInfo swapchainCreateInfo) {
    swapchains.emplace(std::piecewise_construct, std::forward_as_tuple(idCounter), std::forward_as_tuple(device->device, surface, swapchainCreateInfo, idCounter));
    return &swapchains.at(idCounter++);
}