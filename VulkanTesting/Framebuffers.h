#pragma once

#include <stdexcept>
#include <vulkan/vulkan.h>
#include <vector>
#include <array>

class Framebuffers {
public:
	Framebuffers(VkDevice device,std::vector<VkImageView> swapChainImageViews, 
		VkRenderPass renderPass, VkExtent2D swapChainExtent);
	void createSwapChainFramebuffers(VkRenderPass swapChainPresentRenderPass, VkImageView depthImageView);

	void createDisplaySceneFramebuffer(VkRenderPass SceneRenderPass, VkImageView SceneImageView, VkImageView depthImageView);
	void createGeometryPassFrameBuffer(VkRenderPass GeoRenderPass, VkImageView GeometryImageView, VkImageView positionImageView, 
		VkImageView normalImageView, VkImageView albedoImageView, VkImageView depthImageView);
	
	void createSSAOQuadFrameBuffer(VkRenderPass SSAORenderPass, VkImageView SSAOImageView);
	void createSSAOLightingQuadFrameBuffer(VkRenderPass SSAOLightRenderPass, VkImageView SSAOLightImageView);

	std::vector<VkFramebuffer> getSwapChainFramebuffers() { return swapChainFramebuffers; }
	VkFramebuffer GetDisplaySceneFramebuffer() { return displaySceneFramebuffer; }

	void createShadowFramebuffer(VkRenderPass renderPass, VkImageView shadowImageView);
	void createSceneFramebuffer(VkRenderPass SceneRenderPass, VkImageView sceneImageView, VkImageView depthImageView);

	VkFramebuffer getShadowFramebuffer() { return shadowFramebuffer; }
	VkFramebuffer GetSceneFrameBuffer() { return SceneFrameBuffer; }
	VkFramebuffer GetGeometryPassFrameBuffer() { return GeometryPassFrameBuffer; }
	VkFramebuffer GetSSAOFramebuffer() { return SSAOFramebuffer; }
	VkFramebuffer GetSSAOLightingFramebuffer() { return SSAOLightingFramebuffer; }


private:
	std::vector<VkFramebuffer> swapChainFramebuffers;
	std::vector<VkImageView> swapChainImageViews;

	VkFramebuffer shadowFramebuffer;
	VkFramebuffer displaySceneFramebuffer;
	VkFramebuffer SceneFrameBuffer;
	VkFramebuffer GeometryPassFrameBuffer;
	VkFramebuffer SSAOFramebuffer;
	VkFramebuffer SSAOLightingFramebuffer;

	VkRenderPass renderPass;

	VkExtent2D swapChainExtent;
	VkDevice device;
};