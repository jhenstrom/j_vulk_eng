#include "Engine.hpp"
#include "vCamera.hpp"



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
	// kalGen2D = new KalGen2D(glm::vec2(0.01f), 0.005f, .025f); //kalidascope for 2d


	loadGameObjects();

}

Engine::~Engine()
{
}

void Engine::run() {
	
	//std::cout << "max push constants size: " << vDevice.properties.limits.maxPushConstantsSize() << std::endl;
	VRenderSystem vRenderSystem{ vDevice, vRenderer.getSwapChainRenderPass() };
    VCamera vCamera{};
    // vCamera.setView(glm::vec3(0.f), glm::vec3(0.5f, 0.f, 1.f));
    vCamera.setViewTarget(glm::vec3(-1.f, -2.f, 2.f), glm::vec3(0.f, 0.f, 2.5f));

	while (!vWindow.shouldClose()) {
		glfwPollEvents();
		float aspect = vRenderer.getAspectRatio();

    //vCamera.setOrthographicProjection(-aspect, aspect, -1.f, 1.f, -1.f, 1.f);
    vCamera.setPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 10.f);
    if (auto commandBuffer = vRenderer.beginFrame())
		{
			

			vRenderer.beginSwapChainRenderPass(commandBuffer);
			vRenderSystem.renderGameObjects(commandBuffer, vGameObjects, vCamera);
			vRenderer.endSwapChainRenderPass(commandBuffer);
			vRenderer.endFrame();
		}
	}

	vkDeviceWaitIdle(vDevice.device());
}

std::unique_ptr<VModel> createCubeModel(VDevice& device, glm::vec3 offset) {
  std::vector<VModel::Vertex> vertices{
 
      // left face (white)
      {{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
      {{-.5f, .5f, .5f}, {.9f, .9f, .9f}},
      {{-.5f, -.5f, .5f}, {.9f, .9f, .9f}},
      {{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
      {{-.5f, .5f, -.5f}, {.9f, .9f, .9f}},
      {{-.5f, .5f, .5f}, {.9f, .9f, .9f}},

      // right face (yellow)
      {{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
      {{.5f, .5f, .5f}, {.8f, .8f, .1f}},
      {{.5f, -.5f, .5f}, {.8f, .8f, .1f}},
      {{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
      {{.5f, .5f, -.5f}, {.8f, .8f, .1f}},
      {{.5f, .5f, .5f}, {.8f, .8f, .1f}},

      // top face (orange, remember y axis points down)
      {{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
      {{.5f, -.5f, .5f}, {.9f, .6f, .1f}},
      {{-.5f, -.5f, .5f}, {.9f, .6f, .1f}},
      {{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
      {{.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
      {{.5f, -.5f, .5f}, {.9f, .6f, .1f}},

      // bottom face (red)
      {{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
      {{.5f, .5f, .5f}, {.8f, .1f, .1f}},
      {{-.5f, .5f, .5f}, {.8f, .1f, .1f}},
      {{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
      {{.5f, .5f, -.5f}, {.8f, .1f, .1f}},
      {{.5f, .5f, .5f}, {.8f, .1f, .1f}},

      // nose face (blue)
      {{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
      {{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
      {{-.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
      {{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
      {{.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
      {{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},

      // tail face (green)
      {{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
      {{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
      {{-.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
      {{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
      {{.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
      {{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},

  };
  for (auto& v : vertices) {
    v.pos += offset;
  }
  return std::make_unique<VModel>(device, vertices);
}

void Engine::loadGameObjects()
{

	std::shared_ptr<VModel> cubeModel = createCubeModel(vDevice, {0.f, 0.f, 0.f});
	auto cube = vGameObject::createGameObject();
	cube.model = cubeModel;
	cube.transform.translation = {0.f, 0.f, 2.5f};
    cube.transform.scale = {.5f, .5f, .5f};
	vGameObjects.push_back(std::move(cube));


	//Kalidascope for 2d
	// std::vector<VModel::Vertex> verts{ {{0.0f,-0.5f}, {0.0f,0.0f,1.0f}}, {{0.5f,0.5f}, {1.0f,0.0f,0.0f}}, {{-0.5f, 0.5f}, {0.0f,1.0f,0.0f}} }; //replace with read in verts in future

	// auto vModel = std::make_shared<VModel>(vDevice, verts);

	// for (int i=300; i >= 0; i--)
	// {
	// 	vGameObjects.push_back(std::move(kalGen2D->next(vModel, i)));
	// }
}

}