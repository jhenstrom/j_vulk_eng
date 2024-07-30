#include "Engine.hpp"

#include <stdexcept>
#include <array>
#include<cassert>

namespace vwdw {


Engine::Engine()
{
	loadModels();
	createPipelineLayout();
	recreateSwapChain();
	createCommandBuffers();
}

Engine::~Engine()
{
	vkDestroyPipelineLayout(vDevice.device(), pipelineLayout, nullptr);
}

void Engine::run() {
	while (!vWindow.shouldClose()) {
		glfwPollEvents();
		drawFrame();
	}

	vkDeviceWaitIdle(vDevice.device());
}

void Engine::createPipelineLayout()
{
	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.pSetLayouts = nullptr;
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.pPushConstantRanges = nullptr;
	pipelineLayoutInfo.setLayoutCount = 0;
	pipelineLayoutInfo.pushConstantRangeCount = 0;

	if (vkCreatePipelineLayout(vDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create pipeline layout");
	}
}
void Engine::createPipeline()
{
	assert(vSwapChain != nullptr && "cannot create pipeline before swapchain");
	assert(pipelineLayout != nullptr && "cannot create pipeline before pipeline layout");
	PipelineConfigInfo pipelineConfig{};
	VwdwPipeline::defaultConfig(pipelineConfig);
	pipelineConfig.renderPass = vSwapChain->getRenderPass();
	pipelineConfig.pipelineLayout = pipelineLayout;
	vPipeline = std::make_unique<VwdwPipeline>(
		vDevice,
		pipelineConfig,
		"Shaders/simple_shader.vert.spv",
		"Shaders/simple_shader.frag.spv"
	);
}

void Engine::createCommandBuffers()
{
	commandBuffers.resize(vSwapChain->imageCount());
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

void Engine::recreateSwapChain()
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
		vSwapChain = std::make_unique<VSwapChain>(vDevice, extent, std::move(vSwapChain));
		if(vSwapChain->imageCount() != commandBuffers.size())
		{
			freeCommandBuffers();
			createCommandBuffers();
		}
	}

	//if renderpass is compatible, you can reues the pipelien
	createPipeline();
}

void Engine::recordCommandBuffer(int imageIndex)
{
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

	if (vkBeginCommandBuffer(commandBuffers[imageIndex], &beginInfo) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to begin recording command buffer");
	}

	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.renderPass = vSwapChain->getRenderPass();
	renderPassInfo.framebuffer = vSwapChain->getFrameBuffer(imageIndex);
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
	VkRect2D scissor{{0,0}, vSwapChain->getSwapChainExtent()};
	vkCmdSetViewport(commandBuffers[imageIndex], 0, 1, &viewport);
	vkCmdSetScissor(commandBuffers[imageIndex], 0, 1, &scissor);

	vkCmdBeginRenderPass(commandBuffers[imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

	vPipeline->bind(commandBuffers[imageIndex]);

	vModel->bind(commandBuffers[imageIndex]);

	vModel->draw(commandBuffers[imageIndex]);

	vkCmdEndRenderPass(commandBuffers[imageIndex]);

	if (vkEndCommandBuffer(commandBuffers[imageIndex]) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to record command buffer");
	}
}

void Engine::drawFrame()
{
	uint32_t imageIndex;
	auto result = vSwapChain->acquireNextImage(&imageIndex);

	if(result == VK_ERROR_OUT_OF_DATE_KHR)
	{
		recreateSwapChain();
		return;
	}
	if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
	{
		throw std::runtime_error("failed to aquire swapchain image");
	}

	recordCommandBuffer(imageIndex);

	result = vSwapChain->submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);

	if(result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || vWindow.wasWindowResized())
	{
		vWindow.resetWindowResizedFlag();
		recreateSwapChain();
		return;
	}

	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("failed to present swap chain image");
	}

}

void Engine::loadModels()
{
	std::vector<VModel::Vertex> verts{ {{0.0f,-0.5f}, {0.0f,0.0f,1.0f}}, {{0.5f,0.5f}, {1.0f,0.0f,0.0f}}, {{-0.5f, 0.5f}, {0.0f,1.0f,0.0f}} };
	vModel = std::make_unique<VModel>(vDevice, verts);
}

void Engine::freeCommandBuffers()
{
	vkFreeCommandBuffers(vDevice.device(), vDevice.getCommandPool(), static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());
	commandBuffers.clear();
}

}