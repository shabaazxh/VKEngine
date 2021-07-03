#pragma once
#include <vulkan/vulkan.h>
#include <stdexcept>
#include <vector>
#include <memory>
#include "Buffer.h"

class DescriptorSetLayout {
public:
	DescriptorSetLayout(VkDevice device);

	void createDescriptorSetLayout();

	VkDescriptorSetLayout getDescriptorSetLayout() { return descriptorSetLayout; }


private:
	VkDescriptorSetLayout descriptorSetLayout;
	std::vector<VkImage> swapChainImages;
	VkDevice device;
};