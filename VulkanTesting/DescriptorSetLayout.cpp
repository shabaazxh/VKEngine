#include "DescriptorSetLayout.h"

DescriptorSetLayout::DescriptorSetLayout(VkDevice device) {
	this->device = device;
}

void DescriptorSetLayout::createDescriptorSetLayout() {

	//Describe the use and which stage
	VkDescriptorSetLayoutBinding uboLayoutBinding{};
	uboLayoutBinding.binding = 0;
	uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uboLayoutBinding.descriptorCount = 1;
	uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
	uboLayoutBinding.pImmutableSamplers = nullptr;

	//Lighting
	VkDescriptorSetLayoutBinding LightLayoutBinding{};
	LightLayoutBinding.binding = 1;
	LightLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	LightLayoutBinding.descriptorCount = 1;
	LightLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
	LightLayoutBinding.pImmutableSamplers = nullptr;

	// Shadow sampler / Textures
	VkDescriptorSetLayoutBinding samplerLayoutBinding{};
	samplerLayoutBinding.binding = 2;
	samplerLayoutBinding.descriptorCount = 1;
	samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	samplerLayoutBinding.pImmutableSamplers = nullptr;
	samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	VkDescriptorSetLayoutBinding texutreLayoutBinding{};
	texutreLayoutBinding.binding = 3;
	texutreLayoutBinding.descriptorCount = 1;
	texutreLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	texutreLayoutBinding.pImmutableSamplers = nullptr;
	texutreLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	VkDescriptorSetLayoutBinding depthMapLayout{};
	depthMapLayout.binding = 4;
	depthMapLayout.descriptorCount = 1;
	depthMapLayout.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	depthMapLayout.pImmutableSamplers = nullptr;
	depthMapLayout.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	// sampling positions
	VkDescriptorSetLayoutBinding positionSampler{};
	positionSampler.binding = 5;
	positionSampler.descriptorCount = 1;
	positionSampler.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	positionSampler.pImmutableSamplers = nullptr;
	positionSampler.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	// sampling normals
	VkDescriptorSetLayoutBinding normalSampler{};
	normalSampler.binding = 6;
	normalSampler.descriptorCount = 1;
	normalSampler.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	normalSampler.pImmutableSamplers = nullptr;
	normalSampler.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	//sampling diffuse/albedo
	VkDescriptorSetLayoutBinding albedoSampler{};
	albedoSampler.binding = 7;
	albedoSampler.descriptorCount = 1;
	albedoSampler.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	albedoSampler.pImmutableSamplers = nullptr;
	albedoSampler.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	// sampling ssao
	VkDescriptorSetLayoutBinding ssaoSampler{};
	ssaoSampler.binding = 8;
	ssaoSampler.descriptorCount = 1;
	ssaoSampler.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	ssaoSampler.pImmutableSamplers = nullptr;
	ssaoSampler.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	//Normals
	VkDescriptorSetLayoutBinding NormalsTextureLayoutBinding{};
	NormalsTextureLayoutBinding.binding = 9;
	NormalsTextureLayoutBinding.descriptorCount = 1;
	NormalsTextureLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	NormalsTextureLayoutBinding.pImmutableSamplers = nullptr;
	NormalsTextureLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	VkDescriptorSetLayoutBinding aoSampler{};
	aoSampler.binding = 10;
	aoSampler.descriptorCount = 1;
	aoSampler.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	aoSampler.pImmutableSamplers = nullptr;
	aoSampler.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	VkDescriptorSetLayoutBinding EmissionTextureLayoutBinding{};
	EmissionTextureLayoutBinding.binding = 11;
	EmissionTextureLayoutBinding.descriptorCount = 1;
	EmissionTextureLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	EmissionTextureLayoutBinding.pImmutableSamplers = nullptr;
	EmissionTextureLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	VkDescriptorSetLayoutBinding HDRMapLayoutBinding{};
	HDRMapLayoutBinding.binding = 12;
	HDRMapLayoutBinding.descriptorCount = 1;
	HDRMapLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	HDRMapLayoutBinding.pImmutableSamplers = nullptr;
	HDRMapLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	VkDescriptorSetLayoutBinding Light_2_LayoutBinding{};
	Light_2_LayoutBinding.binding = 13;
	Light_2_LayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	Light_2_LayoutBinding.descriptorCount = 1;
	Light_2_LayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
	Light_2_LayoutBinding.pImmutableSamplers = nullptr;

	std::array<VkDescriptorSetLayoutBinding, 14> binings = { uboLayoutBinding, LightLayoutBinding, samplerLayoutBinding, 
		depthMapLayout, positionSampler, normalSampler, albedoSampler, ssaoSampler, texutreLayoutBinding,
	NormalsTextureLayoutBinding, aoSampler, EmissionTextureLayoutBinding, HDRMapLayoutBinding, Light_2_LayoutBinding };

	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = 14;
	layoutInfo.pBindings = binings.data();

	if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor set layout!");
	}
}

