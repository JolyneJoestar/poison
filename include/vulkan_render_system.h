#pragma once
#include <vulkan/vulkan.h>
#include <vector>

namespace rs
{
	const std::vector<const char*> validationLayerNames = { "VK_LAYER_LUNARG_object_tracker" };
	const uint32_t WIDTH = 800;
	const uint32_t HEIGHT = 600;
#ifndef NDDEBUG
	const bool enableValidationLayers = false;
#else
	const bool enableValidationLayers = true;
#endif


	class VulkanRenderSystem
	{
	public:
		VulkanRenderSystem();
		~VulkanRenderSystem();
		void setInstanceCreatInfo(VkInstanceCreateInfo &instanceCreatInfo) { m_instanceCreatInfo = instanceCreatInfo; }
		void creatInstance();
		void cleanUp();


		bool checkValidationLayerSupport();

		//Extensions
		void setExtensionCount(uint32_t count) { m_extensionCount = count; }
		void setExtension(const char** extension) { m_extension = extension; }

		//Debug
		void setupDebugMessenger();
	private:
		void _initVulkan();
		void _pickPhysicalDevice();
		void _creatSurface();
		void _creatSurface_creatSurface();
		VkResult _createDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);

		VkApplicationInfo m_appInfo;
		VkSurfaceKHR m_vkSurface;
		VkInstanceCreateInfo m_instanceCreatInfo;
		VkInstance m_instance;
		uint32_t m_deviceCount;
		uint32_t m_layerCount;
		std::vector<VkPhysicalDevice> m_devices;		

		//Extension
		uint32_t m_extensionCount;
		const char** m_extension;
	};
}