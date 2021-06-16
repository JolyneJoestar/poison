
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "gltf_demo.h"
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include <iostream>

int main()
{
	GLTFDemo gltfDemo;
	glm::mat4 matrix;
	glm::vec4 vec;
	auto test = matrix * vec;
	try
	{
		gltfDemo.run();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}