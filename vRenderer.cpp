#include "vRenderer.hpp"


#include <stdexcept>
#include <array>
#include<cassert>

namespace vwdw {


	VRenderer::VRenderer(VWindow &window, VDevice &device): vDevice{device}, vWindow{window}
	{
		recreateSwapChain();
		createCommandBuffers();

	}

	VRenderer::~VRenderer()
	{
		freeCommandBuffers();
	}

	void VRenderer::createCommandBuffers()
	{
		commandBuffers.resize(VSwapChain::MAX_FRAMES_IN_FLIGHT);
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = vDevice.getCommandPool();
		allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

		if (vkAllocateCommandBuffers(vDevice.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create command buffers!");
		}
	}

	void VRenderer::recreateSwapChain()
	{
		auto extent = vWindow.getExtent();
		while (extent.width == 0 || extent.height == 0)
		{
			extent = vWindow.getExtent();
			glfwWaitEvents();
		}

		vkDeviceWaitIdle(vDevice.device());
		if (vSwapChain == nullptr)
		{
			vSwapChain = std::make_unique<VSwapChain>(vDevice, extent);
		}
		else
		{
			std::shared_ptr<VSwapChain> oldSwapChain = std::move(vSwapChain);
			vSwapChain = std::make_unique<VSwapChain>(vDevice, extent, oldSwapChain);

			if(!oldSwapChain->compareSwapFormats(*vSwapChain.get()))
			{
				throw std::runtime_error("Swap chain image format has changed!");
			}

		}


	}

	void VRenderer::freeCommandBuffers()
	{
		vkFreeCommandBuffers(vDevice.device(), vDevice.getCommandPool(), static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());
		commandBuffers.clear();
	}

	VkCommandBuffer VRenderer::beginFrame()
	{
		assert(!isFrameStarted && "cant call begin frame while already in progress");

		auto result = vSwapChain->acquireNextImage(&currImageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			recreateSwapChain();
			return nullptr;
		}
		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
		{
			throw std::runtime_error("failed to aquire swapchain image");
		}
		isFrameStarted = true;

		auto cBuffer = getCurrCBuffer();

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(cBuffer, &beginInfo) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to begin recording command buffer");
		}

		return cBuffer;

	}

	void VRenderer::endFrame()
	{
		assert(isFrameStarted && "cant call end frame while not in progress");
		auto cBuffer = getCurrCBuffer();
		if (vkEndCommandBuffer(cBuffer) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to record command buffer");
		}
		auto result = vSwapChain->submitCommandBuffers(&cBuffer, &currImageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || vWindow.wasWindowResized())
		{
			vWindow.resetWindowResizedFlag();
			recreateSwapChain();
		}
		else if (result != VK_SUCCESS)
		{
			throw std::runtime_error("failed to present swap chain image");
		}

		isFrameStarted = false;
		currentFrameIndex = (currentFrameIndex + 1) % VSwapChain::MAX_FRAMES_IN_FLIGHT;
	}

	void VRenderer::beginSwapChainRenderPass(VkCommandBuffer cBuffer)
	{
		assert(isFrameStarted && "cant call begin swap chain render pass while not in progress");
		assert(cBuffer == getCurrCBuffer() && "cant start a renderpass on a different frame");
		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.renderPass = vSwapChain->getRenderPass();
		renderPassInfo.framebuffer = vSwapChain->getFrameBuffer(currImageIndex);
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;

		renderPassInfo.renderArea.extent = vSwapChain->getSwapChainExtent();
		renderPassInfo.renderArea.offset = { 0, 0 };

		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { 0.01f, 0.01f, 0.01f, 1.0f };
		clearValues[1].depthStencil = { 1.0f, 0 };

		renderPassInfo.pClearValues = clearValues.data();
		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());

		VkViewport viewport{};
		viewport.y = 0.0f;
		viewport.x = 0.0f;
		viewport.height = static_cast<float>(vSwapChain->height());
		viewport.width = static_cast<float>(vSwapChain->width());
		viewport.maxDepth = 1.0f;
		viewport.minDepth = 0.0f;
		VkRect2D scissor{ {0,0}, vSwapChain->getSwapChainExtent() };
		vkCmdSetViewport(cBuffer, 0, 1, &viewport);
		vkCmdSetScissor(cBuffer, 0, 1, &scissor);

		vkCmdBeginRenderPass(cBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
	}

	void VRenderer::endSwapChainRenderPass(VkCommandBuffer cBuffer)
	{
		assert(isFrameStarted && "cant call end swap chain render pass while not in progress");
		assert(cBuffer == getCurrCBuffer() && "cant end a renderpass on a different frame");
		vkCmdEndRenderPass(cBuffer);
	}

}