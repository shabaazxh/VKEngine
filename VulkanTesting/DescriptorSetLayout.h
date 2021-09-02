#pragma once
#include <vulkan/vulkan.h>
#include <stdexcept>
#include <vector>
#include <memory>
#include "Buffer.h"
#include <array>

class DescriptorSetLayout {
public:
	DescriptorSetLayout(VkDevice device);

	void createDescriptorSetLayout();
	VkDescriptorSetLayout getDescriptorSetLayout() { return descriptorSetLayout; }
	
	/** Scene descriptor set layout */
	void createQuadDescriptorSetLayout();
	VkDescriptorSetLayout GetQuadDescriptorSetLayout() { return QuadDescriptorSetLayout; }

	void createSSAODescriptorSetLayout();
	VkDescriptorSetLayout GetSSAODescriptorSetLayout() { return SSAOLayout; }

	void createSSAOBlurDescriptorSetLayout();
	VkDescriptorSetLayout GetSSAOBlurDescriptorSetLayout() { return SSAOBlurLayout; }


private:
	VkDescriptorSetLayout descriptorSetLayout;
	VkDescriptorSetLayout QuadDescriptorSetLayout;
	VkDescriptorSetLayout SSAOLayout;
	VkDescriptorSetLayout SSAOBlurLayout;

	std::vector<VkImage> swapChainImages;
	VkDevice device;
};