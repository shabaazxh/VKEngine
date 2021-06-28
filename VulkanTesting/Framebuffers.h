#pragma once

#include <stdexcept>
#include <vulkan/vulkan.h>
#include <vector>

class Framebuffers {
public:
	Framebuffers(VkDevice device,std::vector<VkImageView> swapChainImageViews, VkRenderPass renderPass, VkExtent2D swapChainExtent);
	void createFramebuffers();

	std::vector<VkFramebuffer> getSwapChainFramebuffers() { return swapChainFramebuffers; }


private:
	std::vector<VkFramebuffer> swapChainFramebuffers;
	std::vector<VkImageView> swapChainImageViews;
	VkRenderPass renderPass;
	VkExtent2D swapChainExtent;
	VkDevice device;
};