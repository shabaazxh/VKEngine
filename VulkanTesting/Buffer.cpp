#include "Buffer.h"

Buffer::Buffer(VkDevice device, VkPhysicalDevice physicalDevice) {
	this->device = device;
	this->physicalDevice = physicalDevice;
}

void Buffer::createVertexBuffer() {
	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = sizeof(ObjectInfo->getTriangleData()[0]) * ObjectInfo->getTriangleData().size();
	bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateBuffer(device, &bufferInfo, nullptr, &vertexBuffer) != VK_SUCCESS) {
		throw std::runtime_error("failed to create vertex buffer!");
	}

	//Getting buffer memory requirements to allocate memory for the buffer
	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(device, vertexBuffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = findMemoryRequirements(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
	| VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	if (vkAllocateMemory(device, &allocInfo, nullptr, &vertexBufferMemory) != VK_SUCCESS) {
		throw std::runtime_error("failed to allcoate vertex buffer memory!");
	}

	//If memoery allocation is success -> associate the memory to the buffer 
	vkBindBufferMemory(device, vertexBuffer, vertexBufferMemory, 0);
	
	//Copy vertex data into vertex buffer
	void* data;
	vkMapMemory(device, vertexBufferMemory, 0, bufferInfo.size, 0, &data);
	memcpy(data, ObjectInfo->getTriangleData().data(), (size_t) bufferInfo.size);
	vkUnmapMemory(device, vertexBufferMemory);

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