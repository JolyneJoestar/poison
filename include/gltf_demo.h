#pragma once

#include <vulkan/vulkan.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>
#include <stdlib.h>

class GLTFDemo
{
public:
	GLTFDemo();
	~GLTFDemo();

	void run();

private:
	void initVulkan();
	void mainLoop();
	void cleanUp();
	void initWindow();
	void createInstance();

private:
	GLFWwindow* m_window;
	VkInstance m_instance;
};
