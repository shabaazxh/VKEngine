#include "commandBuffers.h"

CommandBuffers::CommandBuffers(VkDevice device, std::vector<VkFramebuffer> framebuffers, VkCommandPool commandPool) {
	this->device = device;
	this->framebuffers = framebuffers;
	this->commandPool = commandPool;
}


void CommandBuffers::createCommandBuffers() {
	
	commandBuffers.resize(framebuffers.size());

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();

	if (vkAllocateCommandBuffers(device, &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate command buffers!");
	}


}