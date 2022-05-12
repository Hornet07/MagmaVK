#pragma once
#include <vulkan/vulkan.h>

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