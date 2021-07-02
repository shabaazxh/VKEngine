#pragma once

#include <vulkan/vulkan.h>
#include "Object.h"
#include <memory>
#include <stdexcept>

class Buffer {
public:
	Buffer(VkDevice device, VkPhysicalDevice physicalDevice, VkCommandPool commandPool, VkQueue graphicsQueue);
	void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, 
		VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);

	void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

	void createVertexBuffer();
	VkBuffer getVertexBuffer() { return vertexBuffer; }
	VkDeviceMemory getVertexBufferMemory() { return vertexBufferMemory; }

	uint32_t findMemoryRequirements(uint32_t typeFilter, VkMemoryPropertyFlags properties);
private:
	VkCommandPool commandPool;
	VkDevice device;
	VkPhysicalDevice physicalDevice;
	VkBuffer vertexBuffer;
	VkDeviceMemory vertexBufferMemory;

	VkQueue graphicsQueue;
	std::unique_ptr<Object> ObjectInfo = std::make_unique<Object>();
};