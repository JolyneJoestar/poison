#include "gltf_demo.h"

GLTFDemo::GLTFDemo()
{
	m_window = nullptr;
	m_vulkanRenderSystem = new rs::VulkanRenderSystem;
}

GLTFDemo::~GLTFDemo()
{
	delete m_vulkanRenderSystem;
}

void GLTFDemo::run()
{
	initWindow();
	
	mainLoop();
	
}


void GLTFDemo::mainLoop()
{
	while (!glfwWindowShouldClose(m_window))
	{
		glfwPollEvents();
	}
}

void GLTFDemo::cleanUp()
{
	m_vulkanRenderSystem->cleanUp();

	glfwDestroyWindow(m_window);

	glfwTerminate();
}

void GLTFDemo::initWindow()
{
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	m_window = glfwCreateWindow(800, 600, "gltf window", nullptr, nullptr);

	VkApplicationInfo appInfo{};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "gltf window";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "No ENngine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	VkInstanceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	uint32_t glfwExtensionCount;
	const char** glfwExtension;

	glfwExtension = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	createInfo.enabledExtensionCount = glfwExtensionCount;
	createInfo.ppEnabledExtensionNames = glfwExtension;
	createInfo.enabledLayerCount = 0;

	m_vulkanRenderSystem->setInstanceCreatInfo(createInfo);
	m_vulkanRenderSystem->creatInstance();
}
