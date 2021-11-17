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

}

