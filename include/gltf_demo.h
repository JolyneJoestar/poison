#pragma once

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
	void _mainLoop();
	void _cleanUp();
	void _initWindow();
	void _initGraphicsContext();

private:
	GLFWwindow* m_window;
	rs::VulkanRenderSystem* m_vulkanRenderSystem;
};
