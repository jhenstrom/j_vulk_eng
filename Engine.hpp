#pragma once

#include "VWindow.hpp"
#include "vGameObject.hpp"
#include "vwdw_pipeline.hpp"
#include "VDevice.hpp"
#include "v_swap_chain.hpp"
#include "model.hpp"

#include <memory>
#include <vector>

namespace vwdw {

class Engine {

	public:
		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;

		Engine();
		~Engine();

		Engine(const Engine&) = delete;
		Engine& operator=(const Engine&) = delete;

		void run();
	private:
		void createPipelineLayout();
		void loadGameObjects();
		void createPipeline();
		void createCommandBuffers();
		void drawFrame();
		void freeCommandBuffers();
		void renderGameObjects(VkCommandBuffer commandBuffer);


		VWindow vWindow{ WIDTH, HEIGHT, "Vulkan_test" };
		VDevice vDevice{ vWindow };
		std::unique_ptr<VSwapChain> vSwapChain;
		//VwdwPipeline pipeline{vDevice, VwdwPipeline::defaultConfig(WIDTH, HEIGHT), "Shaders/simple_shader.vert.spv",  "Shaders/simple_shader.frag.spv" };
		std::unique_ptr<VwdwPipeline> vPipeline;
		VkPipelineLayout pipelineLayout;
		std::vector<VkCommandBuffer> commandBuffers;
		std::vector<vGameObject> vGameObjects;
		void recreateSwapChain();
		void recordCommandBuffer(int imageIndex);
};

}