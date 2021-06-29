#pragma once

#include <vulkan/vulkan.h>
#include <stdexcept>
#include <vector>

class CommandBuffers {
public:
	CommandBuffers(VkDevice device, std::vector<VkFramebuffer> swapChainFramebuffers, VkCommandPool commandPool, VkRenderPass renderPass, VkExtent2D swapChainExtent, VkPipeline graphicsPipeline);
	void createCommandBuffers();

	std::vector<VkCommandBuffer> getCommandBuffers() { return commandBuffers; }

private:

	std::vector<VkCommandBuffer> commandBuffers;
	std::vector<VkFramebuffer> swapChainFramebuffers;
	VkDevice device;
	VkCommandPool commandPool;
	VkRenderPass renderPass;
	VkExtent2D swapChainExtent;
	VkPipeline graphicsPipeline;

};