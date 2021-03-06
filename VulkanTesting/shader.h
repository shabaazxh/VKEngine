#pragma once
#include <vector>
#include <fstream>
#include <vulkan/vulkan.h>
#include "Buffer.h"

class Shader {
public:
	Shader(VkDevice device);
	std::vector<char> readFile(const std::string& filename);
	VkShaderModule createShaderModule(const std::vector<char>& code);

private:
	VkDevice device;
};