#pragma once

#include <vulkan/vulkan.h>
#include "Object.h"
#include <memory>
#include <stdexcept>

class Buffer {
public:
	Buffer(VkDevice device, VkPhysicalDevice physicalDevice);
	void createVertexBuffer();
	VkBuffer getVertexBuffer() { return vertexBuffer; }
	VkDeviceMemory getVertexBufferMemory() { return vertexBufferMemory; }

	uint32_t findMemoryRequirements(uint32_t typeFilter, VkMemoryPropertyFlags properties);
private:
	VkDevice device;
	VkPhysicalDevice physicalDevice;
	VkBuffer vertexBuffer;
	VkDeviceMemory vertexBufferMemory;
	std::unique_ptr<Object> ObjectInfo = std::make_unique<Object>();
};