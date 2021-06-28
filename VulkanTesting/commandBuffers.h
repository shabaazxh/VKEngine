#pragma once

#include <vulkan/vulkan.h>
#include <stdexcept>
#include <vector>

class CommandBuffers {
public:
	CommandBuffers(VkDevice device, std::vector<VkFramebuffer> framebuffers, VkCommandPool commandPool);
	void createCommandBuffers();

	std::vector<VkCommandBuffer> getCommandBuffers() { return commandBuffers; }

private:

	std::vector<VkCommandBuffer> commandBuffers;
	std::vector<VkFramebuffer> framebuffers;
	VkDevice device;
	VkCommandPool commandPool;

};