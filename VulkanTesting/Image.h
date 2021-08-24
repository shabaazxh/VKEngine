#pragma once
#include <vulkan/vulkan.h>
#include "Buffer.h"


#include <iostream>
#include <memory>
#include <stb_image.h>

class ImageResource {
public:
	ImageResource(VkDevice device, VkCommandPool commandPool, VkQueue graphicsQueue, VkPhysicalDevice physicalDevice,
		VkFormat swapChainImageFormat);
	ImageResource(VkPhysicalDevice physicalDevice);
	ImageResource() = default;

	void createTextureImage();

	void createImage(uint32_t width, uint32_t height, VkFormat format,
		VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties,
		VkImage& image, VkDeviceMemory& imageMemory);

	void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout,
		VkImageLayout newLayout);

	void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);

	void createTextureImageView();

	VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);

	bool hasStencilComponent(VkFormat format);

	void createTextureSampler();

	//Get
	VkImage getTextureImage() { return textureImage; }
	VkDeviceMemory getTextureImageMemory() { return textureImageMemory; }
	VkImageView getTextureImageView() { return textureImageView; }

	//Normals
	VkImage getNormImage() { return normalImage; }
	VkImageView getNormImageView() { return normImageView; }

	VkSampler getSampler() { return textureSampler; }

	//Set relevant texture paths
	void setDiffusePath(std::string path) { diffuseTEXTURE = path; }
	void setNormPath(std::string path) { normTEXTURE = path; }

	//Command buffers
	VkCommandBuffer beginSingleTimeCommands();
	void endSingleTimeCommands(VkCommandBuffer commandBuffer);

	VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling,
		VkFormatFeatureFlags features);

	VkFormat findDepthFormat();

	//Shadow
	VkImage getShadowImage() { return shadowImage; }
	VkImageView getshadowImageView() { return shadowImageView; }
	VkDeviceMemory getShadowMemory() { return shadowMemory; }

	//Depth

	VkImage getDepthImage() { return depthImage; }
	VkImageView getDepthImageView() { return depthImageView; }
	VkDeviceMemory getDethMemory() { return depthImageMemory; }

	//SS
	VkImage GetSSImage() { return SSImage; }
	VkImageView GetSSImageView() { return SSImageView; }
	VkDeviceMemory GetSSImageMemory() { return SSMemory; }

	//Scene image
	VkImage GetSceneImage() { return sceneImage; }
	VkImageView GetSceneImageView() { return sceneImageView; }
	VkDeviceMemory GetSceneImageMemory() { return sceneImageMemory; }

	// SSAO Image
	VkImage GetSSAOImage() { return SSAOImage; }
	VkImageView GetSSAOImageView() { return SSAOImageView; }
	VkDeviceMemory GetSSAOImageMemory() { return SSAOImageMemory; }

	// position Image
	VkImage GetPositionImage() { return positionImage; }
	VkImageView GetPositionImageView() { return positionImageView; }
	VkDeviceMemory GetPositionImageMemory() { return positionImageMemory; }

	// normal Image
	VkImage GetNormalImage() { return normalImage; }
	VkImageView GetNormalImageView() { return normalImageView; }
	VkDeviceMemory GetNormalImageMemory() { return normalImageMemory; }

	// noise Image
	VkImage GetNoiseImage() { return noiseImage; }
	VkImageView GetNoiseImageView() { return noiseImageView; }
	VkDeviceMemory GetNoiseImageMemory() { return noiseImageMemory; }

	// Albedo Image
	VkImage GetAlbedoImage() { return albedoImage; }
	VkImageView GetAlbedoImageView() { return albedoImageView; }
	VkDeviceMemory GetAlbedoImageMemory() { return albedoImageMemory; }

	// SSAO Sampling Image
	VkImage GetSSAOSamplingImage() { return ssaoSamplingImage; }
	VkImageView GetSSAOSamplingImageView() { return ssaoSamplingImageView; }
	VkDeviceMemory GetSSAOSamplingImageMemory() { return ssaoSamplingImageMemory; }

	// SSAO Lighting Image
	VkImage GetSSAOLightingImage() { return ssaoLightingImage; }
	VkImageView GetSSAOLightingImageView() { return ssaoLightingImageView; }
	VkDeviceMemory GetSSAOLightingImageMemory() { return ssaoLightingImageMemory; }

	// Geometry image 
	VkImage GetGeometryImage() { return GeometryImage; }
	VkImageView GetGeometryImageView() { return GeometryImageView; }
	VkDeviceMemory GetGeometryImageMemory() { return GeometryImageMemory; }

	void createDepthResources(VkExtent2D swapChainExtent);

	// SCENE Sampler
	void createSceneSampler();
	VkSampler GetSceneSampler() { return sceneSampler; }

	void createRepeatSampler();
	VkSampler GetRepeatSampler() { return RepeatSampler; }


private:

	VkDevice device;
	VkPhysicalDevice physicalDevice;
	VkCommandPool commandPool;
	VkQueue graphicsQueue;
	VkFormat swapChainImageFormat;

	//Texture resources
	VkImage textureImage;
	//VkImage normalImage;

	VkImageView textureImageView;
	VkImageView normImageView;

	VkDeviceMemory textureImageMemory;
	VkSampler textureSampler;
	
	VkImage shadowImage;
	VkImageView shadowImageView;
	VkDeviceMemory shadowMemory;

	VkImage SSImage;
	VkImageView SSImageView;
	VkDeviceMemory SSMemory;

	VkImage depthImage;
	VkDeviceMemory depthImageMemory;
	VkImageView depthImageView;

	VkImage sceneImage;
	VkImageView sceneImageView;
	VkDeviceMemory sceneImageMemory;

	// position image
	VkImage positionImage;
	VkImageView positionImageView;
	VkDeviceMemory positionImageMemory;

	// normal image
	VkImage normalImage;
	VkImageView normalImageView;
	VkDeviceMemory normalImageMemory;

	// noise image
	VkImage noiseImage;
	VkImageView noiseImageView;
	VkDeviceMemory noiseImageMemory;

	VkImage SSAOImage;
	VkImageView SSAOImageView;
	VkDeviceMemory SSAOImageMemory;

	VkImage albedoImage;
	VkImageView albedoImageView;
	VkDeviceMemory albedoImageMemory;

	VkImage ssaoSamplingImage;
	VkImageView ssaoSamplingImageView;
	VkDeviceMemory ssaoSamplingImageMemory;

	VkImage ssaoLightingImage;
	VkImageView ssaoLightingImageView;
	VkDeviceMemory ssaoLightingImageMemory;

	VkImage GeometryImage;
	VkImageView GeometryImageView;
	VkDeviceMemory GeometryImageMemory;

	std::string diffuseTEXTURE;
	std::string normTEXTURE;

	VkSampler sceneSampler;
	VkSampler RepeatSampler;
}; 