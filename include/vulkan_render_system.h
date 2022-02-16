#pragma once
#define GLFW_INCLUDE_VULKAN

#if defined(_WIN32)
#include <windows.h>
#include <fcntl.h>
#include <io.h>
#endif

#include <vulkan/vulkan.h>
#include <vector>
#include <optional>
#include <GLFW/glfw3.h>
#include "GLFW/glfw3native.h"

namespace rs
{
	const std::vector<const char*> validationLayerNames = { "VK_LAYER_LUNARG_object_tracker" };
	const std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
	const uint32_t WIDTH = 800;
	const uint32_t HEIGHT = 600;
#ifndef NDDEBUG
	const bool enableValidationLayers = false;
#else
	const bool enableValidationLayers = true;
#endif

	struct QueueFamiliesIndices {
		std::optional<uint32_t> graphicsFamily;

		bool isComplete() {
			return graphicsFamily.has_value();
		}
	};

	struct SwapChainSupportDetails
	{
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};
	class VulkanRenderSystem
	{
	public:
		VulkanRenderSystem();
		~VulkanRenderSystem();
		void setInstanceCreatInfo(VkInstanceCreateInfo &instanceCreatInfo) { m_instanceCreatInfo = instanceCreatInfo; }
		void creatInstance();
		void cleanUp();

		void createLogicalDevice();


		void pickPhysicalDevice();
		//Extensions
		void setExtensionCount(uint32_t count) { m_extensionCount = count; }
		void setExtension(const char** extension) { m_extension = extension; }

	private:
		void _initVulkan();
		bool _isDeviceSuitable(VkPhysicalDevice device);
		int _rateDeviceSuitability(VkPhysicalDevice device);

		void _creatSurface();

		SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
		//Debug
		void _setupDebugMessenger();
		bool _checkValidationLayerSupport();
		bool _checkDeviceExtensionSupport(VkPhysicalDevice device);

		QueueFamiliesIndices findQueueFamilies(VkPhysicalDevice device);

		VkResult _createDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);

		VkApplicationInfo m_appInfo;
		VkSurfaceKHR m_vkSurface;
		VkInstanceCreateInfo m_instanceCreatInfo;
		VkInstance m_instance;
		uint32_t m_deviceCount;
		uint32_t m_layerCount;
		std::vector<VkPhysicalDevice> m_physicalDevices;
		VkPhysicalDevice m_physicalDevice;
		VkDevice m_logicDevice;
		VkQueue m_graphicsQueue;

		//Extension
		uint32_t m_extensionCount;
		const char** m_extension;
	};
}