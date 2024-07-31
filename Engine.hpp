#pragma once

#include "VWindow.hpp"
#include "vGameObject.hpp"
#include "VDevice.hpp"
#include "vRenderer.hpp"
#include "model.hpp"
#include "KalGen2D.hpp"
#include "vRenderSystem.hpp"

#include <memory>
#include <vector>

namespace vwdw {

class Engine {

	public:
		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;

		Engine();
		~Engine();

		Engine(const Engine&) = delete;
		Engine& operator=(const Engine&) = delete;

		void run();
	private:
		void loadGameObjects();


		VWindow vWindow{ WIDTH, HEIGHT, "Vulkan_test" };
		VDevice vDevice{ vWindow };
		VRenderer vRenderer{ vWindow, vDevice };
		std::vector<vGameObject> vGameObjects;

		//toys
		KalGen2D* kalGen2D;
};

}