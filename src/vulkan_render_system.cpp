#include "vulkan_render_system.h"
#include <iostream>
#include <map>
#include <set>
#include <vulkan/vulkan_win32.h>
#include <GLFW/glfw3native.h>
#include "shader_utils.h"

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
		QueueFamiliesIndices indices = _findQueueFamilies(device);
		bool extensionsSurpported = _checkDeviceExtensionSupport(device);
		bool swapChainAdequate = false;
		if (extensionsSurpported)
		{
			SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
			swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
		}
		return indices.isComplete() && extensionsSurpported && swapChainAdequate;
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
		VkWin32SurfaceCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
		createInfo.hwnd = glfwGetWin32Window(m_window);
		createInfo.hinstance = GetModuleHandle(nullptr);

		if (vkCreateWin32SurfaceKHR(m_instance, &createInfo, nullptr, &m_surface) != VK_SUCCESS)
		{
			throw(std::runtime_error("failed to create window surface"));
		}
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
		std::cout << "pick physical device successed!" << std::endl;
	}

	void VulkanRenderSystem::createSwapChain()
	{
		SwapChainSupportDetails swapChainSupport = querySwapChainSupport(m_physicalDevice);

		VkSurfaceFormatKHR surfaceFormat = _chooseSwapSurfaceFormat(swapChainSupport.formats);
		VkPresentModeKHR presentMode = _chooseSwapPresentMode(swapChainSupport.presentModes);
		VkExtent2D extent = _chooseSwapExtent(swapChainSupport.capabilities);

		uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;

		if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
		{
			imageCount = swapChainSupport.capabilities.maxImageCount;
		}
		VkSwapchainCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = m_surface;

		createInfo.minImageCount = imageCount;
		createInfo.imageFormat = surfaceFormat.format;
		createInfo.imageColorSpace = surfaceFormat.colorSpace;
		createInfo.imageExtent = extent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;


		//store some variables
		m_swapChainImageFormat = surfaceFormat.format;
		m_swapChainExtent = extent;

		QueueFamiliesIndices indices = _findQueueFamilies(m_physicalDevice);
		uint32_t queueFamilyIndice[] = { indices.graphicsFamily.value(),indices.presentFamily.value() };

		if (indices.graphicsFamily != indices.presentFamily)
		{
			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			createInfo.queueFamilyIndexCount = 2;
			createInfo.pQueueFamilyIndices = queueFamilyIndice;
		}
		else
		{
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			createInfo.queueFamilyIndexCount = 0;
			createInfo.pQueueFamilyIndices = nullptr;
		}

		createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode = presentMode;
		createInfo.clipped = VK_TRUE;
		createInfo.oldSwapchain = VK_NULL_HANDLE;

		if (vkCreateSwapchainKHR(m_logicDevice, &createInfo, nullptr, &m_swapChain) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create swap chain");
		}

		vkGetSwapchainImagesKHR(m_logicDevice, m_swapChain, &imageCount, nullptr);
		m_swapChainImages.resize(imageCount);
		vkGetSwapchainImagesKHR(m_logicDevice, m_swapChain, &imageCount, m_swapChainImages.data());
		std::cout << "swap chain create succeed!" << std::endl;
	}

	void VulkanRenderSystem::createImageViews()
	{
		m_swapChainimageViews.resize(m_swapChainImages.size());

		for (size_t i = 0; i < m_swapChainimageViews.size(); i++)
		{
			VkImageViewCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			createInfo.image = m_swapChainImages[i];
			createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			createInfo.format = m_swapChainImageFormat;

			createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

			createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			createInfo.subresourceRange.baseMipLevel = 0;
			createInfo.subresourceRange.levelCount = 1;
			createInfo.subresourceRange.baseArrayLayer = 0;
			createInfo.subresourceRange.layerCount = 1;

			if (vkCreateImageView(m_logicDevice, &createInfo, nullptr, &m_swapChainimageViews[i]) != VK_SUCCESS)
			{
				throw std::runtime_error("failed to create image views");
			}
		}
	}

	void VulkanRenderSystem::createGraphicsPipeline()
	{
		auto vertShaderCode = ShaderUtils::readFile("src/shaders/vert.spv");
		auto fragShaderCode = ShaderUtils::readFile("src/shaders/frag.spv");

		VkShaderModule vertShaderModule = _createShaderModule(vertShaderCode);
		VkShaderModule fragShaderModule = _createShaderModule(fragShaderCode);

		VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
		vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;

		vertShaderStageInfo.module = vertShaderModule;
		vertShaderStageInfo.pName = "main";

		VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
		fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		fragShaderStageInfo.module = fragShaderModule;
		fragShaderStageInfo.pName = "main";

		VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo,fragShaderStageInfo };

		VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputInfo.vertexBindingDescriptionCount = 0;
		vertexInputInfo.pVertexBindingDescriptions = nullptr;
		vertexInputInfo.vertexAttributeDescriptionCount = 0;
		vertexInputInfo.pVertexAttributeDescriptions = nullptr;

		VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
		inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		inputAssembly.primitiveRestartEnable = VK_FALSE;

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = (float)m_swapChainExtent.width;
		viewport.height = (float)m_swapChainExtent.height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		VkRect2D scissor{};
		scissor.offset = { 0,0 };
		scissor.extent = m_swapChainExtent;

		VkPipelineViewportStateCreateInfo viewportState{};
		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.viewportCount = 1;
		viewportState.pViewports = &viewport;
		viewportState.scissorCount = 1;
		viewportState.pScissors = &scissor;

		VkPipelineRasterizationStateCreateInfo rasterizer{};
		rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizer.depthClampEnable = VK_FALSE;
		rasterizer.rasterizerDiscardEnable = VK_FALSE;
		rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
		rasterizer.lineWidth = 1.0f;
		rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
		rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
		rasterizer.depthBiasEnable = VK_FALSE;
		rasterizer.depthBiasConstantFactor = 0.0f;
		rasterizer.depthBiasClamp = 0.0f;
		rasterizer.depthBiasSlopeFactor = 0.0f;

		VkPipelineMultisampleStateCreateInfo multisampling{};
		multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampling.sampleShadingEnable = VK_FALSE;
		multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		multisampling.minSampleShading = 1.0f;
		multisampling.pSampleMask = nullptr;
		multisampling.alphaToCoverageEnable = VK_FALSE;
		multisampling.alphaToOneEnable = VK_FALSE;

		VkPipelineColorBlendAttachmentState colorBlendAttachment{};
		colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_A_BIT | VK_COLOR_COMPONENT_R_BIT
			| VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT;
		colorBlendAttachment.blendEnable = VK_FALSE;
		colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
		colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
		colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
		colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

		VkPipelineColorBlendStateCreateInfo colorBlend{};
		colorBlend.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlend.logicOpEnable = VK_FALSE;
		colorBlend.logicOp = VK_LOGIC_OP_COPY;
		colorBlend.attachmentCount = 1;
		colorBlend.pAttachments = &colorBlendAttachment;
		colorBlend.blendConstants[0] = 0.0f;
		colorBlend.blendConstants[1] = 0.0f;
		colorBlend.blendConstants[2] = 0.0f;
		colorBlend.blendConstants[3] = 0.0f;

		VkDynamicState dynamicStates[] = {
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_LINE_WIDTH
		};
		VkPipelineDynamicStateCreateInfo dynamicState{};
		dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicState.dynamicStateCount = 2;
		dynamicState.pDynamicStates = dynamicStates;

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 0;
		pipelineLayoutInfo.pSetLayouts = nullptr;
		pipelineLayoutInfo.pushConstantRangeCount = 0;
		pipelineLayoutInfo.pPushConstantRanges = nullptr;

		if (vkCreatePipelineLayout(m_logicDevice, &pipelineLayoutInfo, nullptr, &m_pipelineLayout) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create pipeline layout!");
		}

		vkDestroyShaderModule(m_logicDevice, vertShaderModule, nullptr);
		vkDestroyShaderModule(m_logicDevice, fragShaderModule, nullptr);
	}

	void VulkanRenderSystem::createRenderPass()
	{
		VkAttachmentDescription colorAttachment{};
		colorAttachment.format = m_swapChainImageFormat;
		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentReference colorAttachmentRef{};
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	}

	void VulkanRenderSystem::setSurfaceHandle(GLFWwindow* window)
	{
		m_window = window;
	}

	rs::SwapChainSupportDetails VulkanRenderSystem::querySwapChainSupport(VkPhysicalDevice device)
	{
		SwapChainSupportDetails details;
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, m_surface,&details.capabilities);
		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_surface, &formatCount, nullptr);
		if (formatCount != 0)
		{
			details.formats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_surface, &formatCount, details.formats.data());
		}
		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_surface, &presentModeCount, nullptr);
		if (presentModeCount != 0)
		{
			details.presentModes.resize(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_surface, &presentModeCount, details.presentModes.data());
		}
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

	VkSurfaceFormatKHR VulkanRenderSystem::_chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
	{
		for (const auto availableFormat : availableFormats)
		{
			if (availableFormat.format == VK_FORMAT_B8G8R8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			{
				return availableFormat;
			}
		}
		return availableFormats[0];
	}

	VkPresentModeKHR VulkanRenderSystem::_chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
	{
		for (const auto& availablePresentMode : availablePresentModes)
		{
			if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
				return availablePresentMode;
		}
		return VK_PRESENT_MODE_FIFO_KHR;
	}

	VkExtent2D VulkanRenderSystem::_chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
	{
		if (capabilities.currentExtent.width != UINT32_MAX)
		{
			return capabilities.currentExtent;
		}
		else
		{
			VkExtent2D actualExtent = { WIDTH,HEIGHT };

			actualExtent.width = max(capabilities.minImageExtent.width, min(capabilities.maxImageExtent.width, actualExtent.width));
			actualExtent.height = max(capabilities.minImageExtent.height, min(capabilities.maxImageExtent.height, actualExtent.height));

			return actualExtent;
		}
	}

	VkShaderModule VulkanRenderSystem::_createShaderModule(const std::vector<char>& code)
	{
		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = code.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

		VkShaderModule shaderModule;
		if (vkCreateShaderModule(m_logicDevice, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create shader module!");
		}

		return shaderModule;
	}

	VulkanRenderSystem::VulkanRenderSystem():
		m_appInfo{}
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
		_creatSurface();
		std::cout << "creat instance." << std::endl;
	}

	void VulkanRenderSystem::cleanUp()
	{
		vkDestroyPipelineLayout(m_logicDevice, m_pipelineLayout, nullptr);
		for (auto imageView : m_swapChainimageViews)
		{
			vkDestroyImageView(m_logicDevice, imageView, nullptr);
		}
		vkDestroySwapchainKHR(m_logicDevice, m_swapChain, nullptr);
		vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
		vkDestroyDevice(m_logicDevice, nullptr);
		vkDestroyInstance(m_instance, nullptr);

	}

	void VulkanRenderSystem::createLogicalDevice()
	{
		QueueFamiliesIndices indices = _findQueueFamilies(m_physicalDevice);

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
			for (const auto& supportedLayerName : validationLayers)
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

	QueueFamiliesIndices VulkanRenderSystem::_findQueueFamilies(VkPhysicalDevice device)
	{
		QueueFamiliesIndices indices{0,0};

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

		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, m_surface, &presentSupport);
		if (presentSupport)
		{
			indices.presentFamily = i;
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

