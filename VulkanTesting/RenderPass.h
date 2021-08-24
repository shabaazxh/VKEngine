#pragma once


#include <vulkan/vulkan.h>
#include <stdexcept>
#include "Image.h"
#include <memory>
#include <array>

class RenderPass {
public:
	RenderPass(VkDevice device, VkFormat swapChainImageFormat);
	void createSceneRenderPass(VkFormat format);
	void createShadowRenderPass(VkFormat format);
	void createQuadRenderPass();
	void createGeometryPassRenderPass(VkFormat format);
	void createSSAORenderPass();
	void createSSAOLightingRenderPass();

	VkRenderPass GetSceneRenderPass() { return SceneRenderPass; }
	VkRenderPass GetShadowRenderPass() { return shadowPass; }
	VkRenderPass GetQuadRenderPass() { return QuadRenderPass; }
	VkRenderPass GetGeometryPassRenderPass() { return GeometryPassRenderPass; }
	VkRenderPass GetSSAORenderPass() { return SSAORenderPass; }
	VkRenderPass GetSSAOLightingRenderPass() { return SSAOLightingRenderPass; }


	

private:
	VkRenderPass SceneRenderPass;
	VkRenderPass shadowPass;
	VkRenderPass QuadRenderPass;
	VkRenderPass GeometryPassRenderPass;
	VkRenderPass SSAORenderPass;
	VkRenderPass SSAOLightingRenderPass;

	VkFormat swapChainImageFormat;
	VkDevice device;

	std::unique_ptr<ImageResource> imageHelper;

};