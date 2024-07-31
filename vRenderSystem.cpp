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
		glm::mat2 transform;
		glm::vec2 offset;
		alignas(16) glm::vec3 color;
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

	void VRenderSystem::renderGameObjects(VkCommandBuffer commandBuffer, std::vector<vGameObject>& gameObjects)
	{

		int i = 0;
		for (auto& obj : gameObjects)
		{
			i += 1;
			obj.transform.rotation = glm::mod(obj.transform.rotation + 0.00001f * i, 2.f * glm::pi<float>());
		}
		vPipeline->bind(commandBuffer);

		for (auto& obj : gameObjects)
		{
			SimplePushConstantData push{};
			push.offset = obj.transform.translation;
			push.color = obj.color;
			push.transform = obj.transform.mat2();
			vkCmdPushConstants(commandBuffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(SimplePushConstantData), &push);
			obj.model->bind(commandBuffer);
			obj.model->draw(commandBuffer);
		}
	}


}