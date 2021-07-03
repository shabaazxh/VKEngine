#pragma once

#define GLM_FORCE_RADIANS
#include <vulkan/vulkan.h>
#include "Object.h"
#include <memory>
#include <stdexcept>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <GLFW/glfw3.h>

#include <chrono>


class Buffer {
public:
	Buffer(VkDevice device, VkPhysicalDevice physicalDevice, VkCommandPool commandPool, VkQueue graphicsQueue, std::vector<VkImage> swapChainImages, VkExtent2D swapChainExtent);
	Buffer(VkDevice device, VkPhysicalDevice physicalDevice, VkCommandPool commandPool, VkQueue graphicsQueue, std::vector<VkImage> swapChainImages, VkExtent2D swapChainExtent, std::vector<VkDeviceMemory> uniformBuffersMemory);

	Buffer() = default;
	void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, 
		VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);

	void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

	void createVertexBuffer();
	VkBuffer getVertexBuffer() { return vertexBuffer; }
	VkDeviceMemory getVertexBufferMemory() { return vertexBufferMemory; }

	//Uniform buffer
	void createUniformBuffer();
	void updateUniformBuffers(uint32_t currentImage);
	std::vector<VkBuffer> getUniformBuffers() { return uniformBuffers; }
	std::vector<VkDeviceMemory> getUniformBuffersMemory() { return uniformBuffersMemory; }

	uint32_t findMemoryRequirements(uint32_t typeFilter, VkMemoryPropertyFlags properties);

	VkExtent2D getSwapChainExtent() { return swapChainExtent; }

private:
	VkCommandPool commandPool;
	VkDevice device;
	VkPhysicalDevice physicalDevice;

	VkBuffer vertexBuffer;
	VkDeviceMemory vertexBufferMemory;

	std::vector<VkBuffer> uniformBuffers;
	std::vector<VkDeviceMemory> uniformBuffersMemory;
	VkExtent2D swapChainExtent;

	std::vector<VkImage> swapChainImages;

	VkQueue graphicsQueue;
	std::unique_ptr<Object> ObjectInfo = std::make_unique<Object>();
};