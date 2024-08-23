#include "model.hpp"
#include "v_utils.hpp"
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>


#include<cassert>
#include<cstring>
#include<iostream>

namespace std {
	template<> struct hash<vwdw::VModel::Vertex> {
		size_t operator()(vwdw::VModel::Vertex const& vertex) const {
			size_t seed = 0;
			vwdw::hash_combine(seed, vertex.pos, vertex.color, vertex.uv, vertex.normal);
			return seed;
		}
	};
}


namespace vwdw {

VModel::VModel(VDevice& device, const VModel::Mesh& mesh): vDevice{device}
{
	createVertexBuffers(mesh.vertices);
	createIndexBuffers(mesh.indices);
}

VModel::~VModel()
{}

std::unique_ptr<VModel> VModel::createModelFromFile(VDevice &device, const std::string& filePath)
{
	Mesh mesh{};
	mesh.loadModel(filePath);
	std::cout << "Vertex count: " << mesh.vertices.size() << std::endl;
	return std::make_unique<VModel>(device, mesh);
}

void VModel::Mesh::loadModel(const std::string& filePath)
{
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warn, err;

	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filePath.c_str()))
	{
		throw std::runtime_error(warn + err);
	}

	vertices.clear();
	indices.clear();

	std::unordered_map<Vertex, uint32_t> uniqueVertices{};

	for (const auto& shape : shapes)
	{
		for (const auto& index : shape.mesh.indices)
		{
			Vertex vertex{};
			
			if(index.vertex_index >= 0){
				vertex.pos = {
					attrib.vertices[3 * index.vertex_index + 0],
					attrib.vertices[3 * index.vertex_index + 1],
					attrib.vertices[3 * index.vertex_index + 2]
				};


				vertex.color = {
					attrib.colors[3 * index.vertex_index + 0],
					attrib.colors[3 * index.vertex_index + 1],
					attrib.colors[3 * index.vertex_index + 2]
				};
			}

			if(index.normal_index >= 0){
				vertex.normal = {
					attrib.normals[3 * index.normal_index + 0],
					attrib.normals[3 * index.normal_index + 1],
					attrib.normals[3 * index.normal_index + 2]
				};
			}

			if(index.texcoord_index >= 0){
				vertex.uv = {
					attrib.texcoords[2 * index.texcoord_index + 0],
					attrib.texcoords[2 * index.texcoord_index + 1]
				};
			}

			if(uniqueVertices.count(vertex) == 0)
			{
				uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
				vertices.push_back(vertex);
			}
			indices.push_back(uniqueVertices[vertex]);
		}
	}
}


void VModel::bind(VkCommandBuffer cBuffer)
{
	VkBuffer buffers[] = { vertexBuffer->getBuffer() };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(cBuffer, 0, 1, buffers, offsets);
	if (hasindexBuffer)
	{
		vkCmdBindIndexBuffer(cBuffer, indexBuffer->getBuffer(), 0, VK_INDEX_TYPE_UINT32);
	}
}

void VModel::draw(VkCommandBuffer cBuffer)
{
	if (hasindexBuffer)
	{
		vkCmdDrawIndexed(cBuffer, indexCount, 1, 0, 0, 0);
	}
	else {
		vkCmdDraw(cBuffer, vertexCount, 1, 0, 0);
	}
}

void VModel::createVertexBuffers(const std::vector<Vertex>& verts)
{
	vertexCount = static_cast<uint32_t>(verts.size());

	assert(vertexCount >= 3 && "vertex count must be atleast 3");
	VkDeviceSize bufferSize = sizeof(verts[0])*vertexCount;
	uint32_t vertexSize = sizeof(verts[0]);

	VBuffer stgBuffer = VBuffer(vDevice, vertexSize, vertexCount, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	stgBuffer.map();
	stgBuffer.writeToBuffer((void *)verts.data());

	vertexBuffer = std::make_unique<VBuffer>(vDevice, vertexSize, vertexCount, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	

	vDevice.copyBuffer(stgBuffer.getBuffer(), vertexBuffer->getBuffer(), bufferSize);
}

void VModel::createIndexBuffers(const std::vector<uint32_t>& indices)
{
	indexCount = static_cast<uint32_t>(indices.size());
	hasindexBuffer = indexCount > 0;

	if (!hasindexBuffer) return;

	VkDeviceSize bufferSize = sizeof(indices[0]) * indexCount;
	uint32_t indexSize = sizeof(indices[0]);

	VBuffer stgBuffer = VBuffer(vDevice, indexSize, indexCount, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	stgBuffer.map();
	stgBuffer.writeToBuffer((void *)indices.data());

	indexBuffer = std::make_unique<VBuffer>(vDevice, indexSize, indexCount, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	vDevice.copyBuffer(stgBuffer.getBuffer(), indexBuffer->getBuffer(), bufferSize);
}

std::vector<VkVertexInputBindingDescription> VModel::Vertex::getBindingDescriptions()
{
	std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
	bindingDescriptions[0].stride = sizeof(Vertex);
	bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	bindingDescriptions[0].binding = 0;
	return bindingDescriptions;
}

std::vector<VkVertexInputAttributeDescription> VModel::Vertex::getAttributeDescriptions()
{
	std::vector<VkVertexInputAttributeDescription> attrDescriptions{};

	attrDescriptions.push_back({0,0,VK_FORMAT_R32G32B32_SFLOAT,offsetof(Vertex, pos)});
	attrDescriptions.push_back({1,0,VK_FORMAT_R32G32B32_SFLOAT,offsetof(Vertex, color)});
	attrDescriptions.push_back({2,0,VK_FORMAT_R32G32B32_SFLOAT,offsetof(Vertex, normal)});
	attrDescriptions.push_back({3,0,VK_FORMAT_R32G32_SFLOAT,offsetof(Vertex, uv)});
	return attrDescriptions;
}

}