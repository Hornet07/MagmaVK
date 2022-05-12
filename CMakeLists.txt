cmake_minimum_required(VERSION 3.21)
project(MagmaVK)

set(CMAKE_CXX_STANDARD 23)

find_package(glm)
find_package(glfw3)
find_package(Vulkan)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_executable(MagmaVK main.cpp src/vulkan.cpp src/vulkan.h utils.cpp utils.h
        src/device.cpp src/device.h
        src/swapchain.cpp src/swapchain.h
        src/renderpass.cpp src/renderpass.h
        src/pipeline.cpp src/pipeline.h
        src/framebuffer.cpp src/framebuffer.h
        src/commands.cpp src/commands.h
        src/synchronization.cpp src/synchronization.h)

target_link_libraries(MagmaVK glfw vulkan)