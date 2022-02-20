#pragma once

#include <vulkan/vulkan.h>
#include <stdexcept>
#include <vector>
#include <memory>
#include "Buffer.h"
#include "Object.h"
#include "Buffer.h"
#include "vendor/imgui/imgui.h"
#include "vendor/imgui/imgui_impl_vulkan.h"
#include "vendor/imgui/imgui_impl_glfw.h"

class CommandBuffers {
public:
	CommandBuffers() = default;
	CommandBuffers(VkDevice device, 
		std::vector<VkFramebuffer> swapChainFramebuffers, 
		VkQueue graphicsQueue,
		VkCommandPool commandPool, 
		VkRenderPass SceneRenderPass,
		VkExtent2D swapChainExtent, 
		VkPipeline graphicsPipeline, 
		VkPipeline computePipeline, 
		VkPhysicalDevice physicalDevice, 
		VkBuffer renderingBuffer, 
		VkBuffer indexBuffer,
		VkPipelineLayout pipelineLayout,
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
		VkPipeline SSAOBlurPipeline,
		VkPipelineLayout SSAOBlurPipelineLayout,
		std::vector<VkDescriptorSet> SSAOBlurDescriptorSet,
		VkFramebuffer SSAOBlurFramebuffer,
		VkRenderPass SSAOBlurRenderPass,
		VkPipeline FloorPipeline,
		VkPipelineLayout FloorPipelineLayout,
		std::vector<VkDescriptorSet> FloorDescriptorSet,
		VkBuffer positionsVertexBuffer,
		VkBuffer positionsIndexBuffer,
		VkFramebuffer positionsFB,
		VkRenderPass positionsRP,
		VkPipeline positionsPipeline,
		VkPipelineLayout positionsPL,
		std::vector<uint32_t> modelIndex,
		std::vector<uint32_t> FloorModelIndexData,
		std::vector<uint32_t> CubeMapIndex,
		VkBuffer FloorModelIndexBuffer,
		VkBuffer CubeMapVertexBuffer,
		VkBuffer CubeMapIndexBuffer,
		VkPipeline CubeMapPipeline,
		VkPipelineLayout CubeMapPipelineLayout);

	void createCommandBuffers();

	std::vector<VkCommandBuffer> getCommandBuffers() { return commandBuffers; }

	void UI(VkCommandBuffer commandBuffer);

	VkCommandBuffer beginSingleTimeCommands(VkDevice device, VkCommandPool commandPool);
	void endSingleTimeCommnads(VkCommandBuffer commandBuffer, VkDevice device, VkCommandPool commandPool, VkQueue graphicsQueue);

private:
	std::unique_ptr<Object> GameObjectVertices = std::make_unique<Object>();
	std::unique_ptr<Buffer> bufferAseets = std::make_unique<Buffer>();

	std::vector<VkCommandBuffer> commandBuffers;
	std::vector<VkFramebuffer> swapChainFramebuffers;
	VkDevice device;
	VkCommandPool commandPool;
	VkRenderPass SceneRenderPass;
	VkQueue graphicsQueue;

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
	std::vector<uint32_t> FloorModelIndexData;
	VkBuffer FloorModelIndexBuffer;

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

	VkPipeline SSAOBlurPipeline;
	VkPipelineLayout SSAOBlurPipelineLayout;
	std::vector<VkDescriptorSet> SSAOBlurDescriptorSet;
	VkFramebuffer SSAOBlurFramebuffer;
	VkRenderPass SSAOBlurRenderPass;

	VkPipeline FloorPipeline;
	VkPipelineLayout FloorPipelineLayout;

	std::vector<VkDescriptorSet> FloorDescriptorSet;


	VkBuffer positionsVertexBuffer;
	VkBuffer positionsIndexBuffer;
	VkFramebuffer positionsFB;
	VkRenderPass positionsRP;
	VkPipeline positionsPipeline;
	VkPipelineLayout positionsPL;

	VkBuffer CubeMapVertexBuffer;
	VkBuffer CubeMapIndexBuffer;
	VkPipeline CubeMapPipeline;
	VkPipelineLayout CubeMapPipelineLayout;
	std::vector<uint32_t> CubeMapIndex;

	VkCommandBuffer commandBufferImGui;

};