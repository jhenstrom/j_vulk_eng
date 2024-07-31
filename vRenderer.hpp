#pragma once

#include "VWindow.hpp"
#include "VDevice.hpp"
#include "v_swap_chain.hpp"
#include "model.hpp"
#include "KalGen2D.hpp"

#include <memory>
#include <vector>

namespace vwdw {

	class VRenderer {

	public:

		VRenderer(VWindow &vWindow, VDevice &vDevice);
		~VRenderer();

		VRenderer(const VRenderer&) = delete;
		VRenderer& operator=(const VRenderer&) = delete;

		VkRenderPass getSwapChainRenderPass() const { return vSwapChain->getRenderPass(); }
		bool isFrameInProgress() const { return isFrameStarted; }

		VkCommandBuffer getCurrCBuffer() { 
			assert(isFrameStarted && "cannot get cBuffer when frame is not in progress");
			return commandBuffers[currentFrameIndex]; }

		VkCommandBuffer beginFrame();
		void endFrame();
		void beginSwapChainRenderPass(VkCommandBuffer cBuffer);
		void endSwapChainRenderPass(VkCommandBuffer cBuffer);

		int getFrameIndex() const { assert(isFrameStarted && "cannot get frame index when frame is not in progress");
		return currentFrameIndex; }

	private:
		void createCommandBuffers();
		void freeCommandBuffers();
		void recreateSwapChain();


		VWindow &vWindow;
		VDevice &vDevice;
		std::unique_ptr<VSwapChain> vSwapChain;
		std::vector<VkCommandBuffer> commandBuffers;
		uint32_t currImageIndex;
		int currentFrameIndex;
		bool isFrameStarted;
	};

}