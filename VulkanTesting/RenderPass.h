#pragma once


#include <vulkan/vulkan.h>
#include <stdexcept>

class RenderPass {
public:
	RenderPass(VkDevice device, VkPipelineLayout pipelineLayout, VkFormat swapChainImageFormat);
	void createRenderPass();

	VkRenderPass getRenderPass() { return renderPass; }

private:
	VkPipelineLayout pipelineLayout;
	VkRenderPass renderPass;
	VkFormat swapChainImageFormat;
	VkDevice device;


};