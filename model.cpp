#include "model.hpp"
#include<cassert>
#include<cstring>


namespace vwdw {

VModel::VModel(VDevice& device, const VModel::Mesh& mesh): vDevice{device}
{
	createVertexBuffers(mesh.vertices);
	createIndexBuffers(mesh.indices);
}

VModel::~VModel()
{
	vkDestroyBuffer(vDevice.device(), vertexBuffer, nullptr);
	vkFreeMemory(vDevice.device(), vBufferMem, nullptr);

	if (hasindexBuffer)
	{
		vkDestroyBuffer(vDevice.device(), indexBuffer, nullptr);
		vkFreeMemory(vDevice.device(), iBufferMem, nullptr);
	}

}


void VModel::bind(VkCommandBuffer cBuffer)
{
	VkBuffer buffers[] = { vertexBuffer };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(cBuffer, 0, 1, buffers, offsets);
	if (hasindexBuffer)
	{
		vkCmdBindIndexBuffer(cBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT32);
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

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	vDevice.createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	void *data;
	vkMapMemory(vDevice.device(), stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, verts.data(), static_cast<size_t>(bufferSize));
	vkUnmapMemory(vDevice.device(), stagingBufferMemory);


	vDevice.createBuffer(bufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer, vBufferMem);

	vDevice.copyBuffer(stagingBuffer, vertexBuffer, bufferSize);
	vkDestroyBuffer(vDevice.device(), stagingBuffer, nullptr);
	vkFreeMemory(vDevice.device(), stagingBufferMemory, nullptr);
}

void VModel::createIndexBuffers(const std::vector<uint32_t>& indices)
{
	indexCount = static_cast<uint32_t>(indices.size());
	hasindexBuffer = indexCount > 0;

	if (!hasindexBuffer) return;

	VkDeviceSize bufferSize = sizeof(indices[0]) * indexCount;
	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	vDevice.createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	void *data;
	vkMapMemory(vDevice.device(), stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, indices.data(), static_cast<size_t>(bufferSize));
	vkUnmapMemory(vDevice.device(), stagingBufferMemory);


	vDevice.createBuffer(bufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffer, iBufferMem);

	vDevice.copyBuffer(stagingBuffer, indexBuffer, bufferSize);
	vkDestroyBuffer(vDevice.device(), stagingBuffer, nullptr);
	vkFreeMemory(vDevice.device(), stagingBufferMemory, nullptr);
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
	std::vector<VkVertexInputAttributeDescription> attrDescriptions(2);
	attrDescriptions[0].location = 0;
	attrDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
	attrDescriptions[0].binding = 0;
	attrDescriptions[0].offset = offsetof(Vertex, pos);

	attrDescriptions[1].offset = offsetof(Vertex,color);
	attrDescriptions[1].location = 1;
	attrDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
	attrDescriptions[1].binding = 0;
	return attrDescriptions;
}

}