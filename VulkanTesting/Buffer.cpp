#include "Buffer.h"

Buffer::Buffer(VkDevice device, VkPhysicalDevice physicalDevice, VkCommandPool commandPool, VkQueue graphicsQueue, std::vector<VkImage> swapChainImages, VkExtent2D swapChainExtent, std::array<glm::vec3, 3> cameraMovement) {
	this->device = device;
	this->physicalDevice = physicalDevice;
	this->commandPool = commandPool;
	this->graphicsQueue = graphicsQueue;
	this->swapChainImages = swapChainImages;
	this->swapChainExtent = swapChainExtent;
	this->cameraMovement = cameraMovement;
}

Buffer::Buffer(VkDevice device, VkPhysicalDevice physicalDevice, VkCommandPool commandPool, VkQueue graphicsQueue, std::vector<VkImage> swapChainImages, 
	VkExtent2D swapChainExtent, std::vector<VkDeviceMemory> uniformBuffersMemory, 
	std::vector<VkDeviceMemory> LightBuffersMemory, 
	std::vector<VkDeviceMemory> SSAOKenrnelBufferMemory,
	std::array<glm::vec3, 3> cameraMovement) {

	this->device = device;
	this->physicalDevice = physicalDevice;
	this->commandPool = commandPool;
	this->graphicsQueue = graphicsQueue;
	this->swapChainImages = swapChainImages;
	this->swapChainExtent = swapChainExtent;
	this->uniformBuffersMemory = uniformBuffersMemory;
	this->LightBuffersMemory = LightBuffersMemory;
	this->SSAOKenrnelBufferMemory = SSAOKenrnelBufferMemory;
	this->cameraMovement = cameraMovement;
}

Buffer::Buffer(VkDevice device, VkPhysicalDevice physicalDevice) {
	this->device = device;
	this->physicalDevice = physicalDevice;
}

 void Buffer::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, 
	VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory) {

	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateBuffer(device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
		throw std::runtime_error("failed to create buffer!");
	}

	//Getting memory requirements for the buffer
	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = findMemoryRequirements(memRequirements.memoryTypeBits, properties);

	if (vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate the buffer memory!");
	}

	// If memoery allocation is success->associate the memory to the buffer
	vkBindBufferMemory(device, buffer, bufferMemory, 0);

}

void Buffer::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {
	//Memory transfer opertations are executed using command buffers 
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = commandPool;
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer commandBuffer;
	vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

	//Recording the command buffer
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT; //Use command buffer once

	vkBeginCommandBuffer(commandBuffer, &beginInfo);

	VkBufferCopy copyRegion{};
	copyRegion.srcOffset = 0;
	copyRegion.dstOffset = 0;
	copyRegion.size = size;
	vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

	vkEndCommandBuffer(commandBuffer);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(graphicsQueue);

	vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);


}

void Buffer::createVertexBuffer(unsigned int sizeBuffer, std::vector<Vertex> info) {
	VkDeviceSize bufferSize = sizeBuffer;

	//On the CPU 
	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;

	createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
		VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	//Copy vertex data into vertex buffer
	void* data;
	vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, info.data(), (size_t) bufferSize);
	vkUnmapMemory(device, stagingBufferMemory);

	createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | 
		VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 
		vertexBuffer, vertexBufferMemory);

	copyBuffer(stagingBuffer, vertexBuffer, bufferSize);

	vkDestroyBuffer(device, stagingBuffer, nullptr);
	vkFreeMemory(device, stagingBufferMemory, nullptr);

}

void Buffer::createIndexBuffer(unsigned int sizeBuffer, std::vector<uint32_t> info) {
	VkDeviceSize bufferSize = sizeBuffer;

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	void* data;
	vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, info.data(), (size_t)bufferSize);
	vkUnmapMemory(device, stagingBufferMemory);

	createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffer, indexBufferMemory);

	copyBuffer(stagingBuffer, indexBuffer, bufferSize);

	vkDestroyBuffer(device,stagingBuffer, nullptr);
	vkFreeMemory(device, stagingBufferMemory, nullptr);

}

void Buffer::createUniformBuffer() {
	VkDeviceSize bufferSize = sizeof(UniformBufferObject);
	VkDeviceSize bufferSizeLight = sizeof(Light);
	VkDeviceSize SSAOBuffer = sizeof(KernelSample);

	uniformBuffers.resize(swapChainImages.size());
	uniformBuffersMemory.resize(swapChainImages.size());

	for (size_t i = 0; i < swapChainImages.size(); i++) {
		createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, 
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			uniformBuffers[i], uniformBuffersMemory[i]);
	}

	LightBuffers.resize(swapChainImages.size());
	LightBuffersMemory.resize(swapChainImages.size());

	for (size_t i = 0; i < swapChainImages.size(); i++) {
		createBuffer(bufferSizeLight, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			LightBuffers[i], LightBuffersMemory[i]);
	}

	SSAOKernelBuffers.resize(swapChainImages.size());
	SSAOKenrnelBufferMemory.resize(swapChainImages.size());

	for (size_t i = 0; i < swapChainImages.size(); i++) {
		createBuffer(SSAOBuffer, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			SSAOKernelBuffers[i], SSAOKenrnelBufferMemory[i]);
	}

}

