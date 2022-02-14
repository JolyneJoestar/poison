#include "gltf_demo.h"


GLTFDemo::GLTFDemo()
{
	m_window = nullptr;
	m_vulkanRenderSystem = nullptr;
}

GLTFDemo::~GLTFDemo()
{
	_cleanUp();
}

void GLTFDemo::run()
{
	_initWindow();
	_initGraphicsContext();
	_mainLoop();
	
}


void GLTFDemo::_drawFrame()
{

}

void GLTFDemo::_mainLoop()
{
	while (!glfwWindowShouldClose(m_window))
	{
		glfwPollEvents();
		_drawFrame();
	}
}

void GLTFDemo::_cleanUp()
{
	delete m_vulkanRenderSystem;

	glfwDestroyWindow(m_window);

	glfwTerminate();
}

void GLTFDemo::_initWindow()
{
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	if (m_window == nullptr)
	{
		m_window = glfwCreateWindow(rs::WIDTH, rs::HEIGHT, "gltf window", nullptr, nullptr);
	}
	if (m_vulkanRenderSystem == nullptr)
	{
		m_vulkanRenderSystem = new rs::VulkanRenderSystem();
	}
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
	//m_vulkanRenderSystem->createFramebuffers();
}
