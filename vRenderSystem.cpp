#include "vRenderSystem.hpp"



#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include<glm/glm.hpp>
#include<glm/gtc/constants.hpp>

#include <stdexcept>
#include <array>
#include<cassert>

namespace vwdw {

	struct SimplePushConstantData {
		glm::mat4 transform{1.f};
		glm::mat4 normalMat{1.f};
	};


	VRenderSystem::VRenderSystem(VDevice &device, VkRenderPass renderpass) : vDevice{device}
	{
		createPipelineLayout();
		createPipeline(renderpass);
	}

	VRenderSystem::~VRenderSystem()
	{
		vkDestroyPipelineLayout(vDevice.device(), pipelineLayout, nullptr);
	}

	void VRenderSystem::createPipelineLayout()
	{

		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(SimplePushConstantData);


		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.pSetLayouts = nullptr;
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
		pipelineLayoutInfo.setLayoutCount = 0;
		pipelineLayoutInfo.pushConstantRangeCount = 1;

		if (vkCreatePipelineLayout(vDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create pipeline layout");
		}
	}
	void VRenderSystem::createPipeline(VkRenderPass renderPass)
	{
		assert(pipelineLayout != nullptr && "cannot create pipeline before pipeline layout");
		PipelineConfigInfo pipelineConfig{};
		VwdwPipeline::defaultConfig(pipelineConfig);
		pipelineConfig.renderPass = renderPass;
		pipelineConfig.pipelineLayout = pipelineLayout;
		vPipeline = std::make_unique<VwdwPipeline>(
			vDevice,
			pipelineConfig,
			"Shaders/simple_shader.vert.spv",
			"Shaders/simple_shader.frag.spv"
		);
	}

	void VRenderSystem::renderGameObjects(FrameInfo &FI, std::vector<vGameObject>& gameObjects)
	{
		vPipeline->bind(FI.commandBuffer);
		auto projectionView = FI.camera.getProjection() * FI.camera.getView();

		for (auto& obj : gameObjects)
		{
			SimplePushConstantData push{};
			auto modelmat = obj.transform.mat4();
			push.transform = projectionView * modelmat; //usually send both mat to shaders // also transforms to camera space
			push.normalMat = obj.transform.normalMat();
			vkCmdPushConstants(FI.commandBuffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(SimplePushConstantData), &push);
			obj.model->bind(FI.commandBuffer);
			obj.model->draw(FI.commandBuffer);
		}
	}


}