void Buffer::updateUniformBuffers(uint32_t currentImage) {

	static auto startTime = std::chrono::high_resolution_clock::now();

	auto currentTime = std::chrono::high_resolution_clock::now();
	float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
	float spaceTime = glfwGetTime() / 5;

	float near_plane = 1.0f, far_plane = 12.f;

	float lightFOV = 45.0f;
	
	glm::vec4 lightpos = glm::vec4(-2.0f, 4.0f, 5.0f, 1.0f); //-2.0f, 9.0f, -8.0f, 1.0f :: //-2.0f, 5.0f, 5.0f, 1.0f

	// SSAO samples 
	KernelSample ks{};
	std::uniform_real_distribution<float> randomFloats(0.0, 1.0);
	std::default_random_engine generator;
	std::vector<glm::vec4> ssaoKernel;

	for (unsigned int i = 0; i < 64; ++i) {
		
		glm::vec4 sample(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, randomFloats(generator), 1.0);
		sample = glm::normalize(sample);
		sample *= randomFloats(generator);
		
		float scale = float(i) / 64.0;
		scale = lerp(0.1f, 1.0f, scale * scale);
		sample *= scale;
		ssaoKernel.push_back(sample);
	}

	// SSAO samples -> Transfer to the shader
	for (unsigned int i = 0; i < 64; i++) {
		ks.samples[i] = ssaoKernel[i];
	}

	// Model, View & Projection
	UniformBufferObject ubo{};
	ubo.model = glm::mat4(1.0f);
	ubo.model = glm::translate(ubo.model, glm::vec3(0.0f, 0.0f, 0.0f));
	ubo.view = glm::lookAt(cameraMovement[0], cameraMovement[0] * cameraMovement[1], cameraMovement[2]);
	ubo.proj = glm::perspective(glm::radians(fov), swapChainExtent.width / (float) swapChainExtent.height, 0.1f, 1000.0f);
	ubo.proj[1][1] *= -1;

	ks.projection = ubo.proj;

	// Handling Lighting
	Light lighting{};
	lighting.position = glm::vec4(lightpos);
	/*lighting.position.x = 1.0f + sin(glfwGetTime()) * 2.0f;
	lighting.position.y = sin(glfwGetTime() / 2.0f) * 1.0f;
	lighting.position.z = 1.0f + sin(glfwGetTime() / 2.0f) * 5.0f;*/
	//std::cout << lighting.position.y << std::endl;
	lighting.lightColor = glm::vec4(0.5f, 0.5f, 0.5f, 1.0); //0.5f, 0.5f, 0.5f, 1.0
	lighting.objectColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	lighting.viewPos = glm::vec4(cameraMovement[0], 0.0f);
	lighting.invertedNormals = false;
	lighting.Linear = 0.09;
	lighting.Quadratic = 0.032;

	glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, 10.0f, -10.0f, near_plane, far_plane);
	//glm::mat4 lightProjection = glm::perspective(glm::radians(45.f), 1.0f, near_plane, far_plane);
	glm::mat4 lightView = glm::lookAt(glm::vec3(lighting.position), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 depthModelMatrix = glm::mat4(1.0f);
	lighting.lightSpaceMatrix = lightProjection * lightView * depthModelMatrix;

	// Mapping to GPU
	void* data;
	vkMapMemory(device, uniformBuffersMemory[currentImage], 0, sizeof(ubo), 0, &data);
	memcpy(data, &ubo, sizeof(ubo));
	vkUnmapMemory(device, uniformBuffersMemory[currentImage]);

	vkMapMemory(device, LightBuffersMemory[currentImage], 0, sizeof(lighting), 0, &data);
	memcpy(data, &lighting, sizeof(lighting));
	vkUnmapMemory(device, LightBuffersMemory[currentImage]);

	vkMapMemory(device, SSAOKenrnelBufferMemory[currentImage], 0, sizeof(ks), 0, &data);
	memcpy(data, &ks, sizeof(ks));
	vkUnmapMemory(device, SSAOKenrnelBufferMemory[currentImage]);
}

uint32_t Buffer::findMemoryRequirements(uint32_t typeFilter, VkMemoryPropertyFlags properties) {

	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

	/* Make sure memory type is suitable and also check if memory allows is host visible using .propertyFlags */
	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
		if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties){
			return i;
		}
	}
}