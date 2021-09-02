#include "commandBuffers.h"
#include <iostream>

CommandBuffers::CommandBuffers(VkDevice device, std::vector<VkFramebuffer> swapChainFramebuffers, 
	VkCommandPool commandPool, VkRenderPass SceneRenderPass, VkExtent2D swapChainExtent,
	VkPipeline graphicsPipeline, VkPipeline computePipeline, VkPhysicalDevice physicalDevice, 
	VkBuffer renderingBuffer, VkBuffer indexBuffer ,VkPipelineLayout pipelineLayout, 
	std::vector<VkDescriptorSet> descriptorSets, std::vector<Vertex> sceneVertexInformation,
	VkPipeline shadowPipeline, VkFramebuffer shadowFramebuffer, VkRenderPass shadowRenderPass,
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
	std::vector<VkDescriptorSet> FloorDescriptorSet){

	this->device = device;
	this->swapChainFramebuffers = swapChainFramebuffers;
	this->commandPool = commandPool;
	this->SceneRenderPass = SceneRenderPass;
	this->swapChainExtent = swapChainExtent;
	this->graphicsPipeline = graphicsPipeline;
	this->computePipeline = computePipeline;
	this->renderingBuffer = renderingBuffer;
	this->pipelineLayout = pipelineLayout;
	this->descriptorSets = descriptorSets;
	this->indexBuffer = indexBuffer;
	this->sceneVertexInformation = sceneVertexInformation;
	this->shadowPipeline = shadowPipeline;
	this->shadowFramebuffer = shadowFramebuffer;
	this->shadowRenderPass = shadowRenderPass;
	this->shadowPipelineLayout = shadowPipelineLayout;
	this->shadowGraphicsPipeline = shadowGraphicsPipeline;
	this->FloorVertexData = FloorVertexData;
	this->FloorRenderBuffer = FloorRenderBuffer;
	this->QuadIndices = QuadIndices;
	this->QuadVertexBuffer = QuadVertexBuffer;
	this->QuadIndexBuffer = QuadIndexBuffer;
	this->QuadPipeline = QuadPipeline;
	this->QuadPipelineLayout = QuadPipelineLayout;
	this->QuadRenderPass = QuadRenderPass;
	this->displaySceneFramebuffer = displaySceneFramebuffer;
	this->sceneDescriptorSets = sceneDescriptorSets;
	this->GeometryPassPipeline = GeometryPassPipeline;
	this->GeometryPassPipelineLayout = GeometryPassPipelineLayout;
	this->GeoRenderPass = GeoRenderPass;
	this->GeoFramebuffer = GeoFramebuffer;
	this->SSAOQuadPipeline = SSAOQuadPipeline;
	this->SSAOQuadPipelineLayout = SSAOQuadPipelineLayout;
	this->SSAOQuadVertexBuffer = SSAOQuadVertexBuffer;
	this->SSAOQuadIndexBuffer = SSAOQuadIndexBuffer;
	this->SSAOQuadRenderPass = SSAOQuadRenderPass;
	this->SSAOQuadFramebuffer = SSAOQuadFramebuffer;
	this->SSAODescriptorSets = SSAODescriptorSets;
	this->SSAOBlurPipeline = SSAOBlurPipeline;
	this->SSAOBlurPipelineLayout = SSAOBlurPipelineLayout;
	this->SSAOBlurDescriptorSet = SSAOBlurDescriptorSet;
	this->SSAOBlurFramebuffer = SSAOBlurFramebuffer;
	this->SSAOBlurRenderPass = SSAOBlurRenderPass;
	this->FloorPipeline = FloorPipeline;
	this->FloorPipelineLayout = FloorPipelineLayout;
	this->FloorDescriptorSet = FloorDescriptorSet;
}

