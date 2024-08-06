#pragma once

#include "vwdw_pipeline.hpp"

#include "model.hpp"

#include<fstream>
#include<stdexcept>
#include<iostream>
#include <cassert>


namespace vwdw {

VwdwPipeline::VwdwPipeline(VDevice& device, const PipelineConfigInfo config, const std::string& vertPath, const std::string& fragPath) : vdevice{device}
{
	createGraphicsPipeline(config, vertPath, fragPath);
}

VwdwPipeline::~VwdwPipeline()
{
	vkDestroyShaderModule(vdevice.device(), vertShaderMod, nullptr);
	vkDestroyShaderModule(vdevice.device(), fragShaderMod, nullptr);
	vkDestroyPipeline(vdevice.device(), graphicsPipeline, nullptr);
}

std::vector<char> VwdwPipeline::readFile(const std::string& path)
{
	std::ifstream file(path, std::ios::ate | std::ios::binary);

	if (!file.is_open())
	{
		std::cout << path << '\n';
		throw std::runtime_error("failed to open file: " + path);
	}

	size_t fSize = static_cast<size_t>(file.tellg());

	std::vector<char> buffer(fSize);

	file.seekg(0);
	file.read(buffer.data(), fSize);

	file.close();
	return buffer;
}

void VwdwPipeline::createGraphicsPipeline(const PipelineConfigInfo &configInfo, const std::string& vertPath, const std::string& fragPath)
{

	assert(
		configInfo.pipelineLayout != VK_NULL_HANDLE &&
		"Cannot create graphics pipeline: no pipelineLayout provided in configInfo");
	assert(
		configInfo.renderPass != VK_NULL_HANDLE &&
		"Cannot create graphics pipeline: no renderPass provided in configInfo");

	auto vertCode = readFile(vertPath);
	auto fragCode = readFile(fragPath);

	auto bindingdesc = VModel::Vertex::getBindingDescriptions();
	auto attrdesc = VModel::Vertex::getAttributeDescriptions();
	VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attrdesc.size());
	vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(bindingdesc.size());
	vertexInputInfo.pVertexAttributeDescriptions = attrdesc.data(); //binding location offset format are the 4 pieces of info needed
	vertexInputInfo.pVertexBindingDescriptions = bindingdesc.data();

	createShaderMod(vertCode, &vertShaderMod);
	createShaderMod(fragCode, &fragShaderMod);




	VkPipelineShaderStageCreateInfo shaderStages[2];
	shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
	shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	shaderStages[0].pNext = nullptr;
	shaderStages[0].module = vertShaderMod;
	shaderStages[0].flags = 0;
	shaderStages[0].pName = "main";
	shaderStages[0].pSpecializationInfo = nullptr;
	shaderStages[1].module = fragShaderMod;
	shaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	shaderStages[1].pNext = nullptr;
	shaderStages[1].flags = 0;
	shaderStages[1].pSpecializationInfo = nullptr;
	shaderStages[1].pName = "main";


	VkGraphicsPipelineCreateInfo pipelineInfo{};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = 2;
	pipelineInfo.pStages = shaderStages;
	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pDynamicState = &configInfo.dynamicStateInfo;
	pipelineInfo.pDepthStencilState = &configInfo.depthStencilInfo;
	pipelineInfo.pInputAssemblyState = &configInfo.inputAssemblyInfo;
	pipelineInfo.pMultisampleState = &configInfo.multisampleInfo;
	pipelineInfo.pRasterizationState = &configInfo.rasterizationInfo;
	pipelineInfo.pColorBlendState = &configInfo.colorBlendInfo;
	pipelineInfo.pViewportState = &configInfo.viewportInfo;

	pipelineInfo.layout = configInfo.pipelineLayout;
	pipelineInfo.renderPass = configInfo.renderPass;
	pipelineInfo.subpass = configInfo.subpass;

	pipelineInfo.basePipelineIndex = -1;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

	if (vkCreateGraphicsPipelines(
		vdevice.device(),
		VK_NULL_HANDLE,
		1,
		&pipelineInfo,
		nullptr,
		&graphicsPipeline) != VK_SUCCESS) {
		throw std::runtime_error("failed to create graphics pipeline");
	}

}

void VwdwPipeline::createShaderMod(const std::vector<char>& code, VkShaderModule* shaderMod)
{
	VkShaderModuleCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data()); // only works for Vectors, will be invalid cast with C arrays

	if (vkCreateShaderModule(vdevice.device(), &createInfo, nullptr, shaderMod) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create Shader Module");
	}
}

