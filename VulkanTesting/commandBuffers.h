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
		VkCommandPool commandPool, 
		VkRenderPass SceneRenderPass,
		VkExtent2D swapChainExtent, 
		VkPipeline graphicsPipeline, 
		VkPipeline computePipeline, 
		VkPhysicalDevice physicalDevice, 
		VkBuffer renderingBuffer, 
		VkBuffer indexBuffer,VkPipelineLayout pipelineLayout,
		std::vector<VkDescriptorSet> descriptorSets, 
		std::vector<Vertex> sceneVertexInformation,
		VkPipeline shadowPipeline, 
		VkFramebuffer shadowFramebuffer,
		VkRenderPass shadowRenderPass,
		VkPipelineLayout shadowPipelineLayout, 
		VkPipeline shadowGraphicsPipeline,
		std::vector<Vertex> FloorVertexData,
		VkBuffer FloorRenderBuffer,
		std::vector<uint32_t> QuadIndices,
		VkBuffer QuadVertexBuffer,
		VkBuffer QuadIndexBuffer,
		VkPipeline QuadPipeline,
		VkPipelineLayout QuadPipelineLayout,
		VkRenderPass QuadRenderPass,
		VkFramebuffer displaySceneFramebuffer,
		std::vector<VkDescriptorSet> sceneDescriptorSets,
		VkPipeline GeometryPassPipeline,
		VkPipelineLayout GeometryPassPipelineLayout,
		VkRenderPass GeoRenderPass,
		VkFramebuffer GeoFramebuffer,
		VkPipeline SSAOQuadPipeline,
		VkPipelineLayout SSAOQuadPipelineLayout,
		VkBuffer SSAOQuadVertexBuffer,
		VkBuffer SSAOQuadIndexBuffer,
		VkRenderPass SSAOQuadRenderPass,
		VkFramebuffer SSAOQuadFramebuffer,
		std::vector<VkDescriptorSet> SSAODescriptorSets,
		VkPipeline SSAOLightingPipeline,
		VkPipelineLayout SSAOLightingPipelineLayout,
		std::vector<VkDescriptorSet> SSAOLightingDescriptorSet,
		VkFramebuffer SSAOLightingFramebuffer,
		VkRenderPass SSAOLightingRenderPass);

	void createCommandBuffers();

	std::vector<VkCommandBuffer> getCommandBuffers() { return commandBuffers; }

private:
	std::unique_ptr<Object> GameObjectVertices = std::make_unique<Object>();
	std::unique_ptr<Buffer> bufferAseets = std::make_unique<Buffer>();

	std::vector<VkCommandBuffer> commandBuffers;
	std::vector<VkFramebuffer> swapChainFramebuffers;
	VkDevice device;
	VkCommandPool commandPool;
	VkRenderPass SceneRenderPass;

	//Shadows
	VkFramebuffer shadowFramebuffer;
	VkPipeline shadowPipeline;
	VkRenderPass shadowRenderPass;
	VkPipelineLayout shadowPipelineLayout;
	VkPipeline shadowGraphicsPipeline;

	VkExtent2D swapChainExtent;
	VkPipeline graphicsPipeline;
	VkPipeline computePipeline;
	VkPhysicalDevice physicalDevice;
	VkPipelineLayout pipelineLayout;

	std::vector<VkDescriptorSet> descriptorSets;

	VkBuffer renderingBuffer;
	VkBuffer indexBuffer;

	std::vector<Vertex> sceneVertexInformation;
	std::vector<uint32_t> modelIndex;

	std::vector<Vertex> FloorVertexData;
	VkBuffer FloorRenderBuffer;

	std::vector<uint32_t> QuadIndices;
	VkBuffer QuadVertexBuffer;
	VkBuffer QuadIndexBuffer;
	VkPipeline QuadPipeline;
	VkPipelineLayout QuadPipelineLayout;
	VkRenderPass QuadRenderPass;

	VkFramebuffer displaySceneFramebuffer;
	std::vector<VkDescriptorSet> sceneDescriptorSets;

	//Geometry pass
	VkPipeline GeometryPassPipeline;
	VkPipelineLayout GeometryPassPipelineLayout;
	VkRenderPass GeoRenderPass;
	VkFramebuffer GeoFramebuffer;

	// SSAO Quad 
	VkPipeline SSAOQuadPipeline;
	VkPipelineLayout SSAOQuadPipelineLayout;
	VkBuffer SSAOQuadVertexBuffer;
	VkBuffer SSAOQuadIndexBuffer;
	VkRenderPass SSAOQuadRenderPass;
	VkFramebuffer SSAOQuadFramebuffer;
	std::vector<VkDescriptorSet> SSAODescriptorSets;

	VkPipeline SSAOLightingPipeline;
	VkPipelineLayout SSAOLightingPipelineLayout;
	std::vector<VkDescriptorSet> SSAOLightingDescriptorSet;
	VkFramebuffer SSAOLightingFramebuffer;
	VkRenderPass SSAOLightingRenderPass;

};