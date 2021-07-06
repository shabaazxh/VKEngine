#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <stdexcept>
#include <array>

class Descriptors {
public:
	Descriptors(VkDevice device, std::vector<VkImage> swapChainImages, std::vector<VkBuffer> uniformBuffers, VkDescriptorSetLayout descriptorSetLayout);
	void createDescriptorPool();
	void createDescriptorSets();

	VkDescriptorPool getDescriptorPool() { return descriptorPool; }
	std::vector<VkDescriptorSet> getDescriptorSets() { return descriptorSets; }
private:
	VkDevice device;
	VkDescriptorSetLayout descriptorSetLayout;

	std::vector<VkBuffer> uniformBuffers;

	std::vector<VkImage> swapChainImages;
	VkDescriptorPool descriptorPool;
	std::vector<VkDescriptorSet> descriptorSets;
};