void VwdwPipeline::defaultConfig(PipelineConfigInfo &configInfo)
{

	configInfo.inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;

	// reference for possible topogoies:

	//typedef enum VkPrimitiveTopology {
	//	VK_PRIMITIVE_TOPOLOGY_POINT_LIST = 0,
	//	VK_PRIMITIVE_TOPOLOGY_LINE_LIST = 1,
	//	VK_PRIMITIVE_TOPOLOGY_LINE_STRIP = 2,
	//	VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST = 3,
	//	VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP = 4,
	//	VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN = 5,
	//	VK_PRIMITIVE_TOPOLOGY_LINE_LIST_WITH_ADJACENCY = 6,
	//	VK_PRIMITIVE_TOPOLOGY_LINE_STRIP_WITH_ADJACENCY = 7,
	//	VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST_WITH_ADJACENCY = 8,
	//	VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP_WITH_ADJACENCY = 9,
	//	VK_PRIMITIVE_TOPOLOGY_PATCH_LIST = 10,
	//} VkPrimitiveTopology;
	configInfo.inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	configInfo.inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

	configInfo.viewportInfo.viewportCount = 1;
	configInfo.viewportInfo.scissorCount = 1;
	configInfo.viewportInfo.pScissors = nullptr;
	configInfo.viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	configInfo.viewportInfo.pViewports = nullptr;

	configInfo.rasterizationInfo.rasterizerDiscardEnable = VK_FALSE;
	configInfo.rasterizationInfo.depthBiasSlopeFactor = 0.0f;
	configInfo.rasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL;
	configInfo.rasterizationInfo.depthBiasEnable = VK_FALSE;
	configInfo.rasterizationInfo.depthClampEnable = VK_FALSE;
	configInfo.rasterizationInfo.lineWidth = 1.0f;
	configInfo.rasterizationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	configInfo.rasterizationInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
	configInfo.rasterizationInfo.depthBiasConstantFactor = 0.0f;
	configInfo.rasterizationInfo.depthBiasClamp = 0.0f;
	configInfo.rasterizationInfo.cullMode = VK_CULL_MODE_NONE; // come back for performance gains via back-facing triangle culling

	configInfo.multisampleInfo.alphaToOneEnable = VK_FALSE;
	configInfo.multisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	configInfo.multisampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	configInfo.multisampleInfo.sampleShadingEnable = VK_FALSE;
	configInfo.multisampleInfo.pSampleMask = nullptr;
	configInfo.multisampleInfo.alphaToCoverageEnable = VK_FALSE;
	configInfo.multisampleInfo.minSampleShading = 1.0f;

	configInfo.colorBlendAttachment.colorWriteMask =
		VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |
		VK_COLOR_COMPONENT_A_BIT;
	configInfo.colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
	configInfo.colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
	configInfo.colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
	configInfo.colorBlendAttachment.blendEnable = VK_FALSE;
	configInfo.colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	configInfo.colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	configInfo.colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;

	configInfo.colorBlendInfo.logicOpEnable = VK_FALSE;
	configInfo.colorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	configInfo.colorBlendInfo.logicOp = VK_LOGIC_OP_COPY;
	configInfo.colorBlendInfo.attachmentCount = 1;
	configInfo.colorBlendInfo.pAttachments = &configInfo.colorBlendAttachment;
	configInfo.colorBlendInfo.blendConstants[0] = 0.0f;
	configInfo.colorBlendInfo.blendConstants[1] = 0.0f;
	configInfo.colorBlendInfo.blendConstants[2] = 0.0f;
	configInfo.colorBlendInfo.blendConstants[3] = 0.0f;

	configInfo.depthStencilInfo.depthTestEnable = VK_TRUE;
	configInfo.depthStencilInfo.stencilTestEnable = VK_FALSE;
	configInfo.depthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	configInfo.depthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
	//configInfo.depthStencilInfo.depthCompareOp = VK_COMPARE_OP_GREATER_OR_EQUAL;
	configInfo.depthStencilInfo.depthBoundsTestEnable = VK_FALSE;
	configInfo.depthStencilInfo.depthWriteEnable = VK_TRUE;
	configInfo.depthStencilInfo.maxDepthBounds = 1.0f;
	configInfo.depthStencilInfo.minDepthBounds = 0.0f;
	configInfo.depthStencilInfo.front = {};
	configInfo.depthStencilInfo.back = {};

	configInfo.dynamicStateEnables = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
	configInfo.dynamicStateInfo.pDynamicStates = configInfo.dynamicStateEnables.data();
	configInfo.dynamicStateInfo.dynamicStateCount = static_cast<uint32_t>(configInfo.dynamicStateEnables.size());
	configInfo.dynamicStateInfo.flags = 0;
	configInfo.dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;

}

void VwdwPipeline::bind(VkCommandBuffer commandbuffer)
{
	vkCmdBindPipeline(commandbuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

}

}