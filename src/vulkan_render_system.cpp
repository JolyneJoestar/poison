#include "vulkan_render_system.h"
#include <iostream>

namespace rs
{

	void VulkanRenderSystem::_initVulkan()
	{

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

	void VulkanRenderSystem::_creatSurface()
	{
		VkWin32SurfaceCreateInfoKHR sufaceCreateInfo = {};
		sufaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_KEYED_MUTEX_ACQUIRE_RELEASE_INFO_KHR;
	}

	VulkanRenderSystem::VulkanRenderSystem()
	{
		m_deviceCount = 0;
	}

	VulkanRenderSystem::~VulkanRenderSystem()
	{
		cleanUp();
	}

	void VulkanRenderSystem::creatInstance()
	{
		if (vkCreateInstance(&m_instanceCreatInfo, nullptr, &m_instance) != VkResult::VK_SUCCESS)
		{
			printf("Vulkan init failed\n");
		}
	}

	void VulkanRenderSystem::cleanUp()
	{
		vkDestroyInstance(m_instance, nullptr);
	}

	void VulkanRenderSystem::checkValidationLayerSupport()
	{
		vkEnumerateInstanceLayerProperties(&m_layerCount, nullptr);
		std::vector<VkLayerProperties> availableLayers(m_layerCount);
		vkEnumerateInstanceLayerProperties(&m_layerCount, availableLayers.data());
		for (auto validationLayerName : m_validationLayerNames)
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
			}
		}
	}

}

