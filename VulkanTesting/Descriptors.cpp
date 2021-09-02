#include "Descriptors.h"
#include "Object.h"



Descriptors::Descriptors(
	VkDevice device, 
	std::vector<VkImage> swapChainImages, 
	std::vector<VkBuffer> uniformBuffers,
	std::vector<VkBuffer> LightBuffers, 
	std::vector<VkBuffer> kernelBuffers,
	VkDescriptorSetLayout descriptorSetLayout,
	VkDescriptorSetLayout sceneDescriptorSetLayout,
	VkDescriptorSetLayout SSAOLayout,
	VkDescriptorSetLayout SSAOBlurLayout,
	VkImageView SSAOImageView,
	VkImageView AlbedoImageView,
	VkImageView ssaoSamplingImageView,
	VkImageView textureImageView, 
	VkImageView specImageView,
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
	VkImageView DiffuseTextureImageView,
	VkImageView specTextureImageView,
	std::vector<VkBuffer> FloorUniformBuffer,
	std::vector<VkBuffer> FloorLightBuffer,
	VkImageView FloorDiffuseTexture,
	VkImageView FloorSpecTexture,
	VkImageView AOTextureView,
	VkImageView EmissionTextureView)
{
	this->device = device;
	this->swapChainImages = swapChainImages;
	this->uniformBuffers = uniformBuffers;
	this->LightBuffers = LightBuffers;
	this->descriptorSetLayout = descriptorSetLayout;
	this->textureImageView = textureImageView;
	this->textureSampler = textureSampler;
	this->normImageView = normImageView;
	this->shadowImageView = shadowImageView;
	this->sceneSampler = sceneSampler;
	this->sceneImageView = sceneImageView;
	this->FloorUniformBuffer = FloorUniformBuffer;
	this->FloorLightBuffer = FloorLightBuffer;
	this->sceneDescriptorSetLayout = sceneDescriptorSetLayout;
	this->SSAOLayout = SSAOLayout;
	this->SSAOImageView = SSAOImageView;
	this->positionImageView = positionImageView;
	this->normalImageView = normalImageView;
	this->noiseImageView = noiseImageView;
	this->kernelBuffers = kernelBuffers;
	this->SSAOBlurLayout = SSAOBlurLayout;
	this->AlbedoImageView = AlbedoImageView;
	this->ssaoSamplingImageView = ssaoSamplingImageView;
	this->SSAOLightImageView = SSAOLightImageView;
	this->GeoImageView = GeoImageView;
	this->RepeatSampler = RepeatSampler;
	this->DiffuseTextureImageView = DiffuseTextureImageView;
	this->specTextureImageView = specTextureImageView;
	this->FloorDiffuseTexture = FloorDiffuseTexture;
	this->FloorSpecTexture = FloorSpecTexture;
	this->AOTextureView = AOTextureView;
	this->EmissionTextureView = EmissionTextureView;
}

void Descriptors::createDescriptorPool() {

	std::array<VkDescriptorPoolSize, 2>  poolSizes{};
	poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSizes[0].descriptorCount = 20;
	poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	poolSizes[1].descriptorCount = 20;

	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
	poolInfo.pPoolSizes = poolSizes.data();
	poolInfo.maxSets = 40;

	if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor pool!");
	}
}

