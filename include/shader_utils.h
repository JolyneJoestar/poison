//C:\VulkanSDK\1.2.176.1\Bin\glslc.exe base_shader.vert -o vert.spv

#pragma once
#include <fstream>
#include <vector>
#include <vulkan/vulkan.h>

namespace rs
{
	class ShaderUtils
	{
	public:
		static std::vector<char> readFile(const std::string& filename);
	};
}