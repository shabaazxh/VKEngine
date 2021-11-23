#pragma once


#include <vulkan/vulkan.h>
#include <stdexcept>
#include "Image.h"
#include <memory>
#include <array>

class RenderPass {
public:
	RenderPass(VkDevice device, VkFormat swapChainImageFormat);
	//void createRenderPass(std::vector<VkImageView> images)
	void createRenderPass_Positions(VkFormat imageFormat);

	void createSceneRenderPass(VkFormat format);
	void createShadowRenderPass(VkFormat format);
	void createQuadRenderPass();
	void createGeometryPassRenderPass(VkFormat format);
	void createSSAORenderPass();
	void createSSAOBlurRenderPass();

	VkRenderPass GetSceneRenderPass() { return SceneRenderPass; }
	VkRenderPass GetShadowRenderPass() { return shadowPass; }
	VkRenderPass GetQuadRenderPass() { return QuadRenderPass; }
	VkRenderPass GetGeometryPassRenderPass() { return GeometryPassRenderPass; }
	VkRenderPass GetSSAORenderPass() { return SSAORenderPass; }
	VkRenderPass GetSSAOBlurRenderPass() { return SSAOBlurRenderPass; }
	VkRenderPass GetRenderPass() { return renderPass; }

	

private:
	VkRenderPass SceneRenderPass;
	VkRenderPass shadowPass;
	VkRenderPass QuadRenderPass;
	VkRenderPass GeometryPassRenderPass;
	VkRenderPass SSAORenderPass;
	VkRenderPass SSAOBlurRenderPass;

	VkFormat swapChainImageFormat;
	VkDevice device;

	VkRenderPass renderPass;
	std::unique_ptr<ImageResource> imageHelper;

};