void Descriptors::createDescriptorSets() {

	std::vector<VkDescriptorSetLayout> layouts(swapChainImages.size(), descriptorSetLayout);
	std::vector<VkDescriptorSetLayout> SceneLayout(swapChainImages.size(), sceneDescriptorSetLayout);
	std::vector<VkDescriptorSetLayout> SSAODescriptorLayoutInfo(swapChainImages.size(), SSAOLayout);
	std::vector<VkDescriptorSetLayout> SSAOBlurDescriptorLayoutInfo(swapChainImages.size(), SSAOBlurLayout);


	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = descriptorPool;
	allocInfo.descriptorSetCount = static_cast<uint32_t>(swapChainImages.size());
	allocInfo.pSetLayouts = layouts.data();

	// Scene descriptor set
	VkDescriptorSetAllocateInfo sceneDescriptorSetAlloc{};
	sceneDescriptorSetAlloc.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	sceneDescriptorSetAlloc.descriptorPool = descriptorPool;
	sceneDescriptorSetAlloc.descriptorSetCount = static_cast<uint32_t>(swapChainImages.size());
	sceneDescriptorSetAlloc.pSetLayouts = SceneLayout.data();

	// SSAO descriptor set
	VkDescriptorSetAllocateInfo ssaoDescriptorSetAlloc{};
	ssaoDescriptorSetAlloc.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	ssaoDescriptorSetAlloc.descriptorPool = descriptorPool;
	ssaoDescriptorSetAlloc.descriptorSetCount = static_cast<uint32_t>(swapChainImages.size());
	ssaoDescriptorSetAlloc.pSetLayouts = SSAODescriptorLayoutInfo.data();

	// SSAO descriptor set
	VkDescriptorSetAllocateInfo SSAOBlurDescriptorSetAlloc{};
	SSAOBlurDescriptorSetAlloc.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	SSAOBlurDescriptorSetAlloc.descriptorPool = descriptorPool;
	SSAOBlurDescriptorSetAlloc.descriptorSetCount = static_cast<uint32_t>(swapChainImages.size());
	SSAOBlurDescriptorSetAlloc.pSetLayouts = SSAOBlurDescriptorLayoutInfo.data();

	descriptorSets.resize(swapChainImages.size());
	sceneDescriptorSets.resize(swapChainImages.size());
	SSAODescritporSets.resize(swapChainImages.size());
	SSAOBlurDescritporSets.resize(swapChainImages.size());
	FloorDescriptorSet.resize(swapChainImages.size());

	if (vkAllocateDescriptorSets(device, &allocInfo, descriptorSets.data()) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate descriptor sets!");
	}

	if (vkAllocateDescriptorSets(device, &sceneDescriptorSetAlloc, sceneDescriptorSets.data()) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate scene descriptor sets!");
	}

	if (vkAllocateDescriptorSets(device, &ssaoDescriptorSetAlloc, SSAODescritporSets.data()) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate SSAO descriptor sets!");
	}

	if (vkAllocateDescriptorSets(device, &SSAOBlurDescriptorSetAlloc, SSAOBlurDescritporSets.data()) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate SSAO descriptor sets!");
	}

	if (vkAllocateDescriptorSets(device, &allocInfo, FloorDescriptorSet.data()) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate floor descriptor sets!");
	}

	for (size_t i = 0; i < swapChainImages.size(); i++) {

		VkDescriptorBufferInfo bufferInfo{};
		bufferInfo.buffer = uniformBuffers[i];
		bufferInfo.offset = 0;
		bufferInfo.range = sizeof(UniformBufferObject);

		//Lighting
		VkDescriptorBufferInfo LightBufferInfo{};
		LightBufferInfo.buffer = LightBuffers[i];
		LightBufferInfo.offset = 0;
		LightBufferInfo.range = sizeof(Light);

		VkDescriptorImageInfo depthMapImageInfo{};
		depthMapImageInfo.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
		depthMapImageInfo.imageView = shadowImageView;
		depthMapImageInfo.sampler = textureSampler;

		// Diffuse texture here
		VkDescriptorImageInfo DiffuseTextureInfo{};
		DiffuseTextureInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		DiffuseTextureInfo.imageView = DiffuseTextureImageView;
		DiffuseTextureInfo.sampler = textureSampler;

		// Specular texture here
		VkDescriptorImageInfo SpecularTextureInfo{};
		SpecularTextureInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		SpecularTextureInfo.imageView = specTextureImageView;
		SpecularTextureInfo.sampler = RepeatSampler;

		VkDescriptorImageInfo AOTextureInfo{};
		AOTextureInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		AOTextureInfo.imageView = AOTextureView;
		AOTextureInfo.sampler = RepeatSampler;

		VkDescriptorImageInfo EmissionTextureInfo{};
		EmissionTextureInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		EmissionTextureInfo.imageView = EmissionTextureView;
		EmissionTextureInfo.sampler = RepeatSampler;

		VkWriteDescriptorSet descriptorWrite{};
		descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrite.dstSet = descriptorSets[i];
		descriptorWrite.dstBinding = 0;
		descriptorWrite.dstArrayElement = 0;
		descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrite.descriptorCount = 1;
		descriptorWrite.pBufferInfo = &bufferInfo;

		VkWriteDescriptorSet LightDescriptorWrite{};
		LightDescriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		LightDescriptorWrite.dstSet = descriptorSets[i];
		LightDescriptorWrite.dstBinding = 1;
		LightDescriptorWrite.dstArrayElement = 0;
		LightDescriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		LightDescriptorWrite.descriptorCount = 1;
		LightDescriptorWrite.pBufferInfo = &LightBufferInfo;

		// Diffuse texture descriptor write
		VkWriteDescriptorSet DiffuseTextureDescriptorWrite{};
		DiffuseTextureDescriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		DiffuseTextureDescriptorWrite.dstSet = descriptorSets[i];
		DiffuseTextureDescriptorWrite.dstBinding = 3;
		DiffuseTextureDescriptorWrite.dstArrayElement = 0;
		DiffuseTextureDescriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		DiffuseTextureDescriptorWrite.descriptorCount = 1;
		DiffuseTextureDescriptorWrite.pImageInfo = &DiffuseTextureInfo;

		VkWriteDescriptorSet NormalsTextureDescriptorWrite{};
		NormalsTextureDescriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		NormalsTextureDescriptorWrite.dstSet = descriptorSets[i];
		NormalsTextureDescriptorWrite.dstBinding = 9;
		NormalsTextureDescriptorWrite.dstArrayElement = 0;
		NormalsTextureDescriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		NormalsTextureDescriptorWrite.descriptorCount = 1;
		NormalsTextureDescriptorWrite.pImageInfo = &SpecularTextureInfo;

		VkWriteDescriptorSet AOTextureDescriptorWrite{};
		AOTextureDescriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		AOTextureDescriptorWrite.dstSet = descriptorSets[i];
		AOTextureDescriptorWrite.dstBinding = 10;
		AOTextureDescriptorWrite.dstArrayElement = 0;
		AOTextureDescriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		AOTextureDescriptorWrite.descriptorCount = 1;
		AOTextureDescriptorWrite.pImageInfo = &AOTextureInfo;

		VkWriteDescriptorSet EmissionTexutreImageWrite{};
		EmissionTexutreImageWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		EmissionTexutreImageWrite.dstSet = descriptorSets[i];
		EmissionTexutreImageWrite.dstBinding = 11;
		EmissionTexutreImageWrite.dstArrayElement = 0;
		EmissionTexutreImageWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		EmissionTexutreImageWrite.descriptorCount = 1;
		EmissionTexutreImageWrite.pImageInfo = &EmissionTextureInfo;

		VkWriteDescriptorSet depthMapDescriptorWrite{};
		depthMapDescriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		depthMapDescriptorWrite.dstSet = descriptorSets[i];
		depthMapDescriptorWrite.dstBinding = 4;
		depthMapDescriptorWrite.dstArrayElement = 0;
		depthMapDescriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		depthMapDescriptorWrite.descriptorCount = 1;
		depthMapDescriptorWrite.pImageInfo = &depthMapImageInfo;

		std::array<VkWriteDescriptorSet, 7> writeDescriptorSet = { descriptorWrite, LightDescriptorWrite, 
			DiffuseTextureDescriptorWrite, NormalsTextureDescriptorWrite, 
			AOTextureDescriptorWrite, EmissionTexutreImageWrite, depthMapDescriptorWrite };

		vkUpdateDescriptorSets(device, 7, writeDescriptorSet.data(), 0, nullptr);
	}

	// Floor descriptors 
	for (size_t i = 0; i < swapChainImages.size(); i++) {

		VkDescriptorBufferInfo bufferInfo{};
		bufferInfo.buffer = uniformBuffers[i];
		bufferInfo.offset = 0;
		bufferInfo.range = sizeof(UniformBufferObject);

		//Lighting
		VkDescriptorBufferInfo LightBufferInfo{};
		LightBufferInfo.buffer = LightBuffers[i];
		LightBufferInfo.offset = 0;
		LightBufferInfo.range = sizeof(Light);

		VkDescriptorImageInfo depthMapImageInfo{};
		depthMapImageInfo.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
		depthMapImageInfo.imageView = shadowImageView;
		depthMapImageInfo.sampler = textureSampler;

		// Diffuse texture here
		VkDescriptorImageInfo DiffuseTextureInfo{};
		DiffuseTextureInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		DiffuseTextureInfo.imageView = FloorDiffuseTexture;
		DiffuseTextureInfo.sampler = RepeatSampler;

		VkDescriptorImageInfo AOTextureInfo{};
		AOTextureInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		AOTextureInfo.imageView = AOTextureView;
		AOTextureInfo.sampler = RepeatSampler;

		// Normals texture here
		VkDescriptorImageInfo specTextureInfo{};
		specTextureInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		specTextureInfo.imageView = FloorSpecTexture;
		specTextureInfo.sampler = RepeatSampler;

		VkDescriptorImageInfo EmissionTextureInfo{};
		EmissionTextureInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		EmissionTextureInfo.imageView = EmissionTextureView;
		EmissionTextureInfo.sampler = RepeatSampler;

		VkWriteDescriptorSet descriptorWrite{};
		descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrite.dstSet = FloorDescriptorSet[i];
		descriptorWrite.dstBinding = 0;
		descriptorWrite.dstArrayElement = 0;
		descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrite.descriptorCount = 1;
		descriptorWrite.pBufferInfo = &bufferInfo;

		VkWriteDescriptorSet LightDescriptorWrite{};
		LightDescriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		LightDescriptorWrite.dstSet = FloorDescriptorSet[i];
		LightDescriptorWrite.dstBinding = 1;
		LightDescriptorWrite.dstArrayElement = 0;
		LightDescriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		LightDescriptorWrite.descriptorCount = 1;
		LightDescriptorWrite.pBufferInfo = &LightBufferInfo;

		// Diffuse texture descriptor write
		VkWriteDescriptorSet DiffuseTextureDescriptorWrite{};
		DiffuseTextureDescriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		DiffuseTextureDescriptorWrite.dstSet = FloorDescriptorSet[i];
		DiffuseTextureDescriptorWrite.dstBinding = 3;
		DiffuseTextureDescriptorWrite.dstArrayElement = 0;
		DiffuseTextureDescriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		DiffuseTextureDescriptorWrite.descriptorCount = 1;
		DiffuseTextureDescriptorWrite.pImageInfo = &DiffuseTextureInfo;

		VkWriteDescriptorSet NormalsTextureDescriptorWrite{};
		NormalsTextureDescriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		NormalsTextureDescriptorWrite.dstSet = FloorDescriptorSet[i];
		NormalsTextureDescriptorWrite.dstBinding = 9;
		NormalsTextureDescriptorWrite.dstArrayElement = 0;
		NormalsTextureDescriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		NormalsTextureDescriptorWrite.descriptorCount = 1;
		NormalsTextureDescriptorWrite.pImageInfo = &specTextureInfo;

		VkWriteDescriptorSet AOTextureDescriptorWrite{};
		AOTextureDescriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		AOTextureDescriptorWrite.dstSet = FloorDescriptorSet[i];
		AOTextureDescriptorWrite.dstBinding = 10;
		AOTextureDescriptorWrite.dstArrayElement = 0;
		AOTextureDescriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		AOTextureDescriptorWrite.descriptorCount = 1;
		AOTextureDescriptorWrite.pImageInfo = &AOTextureInfo;

		VkWriteDescriptorSet EmissionTexutreImageWrite{};
		EmissionTexutreImageWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		EmissionTexutreImageWrite.dstSet = descriptorSets[i];
		EmissionTexutreImageWrite.dstBinding = 11;
		EmissionTexutreImageWrite.dstArrayElement = 0;
		EmissionTexutreImageWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		EmissionTexutreImageWrite.descriptorCount = 1;
		EmissionTexutreImageWrite.pImageInfo = &EmissionTextureInfo;

		VkWriteDescriptorSet depthMapDescriptorWrite{};
		depthMapDescriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		depthMapDescriptorWrite.dstSet = FloorDescriptorSet[i];
		depthMapDescriptorWrite.dstBinding = 4;
		depthMapDescriptorWrite.dstArrayElement = 0;
		depthMapDescriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		depthMapDescriptorWrite.descriptorCount = 1;
		depthMapDescriptorWrite.pImageInfo = &depthMapImageInfo;

		std::array<VkWriteDescriptorSet, 7> writeDescriptorSet = { descriptorWrite, LightDescriptorWrite,
			DiffuseTextureDescriptorWrite, NormalsTextureDescriptorWrite,
			AOTextureDescriptorWrite, EmissionTexutreImageWrite, depthMapDescriptorWrite };

		vkUpdateDescriptorSets(device, 7, writeDescriptorSet.data(), 0, nullptr);
	}


	/** Scene Descriptor */
	for (size_t i = 0; i < swapChainImages.size(); i++) {

		VkDescriptorImageInfo sceneImageInfo{};
		sceneImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL; //VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
		sceneImageInfo.imageView = sceneImageView;
		sceneImageInfo.sampler = sceneSampler;

		VkDescriptorImageInfo SSAOImageLightingInfo{};
		SSAOImageLightingInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL; //VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
		SSAOImageLightingInfo.imageView = SSAOLightImageView;
		SSAOImageLightingInfo.sampler = sceneSampler;

		VkWriteDescriptorSet sceneImageDescriptorWrite{};
		sceneImageDescriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		sceneImageDescriptorWrite.dstSet = sceneDescriptorSets[i];
		sceneImageDescriptorWrite.dstBinding = 1;
		sceneImageDescriptorWrite.dstArrayElement = 0;
		sceneImageDescriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		sceneImageDescriptorWrite.descriptorCount = 1;
		sceneImageDescriptorWrite.pImageInfo = &sceneImageInfo;

		VkWriteDescriptorSet SSAOImageDescriptorWrite{};
		SSAOImageDescriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		SSAOImageDescriptorWrite.dstSet = sceneDescriptorSets[i];
		SSAOImageDescriptorWrite.dstBinding = 2;
		SSAOImageDescriptorWrite.dstArrayElement = 0;
		SSAOImageDescriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		SSAOImageDescriptorWrite.descriptorCount = 1;
		SSAOImageDescriptorWrite.pImageInfo = &SSAOImageLightingInfo;

		std::array<VkWriteDescriptorSet, 2> writeDescriptorSet = { sceneImageDescriptorWrite, SSAOImageDescriptorWrite };	

		vkUpdateDescriptorSets(device, 2, writeDescriptorSet.data(), 0, nullptr);
	}

	/** SSAO Descriptor */
	for (size_t i = 0; i < swapChainImages.size(); i++) {

		VkDescriptorBufferInfo cameraUniform{};
		cameraUniform.buffer = uniformBuffers[i];
		cameraUniform.offset = 0;
		cameraUniform.range = sizeof(UniformBufferObject);

		VkDescriptorBufferInfo KernelBufferInfo{};
		KernelBufferInfo.buffer = kernelBuffers[i];
		KernelBufferInfo.offset = 0;
		KernelBufferInfo.range = sizeof(KernelSample);

		VkDescriptorImageInfo positionsImageInfo{};
		positionsImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		positionsImageInfo.imageView = positionImageView;
		positionsImageInfo.sampler = sceneSampler;

		VkDescriptorImageInfo normalsImageInfo{};
		normalsImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		normalsImageInfo.imageView = normalImageView;
		normalsImageInfo.sampler = sceneSampler;

		VkDescriptorImageInfo noiseImageInfo{};
		noiseImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL; 
		noiseImageInfo.imageView = noiseImageView;
		noiseImageInfo.sampler = RepeatSampler;

		VkWriteDescriptorSet positionSamplerDescriptorWrite{};
		positionSamplerDescriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		positionSamplerDescriptorWrite.dstSet = SSAODescritporSets[i];
		positionSamplerDescriptorWrite.dstBinding = 1;
		positionSamplerDescriptorWrite.dstArrayElement = 0;
		positionSamplerDescriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		positionSamplerDescriptorWrite.descriptorCount = 1;
		positionSamplerDescriptorWrite.pImageInfo = &positionsImageInfo;

		VkWriteDescriptorSet normalSamplerDescriptorWrite{};
		normalSamplerDescriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		normalSamplerDescriptorWrite.dstSet = SSAODescritporSets[i];
		normalSamplerDescriptorWrite.dstBinding = 2;
		normalSamplerDescriptorWrite.dstArrayElement = 0;
		normalSamplerDescriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		normalSamplerDescriptorWrite.descriptorCount = 1;
		normalSamplerDescriptorWrite.pImageInfo = &normalsImageInfo;

		VkWriteDescriptorSet noiseSamplerDescriptorWrite{};
		noiseSamplerDescriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		noiseSamplerDescriptorWrite.dstSet = SSAODescritporSets[i];
		noiseSamplerDescriptorWrite.dstBinding = 3;
		noiseSamplerDescriptorWrite.dstArrayElement = 0;
		noiseSamplerDescriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		noiseSamplerDescriptorWrite.descriptorCount = 1;
		noiseSamplerDescriptorWrite.pImageInfo = &noiseImageInfo;

		VkWriteDescriptorSet KernelDescriptorWrite{};
		KernelDescriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		KernelDescriptorWrite.dstSet = SSAODescritporSets[i];
		KernelDescriptorWrite.dstBinding = 4;
		KernelDescriptorWrite.dstArrayElement = 0;
		KernelDescriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		KernelDescriptorWrite.descriptorCount = 1;
		KernelDescriptorWrite.pBufferInfo = &KernelBufferInfo;

		VkWriteDescriptorSet cameraDescriptorWrite{};
		cameraDescriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		cameraDescriptorWrite.dstSet = SSAODescritporSets[i];
		cameraDescriptorWrite.dstBinding = 5;
		cameraDescriptorWrite.dstArrayElement = 0;
		cameraDescriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		cameraDescriptorWrite.descriptorCount = 1;
		cameraDescriptorWrite.pBufferInfo = &cameraUniform;

		std::array<VkWriteDescriptorSet, 5> writeDescriptorSet = { positionSamplerDescriptorWrite,
		normalSamplerDescriptorWrite, noiseSamplerDescriptorWrite, KernelDescriptorWrite, cameraDescriptorWrite };

		vkUpdateDescriptorSets(device, 5, writeDescriptorSet.data(), 0, nullptr);
	}

	/** SSAO Lighting Descriptor */
	for (size_t i = 0; i < swapChainImages.size(); i++) {

		VkDescriptorBufferInfo LightUniform{};
		LightUniform.buffer = LightBuffers[i];
		LightUniform.offset = 0;
		LightUniform.range = sizeof(Light);

		VkDescriptorImageInfo positionsImageInfo{};
		positionsImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL; //VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
		positionsImageInfo.imageView = positionImageView;
		positionsImageInfo.sampler = sceneSampler;

		VkDescriptorImageInfo normalsImageInfo{};
		normalsImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL; //VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
		normalsImageInfo.imageView = normalImageView;
		normalsImageInfo.sampler = sceneSampler;

		VkDescriptorImageInfo albedoImage{};
		albedoImage.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL; //VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
		albedoImage.imageView = AlbedoImageView;
		albedoImage.sampler = sceneSampler;

		VkDescriptorImageInfo ssaoImage{};
		ssaoImage.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL; //VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
		ssaoImage.imageView = SSAOImageView;
		ssaoImage.sampler = sceneSampler;

		VkWriteDescriptorSet positionSamplerDescriptorWrite{};
		positionSamplerDescriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		positionSamplerDescriptorWrite.dstSet = SSAOBlurDescritporSets[i];
		positionSamplerDescriptorWrite.dstBinding = 1;
		positionSamplerDescriptorWrite.dstArrayElement = 0;
		positionSamplerDescriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		positionSamplerDescriptorWrite.descriptorCount = 1;
		positionSamplerDescriptorWrite.pImageInfo = &positionsImageInfo;

		VkWriteDescriptorSet normalSamplerDescriptorWrite{};
		normalSamplerDescriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		normalSamplerDescriptorWrite.dstSet = SSAOBlurDescritporSets[i];
		normalSamplerDescriptorWrite.dstBinding = 2;
		normalSamplerDescriptorWrite.dstArrayElement = 0;
		normalSamplerDescriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		normalSamplerDescriptorWrite.descriptorCount = 1;
		normalSamplerDescriptorWrite.pImageInfo = &normalsImageInfo;

		VkWriteDescriptorSet albedoSamplerDescriptorWrite{};
		albedoSamplerDescriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		albedoSamplerDescriptorWrite.dstSet = SSAOBlurDescritporSets[i];
		albedoSamplerDescriptorWrite.dstBinding = 3;
		albedoSamplerDescriptorWrite.dstArrayElement = 0;
		albedoSamplerDescriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		albedoSamplerDescriptorWrite.descriptorCount = 1;
		albedoSamplerDescriptorWrite.pImageInfo = &albedoImage;

		VkWriteDescriptorSet ssaoSamplerDescriptorWrite{};
		ssaoSamplerDescriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		ssaoSamplerDescriptorWrite.dstSet = SSAOBlurDescritporSets[i];
		ssaoSamplerDescriptorWrite.dstBinding = 4;
		ssaoSamplerDescriptorWrite.dstArrayElement = 0;
		ssaoSamplerDescriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		ssaoSamplerDescriptorWrite.descriptorCount = 1;
		ssaoSamplerDescriptorWrite.pImageInfo = &ssaoImage;

		VkWriteDescriptorSet LightSamplerDescriptorWrite{};
		LightSamplerDescriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		LightSamplerDescriptorWrite.dstSet = SSAOBlurDescritporSets[i];
		LightSamplerDescriptorWrite.dstBinding = 5;
		LightSamplerDescriptorWrite.dstArrayElement = 0;
		LightSamplerDescriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		LightSamplerDescriptorWrite.descriptorCount = 1;
		LightSamplerDescriptorWrite.pBufferInfo = &LightUniform;

		std::array<VkWriteDescriptorSet, 5> writeDescriptorSet = { positionSamplerDescriptorWrite,
		normalSamplerDescriptorWrite, albedoSamplerDescriptorWrite, ssaoSamplerDescriptorWrite, LightSamplerDescriptorWrite };

		vkUpdateDescriptorSets(device, 5, writeDescriptorSet.data(), 0, nullptr);
	}
}