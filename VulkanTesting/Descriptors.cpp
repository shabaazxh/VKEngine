#include "Descriptors.h"
#include "Object.h"



Descriptors::Descriptors(
	VkDevice device, 
	std::vector<VkImage> swapChainImages, 
	std::vector<VkBuffer> uniformBuffers,
	std::vector<VkBuffer> LightBuffers, 
	std::vector<VkBuffer> kernelBuffers,
	std::vector<VkBuffer> Light_2_Buffers,
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
	VkImageView FloorDiffuseTexture,
	VkImageView FloorSpecTexture,
	VkImageView AOTextureView,
	VkImageView EmissionTextureView,
	VkImageView depthImageView,
	VkImageView positionsImageView,
	VkImageView HDRImageView)
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
	this->depthImageView = depthImageView;
	this->positionsImageView = positionsImageView;
	this->HDRImageView = HDRImageView;
	this->Light_2_Buffers = Light_2_Buffers;
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

void Descriptors::ImGuiCreateDescriptorPool() {

		VkDescriptorPoolSize pool_sizes[] = {
			{VK_DESCRIPTOR_TYPE_SAMPLER,                1000},
			{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000},
			{VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,          1000},
			{VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,          1000},
			{VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER,   1000},
			{VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER,   1000},
			{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,         1000},
			{VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,         1000},
			{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000},
			{VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000},
			{VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,       1000}
		};

	VkDescriptorPoolCreateInfo pool_info = {};
	pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
	pool_info.maxSets = 1000;
	pool_info.poolSizeCount = std::size(pool_sizes);
	pool_info.pPoolSizes = pool_sizes;

	if (vkCreateDescriptorPool(device, &pool_info, nullptr, &m_ImGuiDescriptorPool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create ImGui Descriptor pool");
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

		// Second light
		VkDescriptorBufferInfo Light_2_BufferInfo{};
		Light_2_BufferInfo.buffer = Light_2_Buffers[i];
		Light_2_BufferInfo.offset = 0;
		Light_2_BufferInfo.range = sizeof(Light);

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

		VkDescriptorImageInfo HDRImageInfo{};
		HDRImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		HDRImageInfo.imageView = HDRImageView;
		HDRImageInfo.sampler = textureSampler;

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

		VkWriteDescriptorSet HDRImageDescriptorWrite{};
		HDRImageDescriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		HDRImageDescriptorWrite.dstSet = descriptorSets[i];
		HDRImageDescriptorWrite.dstBinding = 12;
		HDRImageDescriptorWrite.dstArrayElement = 0;
		HDRImageDescriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		HDRImageDescriptorWrite.descriptorCount = 1;
		HDRImageDescriptorWrite.pImageInfo = &HDRImageInfo;

		VkWriteDescriptorSet Light_2_DescriptorWrite{};
		Light_2_DescriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		Light_2_DescriptorWrite.dstSet = descriptorSets[i];
		Light_2_DescriptorWrite.dstBinding = 13;
		Light_2_DescriptorWrite.dstArrayElement = 0;
		Light_2_DescriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		Light_2_DescriptorWrite.descriptorCount = 1;
		Light_2_DescriptorWrite.pBufferInfo = &Light_2_BufferInfo;

		std::array<VkWriteDescriptorSet, 9> writeDescriptorSet = { descriptorWrite, LightDescriptorWrite, 
			DiffuseTextureDescriptorWrite, NormalsTextureDescriptorWrite, 
			AOTextureDescriptorWrite, EmissionTexutreImageWrite, depthMapDescriptorWrite, HDRImageDescriptorWrite,
		Light_2_DescriptorWrite};

		vkUpdateDescriptorSets(device, 9, writeDescriptorSet.data(), 0, nullptr);
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
		SSAOImageLightingInfo.imageView = SSAOLightImageView; //this should be SSAOLightImageView to blur the SSAO PASS! -> SSAOImageView
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

		VkDescriptorImageInfo depthMapImageInfo{};
		depthMapImageInfo.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
		depthMapImageInfo.imageView = depthImageView;
		depthMapImageInfo.sampler = textureSampler;

		VkDescriptorImageInfo depthToPos{};
		depthToPos.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		depthToPos.imageView = positionImageView;
		depthToPos.sampler = textureSampler;

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

		VkWriteDescriptorSet depthMapDescriptorWrite{};
		depthMapDescriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		depthMapDescriptorWrite.dstSet = SSAODescritporSets[i];
		depthMapDescriptorWrite.dstBinding = 6;
		depthMapDescriptorWrite.dstArrayElement = 0;
		depthMapDescriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		depthMapDescriptorWrite.descriptorCount = 1;
		depthMapDescriptorWrite.pImageInfo = &depthMapImageInfo;

		VkWriteDescriptorSet depthToPosDescriptorWrite{};
		depthToPosDescriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		depthToPosDescriptorWrite.dstSet = SSAODescritporSets[i];
		depthToPosDescriptorWrite.dstBinding = 7;
		depthToPosDescriptorWrite.dstArrayElement = 0;
		depthToPosDescriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		depthToPosDescriptorWrite.descriptorCount = 1;
		depthToPosDescriptorWrite.pImageInfo = &depthToPos;

		std::array<VkWriteDescriptorSet, 7> writeDescriptorSet = { positionSamplerDescriptorWrite,
		normalSamplerDescriptorWrite, noiseSamplerDescriptorWrite, KernelDescriptorWrite, cameraDescriptorWrite,
		depthMapDescriptorWrite, depthToPosDescriptorWrite };

		vkUpdateDescriptorSets(device, 7, writeDescriptorSet.data(), 0, nullptr);
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