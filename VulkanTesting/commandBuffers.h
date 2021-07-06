#pragma once

#include <vulkan/vulkan.h>
#include <stdexcept>
#include <vector>
#include <memory>
#include "Buffer.h"
#include "Object.h"
#include "Buffer.h"

class CommandBuffers {
public:
	CommandBuffers(VkDevice device, std::vector<VkFramebuffer> swapChainFramebuffers, 
		VkCommandPool commandPool, VkRenderPass renderPass, 
		VkExtent2D swapChainExtent, VkPipeline graphicsPipeline, 
		VkPipeline computePipeline, VkPhysicalDevice physicalDevice, 
		VkBuffer renderingBuffer, VkBuffer indexBuffer,VkPipelineLayout pipelineLayout,
		std::vector<VkDescriptorSet> descriptorSets, std::vector<Vertex> CubeVert);
	void createCommandBuffers();

	std::vector<VkCommandBuffer> getCommandBuffers() { return commandBuffers; }

private:
	//std::unique_ptr<Buffer> GameObjectBuffer = std::make_unique<Buffer>(device, physicalDevice);
	std::unique_ptr<Object> GameObjectVertices = std::make_unique<Object>();
	std::unique_ptr<Buffer> bufferAseets = std::make_unique<Buffer>();

	std::vector<VkCommandBuffer> commandBuffers;
	std::vector<VkFramebuffer> swapChainFramebuffers;
	VkDevice device;
	VkCommandPool commandPool;
	VkRenderPass renderPass;
	VkExtent2D swapChainExtent;
	VkPipeline graphicsPipeline;
	VkPipeline computePipeline;
	VkPhysicalDevice physicalDevice;
	VkPipelineLayout pipelineLayout;

	std::vector<VkDescriptorSet> descriptorSets;

	VkBuffer renderingBuffer;
	VkBuffer indexBuffer;

	std::vector<Vertex> CubeVert;
	//std::vector<uint32_t> modelIndex;

	
};