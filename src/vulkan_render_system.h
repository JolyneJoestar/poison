#pragma once
#include <vulkan/vulkan.h>
#include <vector>

namespace rs
{
	class VulkanRenderSystem
	{
	public:
		VulkanRenderSystem();
		~VulkanRenderSystem();
		void setInstanceCreatInfo(VkInstanceCreateInfo &instanceCreatInfo) { m_instanceCreatInfo = instanceCreatInfo; }
		void creatInstance();
		void cleanUp();

	private:
		void _initVulkan();
		void _pickPhysicalDevice();

		VkInstanceCreateInfo m_instanceCreatInfo;
		VkInstance m_instance;
		uint32_t m_deviceCount;
		std::vector<VkPhysicalDevice> m_devices;

	};
}