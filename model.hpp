#pragma once

#include "VDevice.hpp"
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include<glm/glm.hpp>
#include<vector>


namespace vwdw {

	class VModel {
	public:

		struct Vertex {
			glm::vec3 pos;
			glm::vec3 color;
			
			static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
			static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
		};

		struct Mesh {
			std::vector<Vertex> vertices{};
			std::vector<uint32_t> indices;
		};

		VModel(VDevice &device, const VModel::Mesh& mesh);
		~VModel();

		VModel(const VModel&) = delete;
		VModel& operator=(const VModel&) = delete;

		void bind(VkCommandBuffer cBuffer);
		void draw(VkCommandBuffer cBuffer);


	private:
		void createVertexBuffers(const std::vector<Vertex>& verts);
		void createIndexBuffers(const std::vector<uint32_t>& indices);

		VDevice &vDevice;

		VkBuffer vertexBuffer;
		VkDeviceMemory vBufferMem;
		uint32_t vertexCount;


		bool hasindexBuffer = false;
		VkBuffer indexBuffer;
		VkDeviceMemory iBufferMem;
		uint32_t indexCount;
	};

}