#pragma once

#include <string>
#include <vector>
#include "VDevice.hpp"

namespace vwdw {

	struct PipelineConfigInfo {

		//PipelineConfigInfo(const PipelineConfigInfo&) = delete; //dont know why this line breaks things. err is "attempting to reference a deleted function
		PipelineConfigInfo& operator=(const PipelineConfigInfo&) = delete;
		PipelineConfigInfo() = default;

		VkPipelineViewportStateCreateInfo viewportInfo;
		VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
		VkPipelineRasterizationStateCreateInfo rasterizationInfo;
		VkPipelineMultisampleStateCreateInfo multisampleInfo;
		VkPipelineColorBlendAttachmentState colorBlendAttachment;
		VkPipelineColorBlendStateCreateInfo colorBlendInfo;
		VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
		std::vector<VkDynamicState> dynamicStateEnables;
		VkPipelineDynamicStateCreateInfo dynamicStateInfo;
		VkPipelineLayout pipelineLayout = nullptr;
		VkRenderPass renderPass = nullptr;
		uint32_t subpass = 0;

	};

	class VwdwPipeline {
	public:
		VwdwPipeline(VDevice& device, const PipelineConfigInfo config, const std::string& vertPath, const std::string& fragPath);
		~VwdwPipeline();

		VwdwPipeline(const VwdwPipeline&) = delete;
		VwdwPipeline &operator=(const VwdwPipeline&) = delete;
		VwdwPipeline() = default;

		static void defaultConfig(PipelineConfigInfo &config);

		void bind(VkCommandBuffer commandbuffer);

	private:
		static std::vector<char> readFile(const std::string& path);

		void createGraphicsPipeline(const PipelineConfigInfo &config, const std::string& vertPath, const std::string& fragPath);

		void createShaderMod(const std::vector<char>& code, VkShaderModule* shaderMod);

		VDevice& vdevice;

		VkPipeline graphicsPipeline;
		VkShaderModule vertShaderMod;
		VkShaderModule fragShaderMod;

};

}