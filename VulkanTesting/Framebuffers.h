#pragma once

#include <stdexcept>
#include <vulkan/vulkan.h>
#include <vector>
#include <array>

class Framebuffers {
public:
	Framebuffers(VkDevice device,std::vector<VkImageView> swapChainImageViews, 
		VkRenderPass renderPass, VkExtent2D swapChainExtent);

	void createFramebuffer(VkRenderPass renderPass, std::vector<VkImageView> images);


	void createSwapChainFramebuffers(VkRenderPass swapChainPresentRenderPass, VkImageView depthImageView);

	void createDisplaySceneFramebuffer(VkRenderPass SceneRenderPass, VkImageView SceneImageView, VkImageView depthImageView);
	void createGeometryPassFrameBuffer(VkRenderPass GeoRenderPass, VkImageView GeometryImageView, VkImageView positionImageView, 
		VkImageView normalImageView, VkImageView albedoImageView, VkImageView depthImageView);
	
	void createSSAOQuadFrameBuffer(VkRenderPass SSAORenderPass, VkImageView SSAOImageView);
	void createSSAOBlurQuadFrameBuffer(VkRenderPass SSAOLightRenderPass, VkImageView SSAOLightImageView);

	std::vector<VkFramebuffer> getSwapChainFramebuffers() { return swapChainFramebuffers; }
	VkFramebuffer GetDisplaySceneFramebuffer() { return displaySceneFramebuffer; }

	void createShadowFramebuffer(VkRenderPass renderPass, VkImageView shadowImageView);
	void createSceneFramebuffer(VkRenderPass SceneRenderPass, VkImageView sceneImageView, VkImageView depthImageView);

	VkFramebuffer getShadowFramebuffer() { return shadowFramebuffer; }
	VkFramebuffer GetSceneFrameBuffer() { return SceneFrameBuffer; }
	VkFramebuffer GetGeometryPassFrameBuffer() { return GeometryPassFrameBuffer; }
	VkFramebuffer GetSSAOFramebuffer() { return SSAOFramebuffer; }
	VkFramebuffer GetSSAOBlurFramebuffer() { return SSAOBlurFramebuffer; }


	VkFramebuffer GetFramebufers() { return framebuffer; }

private:
	std::vector<VkFramebuffer> swapChainFramebuffers;
	std::vector<VkImageView> swapChainImageViews;

	VkFramebuffer shadowFramebuffer;
	VkFramebuffer displaySceneFramebuffer;
	VkFramebuffer SceneFrameBuffer;
	VkFramebuffer GeometryPassFrameBuffer;
	VkFramebuffer SSAOFramebuffer;
	VkFramebuffer SSAOBlurFramebuffer;

	VkRenderPass renderPass;

	VkFramebuffer framebuffer;

	VkExtent2D swapChainExtent;
	VkDevice device;
};