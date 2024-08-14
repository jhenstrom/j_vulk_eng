#pragma once

#include "VDevice.hpp"
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include<glm/glm.hpp>
#include<vector>
#include<memory>


namespace vwdw {

	class VModel {
	public:

		struct Vertex {
			glm::vec3 pos{};
			glm::vec3 color{};
			glm::vec2 uv{};
			glm::vec3 normal{};
			
			static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
			static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();

			bool operator==(const Vertex& other) const {
				return pos == other.pos && color == other.color && uv == other.uv && normal == other.normal;
			}
		};

		struct Mesh {
			std::vector<Vertex> vertices{};
			std::vector<uint32_t> indices;

			void loadModel(const std::string& filePath);
		};

		VModel(VDevice &device, const VModel::Mesh& mesh);
		~VModel();

		VModel(const VModel&) = delete;
		VModel& operator=(const VModel&) = delete;

		void bind(VkCommandBuffer cBuffer);
		void draw(VkCommandBuffer cBuffer);

		static std::unique_ptr<VModel> createModelFromFile(VDevice& device, const std::string& filePath);


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