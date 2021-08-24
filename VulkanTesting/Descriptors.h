#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <stdexcept>
#include <array>

class Descriptors {
public:
	Descriptors(VkDevice device, 
		std::vector<VkImage> swapChainImages, 
		std::vector<VkBuffer> uniformBuffers, 
		std::vector<VkBuffer> LightBuffers,
		std::vector<VkBuffer> kernelBuffers,
		VkDescriptorSetLayout descriptorSetLayout,
		VkDescriptorSetLayout sceneDescriptorSetLayout,
		VkDescriptorSetLayout SSAOLayout,
		VkDescriptorSetLayout SSAOLightingLayout,
		VkImageView SSAOImageView,
		VkImageView AlbedoImageView,
		VkImageView ssaoSamplingImageView,
		VkImageView textureImageView, 
		VkImageView normImageView, 
		VkImageView shadowImageView,
		VkImageView positionImageView,
		VkImageView normalImageView,
		VkImageView noiseImageView,
		VkImageView SSAOLightImageView,
		VkImageView GeoImageView,
		VkSampler textureSampler,
		VkSampler sceneSampler,
		VkImageView sceneImageView,
		VkSampler RepeatSampler,
		std::vector<VkBuffer> FloorUniformBuffer,
		std::vector<VkBuffer> FloorLightBuffer);

	void createDescriptorPool();
	void createDescriptorSets();

	VkDescriptorPool getDescriptorPool() { return descriptorPool; }
	std::vector<VkDescriptorSet> getDescriptorSets() { return descriptorSets; }
	std::vector<VkDescriptorSet> GetSceneDescriptorSets() { return sceneDescriptorSets; }
	std::vector<VkDescriptorSet> GetSSAODescriptorSets() { return SSAODescritporSets; }
	std::vector<VkDescriptorSet> GetSSAOLightingDescriptorSets() { return SSAOLightingDescritporSets; }

private:
	VkDevice device;
	VkDescriptorSetLayout descriptorSetLayout;
	VkDescriptorSetLayout sceneDescriptorSetLayout;
	VkDescriptorSetLayout SSAOLightingLayout;

	std::vector<VkBuffer> uniformBuffers;
	std::vector<VkBuffer> LightBuffers;
	std::vector<VkBuffer> kernelBuffers;

	std::vector<VkBuffer> FloorUniformBuffer;
	std::vector<VkBuffer> FloorLightBuffer;

	std::vector<VkImage> swapChainImages;
	VkDescriptorPool descriptorPool;

	std::vector<VkDescriptorSet> descriptorSets;
	std::vector<VkDescriptorSet> sceneDescriptorSets;
	std::vector<VkDescriptorSet> SSAODescritporSets;
	std::vector<VkDescriptorSet> SSAOLightingDescritporSets;

	VkImageView textureImageView;
	VkImageView normImageView;
	VkImageView shadowImageView;
	VkImageView AlbedoImageView;
	VkImageView ssaoSamplingImageView;

	VkSampler textureSampler;

	// Scene info
	VkSampler sceneSampler;
	VkImageView sceneImageView;

	// SSAO 
	VkDescriptorSetLayout SSAOLayout;
	VkImageView SSAOImageView;

	VkImageView positionImageView;
	VkImageView normalImageView;
	VkImageView noiseImageView;
	VkImageView SSAOLightImageView;
	VkImageView GeoImageView;


	VkSampler RepeatSampler;

};