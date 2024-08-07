#pragma once

#define GLFW_INCLUDE_VULKAN
#include<GLFW/glfw3.h>

#include <string>

namespace vwdw {

class VWindow {


public:
	VWindow(int w, int h, std::string name);
	~VWindow();

	VWindow(const VWindow&) = delete;
	VWindow& operator=(const VWindow&) = delete;

	bool shouldClose() { return glfwWindowShouldClose(window); }
	VkExtent2D getExtent() {
		return { static_cast<uint32_t>(width), static_cast<uint32_t>(height) };
	}
	bool wasWindowResized() {
		return framebufferResized;
	}
	void resetWindowResizedFlag() {
		framebufferResized = false;
	}


	void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);

	GLFWwindow *getWindow() const { return window; }

private:
	static void framebufferResizeCallback(GLFWwindow* window, int width, int height);
	void initWindow();
	int width;
	int height;
	bool framebufferResized = false;

	std::string windowName;

	GLFWwindow* window;
};

}