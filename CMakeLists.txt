cmake_minimum_required(VERSION 3.21)
project(MagmaVK)

set(CMAKE_CXX_STANDARD 23)

find_package(glm)
find_package(glfw3)
find_package(Vulkan)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_executable(MagmaVK main.cpp src/vulkan.cpp src/vulkan.h utils.cpp utils.h
        src/device.cpp src/swapchain.cpp src/renderpass.cpp src/pipeline.cpp src/framebuffer.cpp src/commands.cpp src/synchronization.cpp)

target_link_libraries(MagmaVK glfw vulkan)