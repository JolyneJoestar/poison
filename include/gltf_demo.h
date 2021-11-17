#pragma once

#include <vulkan/vulkan.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>
#include <stdlib.h>
#include "vulkan_render_system.h"

class GLTFDemo
{
public:
	GLTFDemo();
	~GLTFDemo();

	void run();

private:
	void mainLoop();
	void cleanUp();
	void initWindow();

private:
	GLFWwindow* m_window;
	rs::VulkanRenderSystem* m_vulkanRenderSystem;
};
