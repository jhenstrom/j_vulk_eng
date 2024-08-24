#pragma once

#include "vCamera.hpp"

#include <vulkan/vulkan.h>


namespace vwdw {

    struct FrameInfo {
        int frameIndex;
        VkCommandBuffer commandBuffer;
        VCamera& camera;
        float frameTime;
    };
}