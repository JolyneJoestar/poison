#include "vulkan_render_system.h"
#include <iostream>

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

	void VulkanRenderSystem::_pickPhysicalDevice()
	{
		
		vkEnumeratePhysicalDevices(m_instance, &m_deviceCount, nullptr);
		if (m_deviceCount == 0)
		{
			printf("there is no valid Vulkan physical device\n");
			return;
		}
		m_devices.resize(m_deviceCount);
		vkEnumeratePhysicalDevices(m_instance, &m_deviceCount, m_devices.data());
	}

	void VulkanRenderSystem::_creatSurface_creatSurface()
	{
//		VkWin32SurfaceCreateInfoKHR sufaceCreateInfo = {};
//		sufaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_KEYED_MUTEX_ACQUIRE_RELEASE_INFO_KHR;
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
		if (enableValidationLayers && !checkValidationLayerSupport())
		{
			throw std::runtime_error("validation layers request,but not available!");
		}
		setupDebugMessenger();
	}

	void VulkanRenderSystem::cleanUp()
	{
		vkDestroyInstance(m_instance, nullptr);
	}

	bool VulkanRenderSystem::checkValidationLayerSupport()
	{
		vkEnumerateInstanceLayerProperties(&m_layerCount, nullptr);
		std::vector<VkLayerProperties> availableLayers(m_layerCount);
		vkEnumerateInstanceLayerProperties(&m_layerCount, availableLayers.data());
		for (auto validationLayerName : validationLayerNames)
		{
			bool support = false;
			for (auto supportedLayerName : availableLayers)
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
		}
		return true;
	}

	void VulkanRenderSystem::setupDebugMessenger()
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
	}

}

