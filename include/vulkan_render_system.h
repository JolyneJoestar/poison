#pragma once
#define GLFW_INCLUDE_VULKAN

#if defined(_WIN32)
#include <windows.h>
#include <fcntl.h>
#include <io.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#endif

#include <vulkan/vulkan.h>
#include <vector>
#include <optional>

#include <GLFW/glfw3.h>

//#define  NDDEBUG

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
		std::optional<uint32_t> presentFamily;
		bool isComplete() {
			return graphicsFamily.has_value() && presentFamily.has_value();
		}
	};

	struct SwapChainSupportDetails
	{
		VkSurfaceCapabilitiesKHR capabilities{};
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

		void createSwapChain();

		void createImageViews();

		void createGraphicsPipeline();

		void createRenderPass();

		void createFramebuffers();
		//Extensions
		void setExtensionCount(uint32_t count) { m_extensionCount = count; }
		void setExtension(const char** extension) { m_extension = extension; }

		void setSurfaceHandle(GLFWwindow* window);
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

		QueueFamiliesIndices _findQueueFamilies(VkPhysicalDevice device);

		VkResult _createDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);

		//surface && swapChain
		VkSurfaceFormatKHR _chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
		VkPresentModeKHR _chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
		VkExtent2D _chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

		//shaderModule
		VkShaderModule _createShaderModule(const std::vector<char>& code);

		VkApplicationInfo m_appInfo;
		VkSurfaceKHR m_vkSurface;
		VkInstanceCreateInfo m_instanceCreatInfo;
		VkInstance m_instance;
		uint32_t m_deviceCount;
		uint32_t m_layerCount;


		//devices
		std::vector<VkPhysicalDevice> m_physicalDevices;
		VkPhysicalDevice m_physicalDevice;
		VkDevice m_logicDevice;

		//queue
		VkQueue m_graphicsQueue;

		//Extension
		uint32_t m_extensionCount;
		const char** m_extension;

		//surface 
		GLFWwindow* m_window;
		VkSurfaceKHR m_surface;

		//swap chain
		VkSwapchainKHR m_swapChain;
		VkFormat m_swapChainImageFormat;
		VkExtent2D m_swapChainExtent;

		std::vector<VkImage> m_swapChainImages;
		std::vector<VkImageView> m_swapChainimageViews;

		//pipeline
		VkPipelineLayout m_pipelineLayout;

		//Renderpass
		VkRenderPass m_renderPass;
		VkPipelineLayout pipelineLayout;
		VkPipeline m_graphicsPipline;
	};
}