void CommandBuffers::createCommandBuffers() {
	commandBuffers.resize(swapChainFramebuffers.size());

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();

	if (vkAllocateCommandBuffers(device, &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate command buffers!");
	}

	for (size_t i = 0; i < commandBuffers.size(); i++) {
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("failed to begin recording command buffer!");
		}

		//------------------------- Shadow render pass --------------------------------------------------
		VkRenderPassBeginInfo renderPassInfo2{};
		renderPassInfo2.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo2.renderPass = shadowRenderPass;
		renderPassInfo2.framebuffer = shadowFramebuffer;
		renderPassInfo2.renderArea.offset = { 0,0 };
		renderPassInfo2.renderArea.extent.width = 2048;
		renderPassInfo2.renderArea.extent.height = 2048;

		VkClearValue clearValuesShadow[2];
		clearValuesShadow[0].depthStencil = { 1.0f, 0 };

		renderPassInfo2.clearValueCount = 1;
		renderPassInfo2.pClearValues = clearValuesShadow;

		vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo2, VK_SUBPASS_CONTENTS_INLINE);
		vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, shadowGraphicsPipeline);

		VkBuffer vertexBuffers2[] = { renderingBuffer };

		VkDeviceSize offsets2[] = { 0 };

		vkCmdBindVertexBuffers(commandBuffers[i], 0, 1, vertexBuffers2, offsets2);
		//vkCmdBindIndexBuffer(commandBuffers[i], indexBuffer, 0, VK_INDEX_TYPE_UINT32);
		vkCmdBindDescriptorSets(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, shadowPipelineLayout, 0, 1, &descriptorSets[i], 0, nullptr);
		//vkCmdDrawIndexed(commandBuffers[i], static_cast<uint32_t>(modelIndex.size()), 1, 0, 0, 0);
		vkCmdDraw(commandBuffers[i], sceneVertexInformation.size(), 1, 0, 0);

		vkCmdEndRenderPass(commandBuffers[i]);

		// -------------------- Normal scene render ----------------------
		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = SceneRenderPass;
		renderPassInfo.framebuffer = displaySceneFramebuffer;
		renderPassInfo.renderArea.offset = { 0,0 };
		renderPassInfo.renderArea.extent = swapChainExtent;

		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { 0.3f, 0.5f, .7f, 1.0f };
		clearValues[1].depthStencil = { 1.0f, 0 };

		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkBuffer vertexBuffers3[] = { renderingBuffer };
		VkBuffer FloorVertex[] = { FloorRenderBuffer };
		VkDeviceSize offsets3[] = { 0 };

		vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);
		vkCmdBindDescriptorSets(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSets[i], 0, nullptr);
		
		// Object model
		vkCmdBindVertexBuffers(commandBuffers[i], 0, 1, vertexBuffers3, offsets3);
		vkCmdDraw(commandBuffers[i], sceneVertexInformation.size(), 1, 0, 0);
		
		vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, FloorPipeline);
		vkCmdBindDescriptorSets(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, FloorPipelineLayout, 0, 1, &FloorDescriptorSet[i], 0, nullptr);
		//Floor model
		vkCmdBindVertexBuffers(commandBuffers[i], 0, 1, FloorVertex, offsets3);
		vkCmdDraw(commandBuffers[i], FloorVertexData.size(), 1, 0, 0);

		vkCmdEndRenderPass(commandBuffers[i]);

		/** -----------------------------  GEOMETRY PASS Render -------------------------- */

		VkRenderPassBeginInfo GeometryPassRenderInfo{};
		GeometryPassRenderInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		GeometryPassRenderInfo.renderPass = GeoRenderPass;
		GeometryPassRenderInfo.framebuffer = GeoFramebuffer;
		GeometryPassRenderInfo.renderArea.offset = { 0,0 };
		GeometryPassRenderInfo.renderArea.extent = swapChainExtent;

		std::array<VkClearValue, 4> GeoPassClearColor{};
		GeoPassClearColor[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };
		GeoPassClearColor[1].color = { 0.0f, 0.0f, 0.0f, 1.0f };
		GeoPassClearColor[2].color = { 0.0f, 0.0f, 0.0f, 1.0f };
		GeoPassClearColor[3].depthStencil = { 1.0f, 0 };
		/*GeoPassClearColor[3].color = { 0.0f, 0.0f, 0.0f, 1.0f };*/

		GeometryPassRenderInfo.clearValueCount = static_cast<uint32_t>(GeoPassClearColor.size());
		GeometryPassRenderInfo.pClearValues = GeoPassClearColor.data();

		vkCmdBeginRenderPass(commandBuffers[i], &GeometryPassRenderInfo, VK_SUBPASS_CONTENTS_INLINE);

		vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, GeometryPassPipeline);
		vkCmdBindDescriptorSets(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, GeometryPassPipelineLayout, 0, 1, &descriptorSets[i], 0, nullptr);

		// Object model
		vkCmdBindVertexBuffers(commandBuffers[i], 0, 1, vertexBuffers3, offsets3);
		vkCmdDraw(commandBuffers[i], sceneVertexInformation.size(), 1, 0, 0);

		////Floor model
		vkCmdBindVertexBuffers(commandBuffers[i], 0, 1, FloorVertex, offsets3);
		vkCmdDraw(commandBuffers[i], FloorVertexData.size(), 1, 0, 0);

		vkCmdEndRenderPass(commandBuffers[i]);


		/** -----------------------------  SSAO QUAD Render -------------------------- */

		VkRenderPassBeginInfo SSAOQuadRenderPassInfo{};
		SSAOQuadRenderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		SSAOQuadRenderPassInfo.renderPass = SSAOQuadRenderPass;
		SSAOQuadRenderPassInfo.framebuffer = SSAOQuadFramebuffer;
		SSAOQuadRenderPassInfo.renderArea.offset = { 0,0 };
		SSAOQuadRenderPassInfo.renderArea.extent = swapChainExtent;

		std::array<VkClearValue, 1> SSAOQuadClearValues{};
		SSAOQuadClearValues[0].color = { 1.0f, 1.0f, 1.0f, 1.0f };

		SSAOQuadRenderPassInfo.clearValueCount = static_cast<uint32_t>(SSAOQuadClearValues.size());
		SSAOQuadRenderPassInfo.pClearValues = SSAOQuadClearValues.data();

		vkCmdBeginRenderPass(commandBuffers[i], &SSAOQuadRenderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkBuffer SSAOQuadVertexBufferArr[] = { SSAOQuadVertexBuffer };
		VkDeviceSize SSAOQuadOffSet[] = { 0 };

		// SSAO Quad render
		vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, SSAOQuadPipeline);
		vkCmdBindDescriptorSets(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, SSAOQuadPipelineLayout, 0, 1, &SSAODescriptorSets[i], 0, nullptr);
		vkCmdBindVertexBuffers(commandBuffers[i], 0, 1, SSAOQuadVertexBufferArr, SSAOQuadOffSet);
		vkCmdBindIndexBuffer(commandBuffers[i], SSAOQuadIndexBuffer, 0, VK_INDEX_TYPE_UINT32);

		vkCmdDrawIndexed(commandBuffers[i], static_cast<uint32_t>(QuadIndices.size()), 1, 0, 0, 0);

		vkCmdEndRenderPass(commandBuffers[i]);

		/** -----------------------------  SSAO Blur QUAD Render -------------------------- */

		VkRenderPassBeginInfo SSAOBlurQuadRenderPassInfo{};
		SSAOBlurQuadRenderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		SSAOBlurQuadRenderPassInfo.renderPass = SSAOBlurRenderPass;
		SSAOBlurQuadRenderPassInfo.framebuffer = SSAOBlurFramebuffer;
		SSAOBlurQuadRenderPassInfo.renderArea.offset = { 0,0 };
		SSAOBlurQuadRenderPassInfo.renderArea.extent = swapChainExtent;

		std::array<VkClearValue, 1> SSAOBlurQuadClearValues{};
		SSAOBlurQuadClearValues[0].color = { 1.0f, 1.0f, 1.0f, 1.0f };

		SSAOBlurQuadRenderPassInfo.clearValueCount = static_cast<uint32_t>(SSAOBlurQuadClearValues.size());
		SSAOBlurQuadRenderPassInfo.pClearValues = SSAOBlurQuadClearValues.data();

		vkCmdBeginRenderPass(commandBuffers[i], &SSAOBlurQuadRenderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkBuffer SSAOBlurQuadVertexBufferArr[] = { SSAOQuadVertexBuffer };
		VkDeviceSize SSAOBlurQuadOffSet[] = { 0 };

		// SSAO Quad render
		vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, SSAOBlurPipeline);
		vkCmdBindDescriptorSets(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, SSAOBlurPipelineLayout, 0, 1, &SSAOBlurDescriptorSet[i], 0, nullptr);
		vkCmdBindVertexBuffers(commandBuffers[i], 0, 1, SSAOBlurQuadVertexBufferArr, SSAOBlurQuadOffSet);
		vkCmdBindIndexBuffer(commandBuffers[i], SSAOQuadIndexBuffer, 0, VK_INDEX_TYPE_UINT32);

		vkCmdDrawIndexed(commandBuffers[i], static_cast<uint32_t>(QuadIndices.size()), 1, 0, 0, 0);

		vkCmdEndRenderPass(commandBuffers[i]);

		/** -----------------------------  QUAD Render -------------------------- */

		VkRenderPassBeginInfo QuadRenderPassInfo{};
		QuadRenderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		QuadRenderPassInfo.renderPass = QuadRenderPass;
		QuadRenderPassInfo.framebuffer = swapChainFramebuffers[i];
		QuadRenderPassInfo.renderArea.offset = { 0,0 };
		QuadRenderPassInfo.renderArea.extent = swapChainExtent;

		std::array<VkClearValue, 1> QuadClearValues{};
		QuadClearValues[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };

		QuadRenderPassInfo.clearValueCount = static_cast<uint32_t>(QuadClearValues.size());
		QuadRenderPassInfo.pClearValues = QuadClearValues.data();

		vkCmdBeginRenderPass(commandBuffers[i], &QuadRenderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkBuffer QuadVertexBufferArr[] = { QuadVertexBuffer };
		VkDeviceSize QuadOffSet[] = { 0 };

		// Quad render
		vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, QuadPipeline);
		vkCmdBindDescriptorSets(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, QuadPipelineLayout, 0, 1, &sceneDescriptorSets[i], 0, nullptr);
		vkCmdBindVertexBuffers(commandBuffers[i], 0, 1, QuadVertexBufferArr, QuadOffSet);
		vkCmdBindIndexBuffer(commandBuffers[i], QuadIndexBuffer, 0, VK_INDEX_TYPE_UINT32);
		
		vkCmdDrawIndexed(commandBuffers[i], static_cast<uint32_t>(QuadIndices.size()), 1, 0, 0, 0);

		vkCmdEndRenderPass(commandBuffers[i]);
		
		if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to record command buffer!");
		}

	}

}

/* COMPUTE PIPELINE */
		//vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_COMPUTE, computePipeline);
		//vkCmdDispatch(commandBuffers[i], 256,1,1);