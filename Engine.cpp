#include "Engine.hpp"



#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include<glm/glm.hpp>
#include<glm/gtc/constants.hpp>

#include <stdexcept>
#include <array>
#include<cassert>

namespace vwdw {


Engine::Engine()
{
	//init toys
	kalGen2D = new KalGen2D(glm::vec2(0.01f), 0.005f, .025f);


	loadGameObjects();

}

Engine::~Engine()
{
}

void Engine::run() {
	
	//std::cout << "max push constants size: " << vDevice.properties.limits.maxPushConstantsSize() << std::endl;
	VRenderSystem vRenderSystem{ vDevice, vRenderer.getSwapChainRenderPass() };

	while (!vWindow.shouldClose()) {
		glfwPollEvents();
		if (auto commandBuffer = vRenderer.beginFrame())
		{
			

			vRenderer.beginSwapChainRenderPass(commandBuffer);
			vRenderSystem.renderGameObjects(commandBuffer, vGameObjects);
			vRenderer.endSwapChainRenderPass(commandBuffer);
			vRenderer.endFrame();
		}
	}

	vkDeviceWaitIdle(vDevice.device());
}

void Engine::loadGameObjects()
{
	std::vector<VModel::Vertex> verts{ {{0.0f,-0.5f}, {0.0f,0.0f,1.0f}}, {{0.5f,0.5f}, {1.0f,0.0f,0.0f}}, {{-0.5f, 0.5f}, {0.0f,1.0f,0.0f}} }; //replace with read in verts in future

	auto vModel = std::make_shared<VModel>(vDevice, verts);

	for (int i=300; i >= 0; i--)
	{
		vGameObjects.push_back(std::move(kalGen2D->next(vModel, i)));
	}
}

}