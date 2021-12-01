#pragma once
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_win32.h>
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

		void checkValidationLayerSupport();

	private:
		void _initVulkan();
		void _pickPhysicalDevice();
		void _creatSurface();

		VkSurfaceKHR m_vkSurface;
		VkInstanceCreateInfo m_instanceCreatInfo;
		VkInstance m_instance;
		uint32_t m_deviceCount;
		uint32_t m_layerCount;
		std::vector<VkPhysicalDevice> m_devices;
		std::vector<const char*> m_validationLayerNames = { "VK_LAYER_LUNARG_object_tracker" };
	};
}