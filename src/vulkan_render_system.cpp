#include "vulkan_render_system.h"
#include <iostream>
#include <map>
#include <set>

namespace rs
{
	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT * pCallbackData,
		void* pUserData
	)
	{
		if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
		{
			std::cerr << "validation layer:" << pCallbackData->pMessage << std::endl;
			return VK_FALSE;
		}
		else
		{
			return VK_TRUE;
		}
	}
	void VulkanRenderSystem::_initVulkan()
	{
		m_appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		m_appInfo.pApplicationName = "gltf window";
		m_appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		m_appInfo.pEngineName = "No ENngine";
		m_appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		m_appInfo.apiVersion = VK_API_VERSION_1_0;

		m_instanceCreatInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		m_instanceCreatInfo.pApplicationInfo = &m_appInfo;

		m_instanceCreatInfo.enabledExtensionCount = m_extensionCount;
		m_instanceCreatInfo.ppEnabledExtensionNames = m_extension;

		if (enableValidationLayers)
		{
			m_instanceCreatInfo.enabledLayerCount = static_cast<uint32_t>(validationLayerNames.size());
			m_instanceCreatInfo.ppEnabledExtensionNames = validationLayerNames.data();
		}
		else
		{
			m_instanceCreatInfo.enabledLayerCount = 0;
		}
	}

	bool VulkanRenderSystem::_isDeviceSuitable(VkPhysicalDevice device)
	{
		QueueFamiliesIndices indices = findQueueFamilies(device);
		bool extensionsSurpported = _checkDeviceExtensionSupport(device);
		return indices.isComplete() && extensionsSurpported;
	}

	int VulkanRenderSystem::_rateDeviceSuitability(VkPhysicalDevice device)
	{
		int score = 0;
		VkPhysicalDeviceProperties deviceProperties;
		vkGetPhysicalDeviceProperties(device, &deviceProperties);
		VkPhysicalDeviceFeatures deviceFeatures;
		vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

		if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU)
		{
			score += 1000;
		}

		score += deviceProperties.limits.maxImageDimension2D;

		if (deviceFeatures.geometryShader)
			score += 1000;

		return score;

	}

	void VulkanRenderSystem::_creatSurface()
	{

	}

	void VulkanRenderSystem::pickPhysicalDevice()
	{
		std::multimap<int, VkPhysicalDevice> candidates;

		vkEnumeratePhysicalDevices(m_instance, &m_deviceCount, nullptr);
		if (m_deviceCount == 0)
		{
			printf("there is no valid Vulkan physical device\n");
			return;
		}
		m_physicalDevices.resize(m_deviceCount);
		vkEnumeratePhysicalDevices(m_instance, &m_deviceCount, m_physicalDevices.data());
		for (const auto& device : m_physicalDevices)
		{
			//if (_isDeviceSuitable(device))
			//{
			//	std::cout << "device handle is" << device << std::endl;
			//	physicalDevice = device;
			//	break;
			//}
			int score = _rateDeviceSuitability(device);
			candidates.insert(std::make_pair(score, device));
		}
		//if (physicalDevice == VK_NULL_HANDLE)
		if (candidates.rbegin()->first > 0)
		{
			m_physicalDevice = candidates.rbegin()->second;
		}
		else
			throw std::runtime_error("failed to find a suitable GPU");
	}

	rs::SwapChainSupportDetails VulkanRenderSystem::querySwapChainSupport(VkPhysicalDevice device)
	{
		SwapChainSupportDetails details;
		//vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface);
		return details;
	}

	VkResult VulkanRenderSystem::_createDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
		const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger)
	{
		auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessageEXT");
		if (func != nullptr)
		{
			return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
		}
		else
		{
			return VK_ERROR_EXTENSION_NOT_PRESENT;
		}
	}

	VulkanRenderSystem::VulkanRenderSystem()
	{
		m_deviceCount = 0;
		m_extension = NULL;
		m_extensionCount = 0;
	}

	VulkanRenderSystem::~VulkanRenderSystem()
	{
		cleanUp();
	}

	void VulkanRenderSystem::creatInstance()
	{
		_initVulkan();
		if (vkCreateInstance(&m_instanceCreatInfo, nullptr, &m_instance) != VkResult::VK_SUCCESS)
		{
			printf("Vulkan init failed\n");
		}
		if (enableValidationLayers && !_checkValidationLayerSupport())
		{
			throw std::runtime_error("validation layers request,but not available!");
		}
		_setupDebugMessenger();
		std::cout << "creat instance." << std::endl;
	}

	void VulkanRenderSystem::cleanUp()
	{
		vkDestroyDevice(m_logicDevice, nullptr);
		vkDestroyInstance(m_instance, nullptr);

	}

	void VulkanRenderSystem::createLogicalDevice()
	{
		QueueFamiliesIndices indices = findQueueFamilies(m_physicalDevice);

		VkDeviceQueueCreateInfo queueCreateInfo{};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = indices.graphicsFamily.value();
		queueCreateInfo.queueCount = 1;
		float queuePriority = 1.0;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		VkPhysicalDeviceFeatures deviceFeatures{};
		VkDeviceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		createInfo.pQueueCreateInfos = &queueCreateInfo;
		createInfo.queueCreateInfoCount = 1;
		createInfo.pEnabledFeatures = &deviceFeatures;
		createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
		createInfo.ppEnabledExtensionNames = deviceExtensions.data();

		if (enableValidationLayers)
		{
			createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayerNames.size());
			createInfo.ppEnabledLayerNames = validationLayerNames.data();
		}
		else
		{
			createInfo.enabledLayerCount = 0;
		}

		if (vkCreateDevice(m_physicalDevice, &createInfo, nullptr, &m_logicDevice) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create logical device");
		}
		vkGetDeviceQueue(m_logicDevice, indices.graphicsFamily.value(), 0, &m_graphicsQueue);

	}

	bool VulkanRenderSystem::_checkValidationLayerSupport()
	{
		vkEnumerateInstanceLayerProperties(&m_layerCount, nullptr);
		std::vector<VkLayerProperties> validationLayers;
		validationLayers.resize(m_layerCount);
		vkEnumerateInstanceLayerProperties(&m_layerCount, validationLayers.data());
		for (auto validationLayerName : validationLayerNames)
		{
			bool support = false;
			for (auto supportedLayerName : validationLayers)
			{
				if (strcmp(validationLayerName, supportedLayerName.layerName) == 0)
				{
					support = true;
				}
			}
			if (!support)
			{
				std::cout << validationLayerName << "is not supported" << std::endl;
				return false;
			}
			else
			{
				std::cout << validationLayerName << "is supported" << std::endl;
			}
		}
		return true;
	}

	bool VulkanRenderSystem::_checkDeviceExtensionSupport(VkPhysicalDevice device)
	{
		uint32_t extensionCount;
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

		std::vector<VkExtensionProperties> availabeExtensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availabeExtensions.data());

		std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

		for (const auto& extension : availabeExtensions)
		{
			requiredExtensions.erase(extension.extensionName);
		}

		return requiredExtensions.empty();
	}

	QueueFamiliesIndices VulkanRenderSystem::findQueueFamilies(VkPhysicalDevice device)
	{
		QueueFamiliesIndices indices;

		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());
		int i = 0;
		for (const auto& queueFamily : queueFamilies)
		{
			if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				indices.graphicsFamily = i;
			}
			i++;
		}
		return indices;
	}

	void VulkanRenderSystem::_setupDebugMessenger()
	{
		if (!enableValidationLayers)
			return;
		VkDebugUtilsMessengerCreateInfoEXT createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		createInfo.pfnUserCallback = debugCallback;
		createInfo.pUserData = nullptr;
		std::cout << "set up debug messenger." << std::endl;
	}

}

