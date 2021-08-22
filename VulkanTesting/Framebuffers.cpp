#include "Framebuffers.h"


Framebuffers::Framebuffers(VkDevice device, std::vector<VkImageView> swapChainImageViews, 
	VkRenderPass renderPass, VkExtent2D swapChainExtent) {
	this->device = device;
	this->swapChainImageViews = swapChainImageViews;
	this->renderPass = renderPass;
	this->swapChainExtent = swapChainExtent;
}

void Framebuffers::createSwapChainFramebuffers(VkRenderPass swapChainPresentRenderPass, VkImageView depthImageView) {

	swapChainFramebuffers.resize(swapChainImageViews.size());

	//iterate through the views and create frame buffers from them

	for (size_t i = 0; i < swapChainImageViews.size(); i++) {
		std::array<VkImageView,1> attachment = {
			swapChainImageViews[i]
			//depthImageView
		};

		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = swapChainPresentRenderPass; //give it the render pass here
		framebufferInfo.attachmentCount = static_cast<uint32_t>(attachment.size());
		framebufferInfo.pAttachments = attachment.data();
		framebufferInfo.width = swapChainExtent.width;
		framebufferInfo.height = swapChainExtent.height;
		framebufferInfo.layers = 1;

		if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &swapChainFramebuffers[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to create framebuffer!");
		}
	}
}

void Framebuffers::createDisplaySceneFramebuffer(VkRenderPass SceneRenderPass, VkImageView SceneImageView, VkImageView depthImageView) {


	std::array<VkImageView, 2> attachment = { SceneImageView, depthImageView };

	VkFramebufferCreateInfo framebufferInfo{};
	framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	framebufferInfo.renderPass = SceneRenderPass; //give it the render pass here
	framebufferInfo.attachmentCount = static_cast<uint32_t>(attachment.size());
	framebufferInfo.pAttachments = attachment.data();
	framebufferInfo.width = swapChainExtent.width;
	framebufferInfo.height = swapChainExtent.height;
	framebufferInfo.layers = 1;

	if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &displaySceneFramebuffer) != VK_SUCCESS) {
		throw std::runtime_error("failed to create framebuffer!");
	}
}
	

void Framebuffers::createShadowFramebuffer(VkRenderPass renderPass, VkImageView shadowImageView) {

	std::array<VkImageView, 1> attachments = { shadowImageView };

	VkFramebufferCreateInfo frameBufferInfo{};
	frameBufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	frameBufferInfo.renderPass = renderPass; //give it the render pass here
	frameBufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
	frameBufferInfo.pAttachments = attachments.data();
	frameBufferInfo.width = 2048;
	frameBufferInfo.height = 2048;
	frameBufferInfo.layers = 1;

	if (vkCreateFramebuffer(device, &frameBufferInfo, nullptr, &shadowFramebuffer) != VK_SUCCESS) {
		throw std::runtime_error("failed to create shadow framebuffer!");
	}

}


void Framebuffers::createSceneFramebuffer(VkRenderPass SceneRenderPass, VkImageView sceneImageView, VkImageView depthImageView) {
	
	std::array<VkImageView, 2> attachments = { sceneImageView, depthImageView };

	VkFramebufferCreateInfo frameBufferInfo{};
	frameBufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	frameBufferInfo.renderPass = SceneRenderPass;
	frameBufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
	frameBufferInfo.pAttachments = attachments.data();
	frameBufferInfo.width = swapChainExtent.width;
	frameBufferInfo.height = swapChainExtent.height;
	frameBufferInfo.layers = 1;

	if (vkCreateFramebuffer(device, &frameBufferInfo, nullptr, &SceneFrameBuffer) != VK_SUCCESS) {
		throw std::runtime_error("failed to create shadow framebuffer!");
	}
}


void Framebuffers::createGeometryPassFrameBuffer(VkRenderPass GeoRenderPass, VkImageView GeometryImageView, VkImageView positionImageView,
	VkImageView normalImageView, VkImageView albedoImageView, VkImageView depthImageView) {

	std::array<VkImageView, 4> attachments = { /*GeometryImageView,*/ positionImageView, normalImageView, albedoImageView, depthImageView };

	VkFramebufferCreateInfo frameBufferInfo{};
	frameBufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	frameBufferInfo.renderPass = GeoRenderPass;
	frameBufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
	frameBufferInfo.pAttachments = attachments.data();
	frameBufferInfo.width = swapChainExtent.width;
	frameBufferInfo.height = swapChainExtent.height;
	frameBufferInfo.layers = 1;

	if (vkCreateFramebuffer(device, &frameBufferInfo, nullptr, &GeometryPassFrameBuffer) != VK_SUCCESS) {
		throw std::runtime_error("failed to create geometry framebuffer!");
	}
}

void Framebuffers::createSSAOQuadFrameBuffer(VkRenderPass SSAORenderPass, VkImageView SSAOImageView) {

	std::array<VkImageView, 1> attachments = { SSAOImageView };

	VkFramebufferCreateInfo frameBufferInfo{};
	frameBufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	frameBufferInfo.renderPass = SSAORenderPass;
	frameBufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
	frameBufferInfo.pAttachments = attachments.data();
	frameBufferInfo.width = swapChainExtent.width;
	frameBufferInfo.height = swapChainExtent.height;
	frameBufferInfo.layers = 1;

	if (vkCreateFramebuffer(device, &frameBufferInfo, nullptr, &SSAOFramebuffer) != VK_SUCCESS) {
		throw std::runtime_error("failed to create shadow framebuffer!");
	}
}


void Framebuffers::createSSAOLightingQuadFrameBuffer(VkRenderPass SSAOLightRenderPass, VkImageView SSAOLightImageView) {
	
	std::array<VkImageView, 1> attachments = { SSAOLightImageView };

	VkFramebufferCreateInfo frameBufferInfo{};
	frameBufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	frameBufferInfo.renderPass = SSAOLightRenderPass;
	frameBufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
	frameBufferInfo.pAttachments = attachments.data();
	frameBufferInfo.width = swapChainExtent.width;
	frameBufferInfo.height = swapChainExtent.height;
	frameBufferInfo.layers = 1;

	if (vkCreateFramebuffer(device, &frameBufferInfo, nullptr, &SSAOLightingFramebuffer) != VK_SUCCESS) {
		throw std::runtime_error("failed to create shadow framebuffer!");
	}
}