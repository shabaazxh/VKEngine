#pragma once

#define GLM_FORCE_RADIANS
#include <vulkan/vulkan.h>
#include "Object.h"
#include <memory>
#include <stdexcept>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <GLFW/glfw3.h>
#include <array>
#include <chrono>
#include <random>


class Buffer {

public:
	Buffer(VkDevice device, VkPhysicalDevice physicalDevice, VkCommandPool commandPool, VkQueue graphicsQueue, std::vector<VkImage> swapChainImages, VkExtent2D swapChainExtent, std::array<glm::vec3, 3> cameraMovement);
	Buffer(VkDevice device, VkPhysicalDevice physicalDevice, VkCommandPool commandPool, VkQueue graphicsQueue, std::vector<VkImage> swapChainImages, VkExtent2D swapChainExtent, std::vector<VkDeviceMemory> uniformBuffersMemory, std::vector<VkDeviceMemory> LightBuffersMemory, std::vector<VkDeviceMemory> SSAOKenrnelBufferMemory,std::array<glm::vec3, 3> cameraMovement);
	Buffer(VkDevice device, VkPhysicalDevice physicalDevice);
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

	//Light buffer
	std::vector<VkBuffer> getLightBuffers() { return LightBuffers; }
	std::vector<VkDeviceMemory> getLightBuffersMemory() { return LightBuffersMemory; }

	std::vector<VkBuffer> GetSSAOUniformBuffer() { return SSAOKernelBuffers; }
	std::vector<VkDeviceMemory> GetSSAOUniformBufferMemory() { return SSAOKenrnelBufferMemory; }

	//Helpers
	uint32_t findMemoryRequirements(uint32_t typeFilter, VkMemoryPropertyFlags properties);

	VkExtent2D getSwapChainExtent() { return swapChainExtent; }

	float getFOV() { return fov; }

	VkBuffer getDepthBuffer() { return depthShapeBuffer; }
	VkDeviceMemory getDepthBufferMemory() { return depthShapeBufferMemory; }

	VkBuffer getDepthIndexBuffer() { return depthIndexBuffer; }
	VkDeviceMemory getDepthIndexBufferMemory() { return depthIndexMemory; }


	glm::vec3 getLightPos() { return lightPos; }

	float lerp(float a, float b, float f) {
		return a + f * (b - a);
	}

private:
	VkCommandPool commandPool;
	VkDevice device;
	VkPhysicalDevice physicalDevice;

	VkBuffer vertexBuffer;
	VkDeviceMemory vertexBufferMemory;

	//DEPTH
	VkBuffer depthShapeBuffer;
	VkDeviceMemory depthShapeBufferMemory;
	VkBuffer depthIndexBuffer;
	VkDeviceMemory depthIndexMemory;


	VkBuffer indexBuffer;
	VkDeviceMemory indexBufferMemory;

	//Model buffer
	/*VkBuffer modelBuffer;
	VkDeviceMemory modelBufferMemory;*/

	//uniform buffer
	std::vector<VkBuffer> uniformBuffers;
	std::vector<VkDeviceMemory> uniformBuffersMemory;

	//Light bufffer
	std::vector<VkBuffer> LightBuffers;
	std::vector<VkDeviceMemory> LightBuffersMemory;

	std::vector<VkBuffer> SSAOKernelBuffers;
	std::vector<VkDeviceMemory> SSAOKenrnelBufferMemory;

	VkExtent2D swapChainExtent;

	std::vector<VkImage> swapChainImages;

	std::array<glm::vec3, 3> cameraMovement;

	VkQueue graphicsQueue;
	std::unique_ptr<Object> ObjectInfo = std::make_unique<Object>();

	glm::vec3 lightPos = glm::vec3(2.0f, -10.0f, 3.0f);
	float fov = 45.0f;
};