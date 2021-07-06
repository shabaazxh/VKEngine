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
#include <array>
#include <chrono>


class Buffer {
public:
	Buffer(VkDevice device, VkPhysicalDevice physicalDevice, VkCommandPool commandPool, VkQueue graphicsQueue, std::vector<VkImage> swapChainImages, VkExtent2D swapChainExtent, std::array<glm::vec3, 3> cameraMovement);
	Buffer(VkDevice device, VkPhysicalDevice physicalDevice, VkCommandPool commandPool, VkQueue graphicsQueue, std::vector<VkImage> swapChainImages, VkExtent2D swapChainExtent, std::vector<VkDeviceMemory> uniformBuffersMemory, std::array<glm::vec3, 3> cameraMovement);

	Buffer() = default;
	void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, 
		VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);

	void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

	//Vertex buffer
	void createVertexBuffer(unsigned int sizeBuffer, std::vector<Vertex> info);
	VkBuffer getVertexBuffer() { return vertexBuffer; }
	VkDeviceMemory getVertexBufferMemory() { return vertexBufferMemory; }

	//Index buffer
	void createIndexBuffer(unsigned int sizeBuffer, std::vector<uint32_t> info);
	VkBuffer getIndexBuffer() { return indexBuffer; }
	VkDeviceMemory getIndexBufferMemory() { return indexBufferMemory; }

	//Uniform buffer
	void createUniformBuffer();
	void updateUniformBuffers(uint32_t currentImage);
	std::vector<VkBuffer> getUniformBuffers() { return uniformBuffers; }
	std::vector<VkDeviceMemory> getUniformBuffersMemory() { return uniformBuffersMemory; }

	uint32_t findMemoryRequirements(uint32_t typeFilter, VkMemoryPropertyFlags properties);

	VkExtent2D getSwapChainExtent() { return swapChainExtent; }

	glm::vec3 getCP() { return cameraPos; }

private:
	VkCommandPool commandPool;
	VkDevice device;
	VkPhysicalDevice physicalDevice;

	VkBuffer vertexBuffer;
	VkDeviceMemory vertexBufferMemory;

	VkBuffer indexBuffer;
	VkDeviceMemory indexBufferMemory;

	//Model buffer
	/*VkBuffer modelBuffer;
	VkDeviceMemory modelBufferMemory;*/

	std::vector<VkBuffer> uniformBuffers;
	std::vector<VkDeviceMemory> uniformBuffersMemory;
	VkExtent2D swapChainExtent;

	std::vector<VkImage> swapChainImages;

	std::array<glm::vec3, 3> cameraMovement;

	glm::vec3 cameraPos;
	glm::vec3 cameraFront;
	glm::vec3 cameraUp;

	VkQueue graphicsQueue;
	std::unique_ptr<Object> ObjectInfo = std::make_unique<Object>();
};