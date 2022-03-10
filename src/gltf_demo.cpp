#include "gltf_demo.h"


GLTFDemo::GLTFDemo()
{
	m_window = nullptr;
	m_vulkanRenderSystem = new rs::VulkanRenderSystem();
}

GLTFDemo::~GLTFDemo()
{
	delete m_vulkanRenderSystem;
}

void GLTFDemo::run()
{
	_initWindow();
	_initGraphicsContext();
	_mainLoop();
	
}


void GLTFDemo::_mainLoop()
{
	while (!glfwWindowShouldClose(m_window))
	{
		glfwPollEvents();
	}
}

void GLTFDemo::_cleanUp()
{
	m_vulkanRenderSystem->cleanUp();

	glfwDestroyWindow(m_window);

	glfwTerminate();
}

void GLTFDemo::_initWindow()
{
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	m_window = glfwCreateWindow(1200, 800, "gltf window", nullptr, nullptr);
}

void GLTFDemo::_initGraphicsContext()
{
	uint32_t glfwExtensionCount;
	const char** glfwExtension;

	glfwExtension = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	m_vulkanRenderSystem->setExtension(glfwExtension);
	m_vulkanRenderSystem->setExtensionCount(glfwExtensionCount);
	m_vulkanRenderSystem->setSurfaceHandle(m_window);
	m_vulkanRenderSystem->creatInstance();

	m_vulkanRenderSystem->pickPhysicalDevice();
	m_vulkanRenderSystem->createLogicalDevice();
	m_vulkanRenderSystem->createSwapChain();
	m_vulkanRenderSystem->createImageViews();
	m_vulkanRenderSystem->createRenderPass();
	m_vulkanRenderSystem->createGraphicsPipeline();
}
