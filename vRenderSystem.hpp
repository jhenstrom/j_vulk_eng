#pragma once

#include "vGameObject.hpp"
#include "vwdw_pipeline.hpp"
#include "VDevice.hpp"
#include "model.hpp"
#include "KalGen2D.hpp"
#include "vCamera.hpp"

#include <memory>
#include <vector>

namespace vwdw {

	class VRenderSystem {

	public:

		VRenderSystem(VDevice &device, VkRenderPass renderPass);
		~VRenderSystem();

		VRenderSystem(const VRenderSystem&) = delete;
		VRenderSystem& operator=(const VRenderSystem&) = delete;

		void renderGameObjects(VkCommandBuffer commandBuffer, std::vector<vGameObject> &gameObjects, const VCamera &camera);

	private:
		void createPipelineLayout();
		void createPipeline(VkRenderPass renderPass);

		VDevice &vDevice;

		std::unique_ptr<VwdwPipeline> vPipeline;
		VkPipelineLayout pipelineLayout;

	};

}