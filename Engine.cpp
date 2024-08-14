#include "Engine.hpp"
#include "vCamera.hpp"
#include "keyboardController.hpp"



#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include<glm/glm.hpp>
#include<glm/gtc/constants.hpp>

#include <stdexcept>
#include <chrono>
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
    //vCamera.setViewTarget(glm::vec3(-1.f, -2.f, 2.f), glm::vec3(0.f, 0.f, 2.5f));

    auto viewerObj = vGameObject::createGameObject();
    KeyboardController cameraController{};

    auto currentTime = std::chrono::high_resolution_clock::now();

	while (!vWindow.shouldClose()) {
		glfwPollEvents();

        auto newTime = std::chrono::high_resolution_clock::now();
        float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
        currentTime = newTime;

        cameraController.moveInPlaneXZ(vWindow.getWindow(), frameTime, viewerObj);
        vCamera.setViewXYZ(viewerObj.transform.translation, viewerObj.transform.rotation);

	    float aspect = vRenderer.getAspectRatio();

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

void Engine::loadGameObjects()
{

	// std::shared_ptr<VModel> cubeModel = createCubeModel(vDevice, {0.f, 0.f, 0.f});
  std::shared_ptr<VModel> model = VModel::createModelFromFile(vDevice, "models/smooth_vase.obj");
	auto cube = vGameObject::createGameObject();
	cube.model = model;
	cube.transform.translation = {0.2f, 0.f, 2.5f};
    cube.transform.scale = {1.f, .5f, .5f};
	vGameObjects.push_back(std::move(cube));

	std::shared_ptr<VModel> model2 = VModel::createModelFromFile(vDevice, "models/flat_vase.obj");
	auto cube2 = vGameObject::createGameObject();
	cube2.model = model2;
	cube2.transform.translation = { -0.2f, 0.f, 2.5f };
	cube2.transform.scale = { 1.f, .5f, .5f };
	vGameObjects.push_back(std::move(cube2));

}

}