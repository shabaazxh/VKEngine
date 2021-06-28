#pragma once

#include <vector>
#include "shader.h"
#include <vulkan/vulkan.h>

class Pipeline {
public:
	Pipeline(VkDevice device, VkRenderPass renderPass,VkExtent2D swapChainExtent);
	void createGraphicsPipeline(std::string vertFile, std::string fragFile);
	VkPipelineLayout getPipelineLayout() { return pipelineLayout; }
	VkPipeline getGraphicsPipeline() { return graphicsPipeline; }
private:
	VkDevice device;
	VkExtent2D swapChainExtent;
	VkPipelineLayout pipelineLayout;
	VkRenderPass renderPass;
	VkPipeline graphicsPipeline;
};