void DescriptorSetLayout::createQuadDescriptorSetLayout() {

	VkDescriptorSetLayoutBinding sceneSamplerLayoutBinding{};
	sceneSamplerLayoutBinding.binding = 1;
	sceneSamplerLayoutBinding.descriptorCount = 1;
	sceneSamplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	sceneSamplerLayoutBinding.pImmutableSamplers = nullptr;
	sceneSamplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	VkDescriptorSetLayoutBinding SSAOSamplerLayoutBinding{};
	SSAOSamplerLayoutBinding.binding = 2;
	SSAOSamplerLayoutBinding.descriptorCount = 1;
	SSAOSamplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	SSAOSamplerLayoutBinding.pImmutableSamplers = nullptr;
	SSAOSamplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	std::array<VkDescriptorSetLayoutBinding, 2> binings = { sceneSamplerLayoutBinding, SSAOSamplerLayoutBinding };

	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = 2;
	layoutInfo.pBindings = binings.data();

	if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &QuadDescriptorSetLayout) != VK_SUCCESS) {
		throw std::runtime_error("failed to create scene descriptor set layout!");
	}
}

void DescriptorSetLayout::createSSAODescriptorSetLayout() {

	VkDescriptorSetLayoutBinding positionSampler{};
	positionSampler.binding = 1;
	positionSampler.descriptorCount = 1;
	positionSampler.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	positionSampler.pImmutableSamplers = nullptr;
	positionSampler.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	VkDescriptorSetLayoutBinding normalSampler{};
	normalSampler.binding = 2;
	normalSampler.descriptorCount = 1;
	normalSampler.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	normalSampler.pImmutableSamplers = nullptr;
	normalSampler.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	VkDescriptorSetLayoutBinding noiseSampler{};
	noiseSampler.binding = 3;
	noiseSampler.descriptorCount = 1;
	noiseSampler.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	noiseSampler.pImmutableSamplers = nullptr;
	noiseSampler.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	VkDescriptorSetLayoutBinding kernelSampleLayout{};
	kernelSampleLayout.binding = 4;
	kernelSampleLayout.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	kernelSampleLayout.descriptorCount = 1;
	kernelSampleLayout.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_VERTEX_BIT;
	kernelSampleLayout.pImmutableSamplers = nullptr;

	VkDescriptorSetLayoutBinding cameraLayout{};
	cameraLayout.binding = 5;
	cameraLayout.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	cameraLayout.descriptorCount = 1;
	cameraLayout.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_VERTEX_BIT;
	cameraLayout.pImmutableSamplers = nullptr;

	VkDescriptorSetLayoutBinding depthImageLayout{};
	depthImageLayout.binding = 6;
	depthImageLayout.descriptorCount = 1;
	depthImageLayout.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	depthImageLayout.pImmutableSamplers = nullptr;
	depthImageLayout.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_VERTEX_BIT;

	VkDescriptorSetLayoutBinding depthToPosImageLayout{};
	depthToPosImageLayout.binding = 7;
	depthToPosImageLayout.descriptorCount = 1;
	depthToPosImageLayout.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	depthToPosImageLayout.pImmutableSamplers = nullptr;
	depthToPosImageLayout.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_VERTEX_BIT;

	std::array<VkDescriptorSetLayoutBinding, 7> binings = { positionSampler, normalSampler, noiseSampler, kernelSampleLayout,
	cameraLayout, depthImageLayout, depthToPosImageLayout };

	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = 7;
	layoutInfo.pBindings = binings.data();

	if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &SSAOLayout) != VK_SUCCESS) {
		throw std::runtime_error("failed to create scene descriptor set layout!");
	}
}


void DescriptorSetLayout::createSSAOBlurDescriptorSetLayout() {

	VkDescriptorSetLayoutBinding positionSampler{};
	positionSampler.binding = 1;
	positionSampler.descriptorCount = 1;
	positionSampler.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	positionSampler.pImmutableSamplers = nullptr;
	positionSampler.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	VkDescriptorSetLayoutBinding normalSampler{};
	normalSampler.binding = 2;
	normalSampler.descriptorCount = 1;
	normalSampler.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	normalSampler.pImmutableSamplers = nullptr;
	normalSampler.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	VkDescriptorSetLayoutBinding albedoSampler{};
	albedoSampler.binding = 3;
	albedoSampler.descriptorCount = 1;
	albedoSampler.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	albedoSampler.pImmutableSamplers = nullptr;
	albedoSampler.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	VkDescriptorSetLayoutBinding ssaoSampler{};
	ssaoSampler.binding = 4;
	ssaoSampler.descriptorCount = 1;
	ssaoSampler.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	ssaoSampler.pImmutableSamplers = nullptr;
	ssaoSampler.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	VkDescriptorSetLayoutBinding LightLayoutBinding{};
	LightLayoutBinding.binding = 5;
	LightLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	LightLayoutBinding.descriptorCount = 1;
	LightLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
	LightLayoutBinding.pImmutableSamplers = nullptr;

	VkDescriptorSetLayoutBinding sceneSampler{};
	sceneSampler.binding = 6;
	sceneSampler.descriptorCount = 1;
	sceneSampler.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	sceneSampler.pImmutableSamplers = nullptr;
	sceneSampler.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	std::array<VkDescriptorSetLayoutBinding, 6> binings = { positionSampler, normalSampler, albedoSampler, ssaoSampler, 
		LightLayoutBinding, sceneSampler };

	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = 6;
	layoutInfo.pBindings = binings.data();

	if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &SSAOBlurLayout) != VK_SUCCESS) {
		throw std::runtime_error("failed to create SSAO Lighting descriptor set layout!");
	}
}
