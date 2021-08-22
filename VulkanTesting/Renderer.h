#pragma once

#include <vulkan/vulkan.h>
#include <stdexcept>
#include <vector>
#include "Buffer.h"
#include <memory>
#include <iostream>
#include <array>

class Renderer {
public:
	Renderer(VkDevice device, VkSwapchainKHR swapChain, 
		std::vector<VkCommandBuffer> commandBuffers, 
		VkQueue graphicsQueue, VkQueue presentQueue, 
		std::vector<VkImage> swapChainImages, VkExtent2D swapChainExtent, 
		VkPhysicalDevice physicalDevice, VkCommandPool commandPool, 
		std::vector<VkDeviceMemory> uniformBufferMemory, 
		std::vector<VkDeviceMemory> LightBufferMemory,
		std::vector<VkDeviceMemory> SSAOKenrnelBufferMemory,
		std::array<glm::vec3, 3> cameraMovement);
	void createSyncObjects();
	void drawFrame();

	std::vector<VkSemaphore> getAvailableSemaphore() { return imageAvaialbleSemaphore; }
	std::vector<VkSemaphore> getFinishedSemaphore() { return renderFinishedSemaphore; }

	std::vector<VkFence> getFences() { return inFlightFences; }

	int getMaxFrames() { return MAX_FRAMES_IN_FLIGHT; }

	void scroll_callback(GLFWwindow* window, double xOffset, double yOffset);
	void processInput(GLFWwindow* window);

private:
	std::vector<VkSemaphore> imageAvaialbleSemaphore;
	std::vector<VkSemaphore> renderFinishedSemaphore;
	std::vector<VkFence> inFlightFences;
	std::vector<VkFence> imagesInFlight;

	std::vector<VkDeviceMemory> uniformBufferMemory;
	std::vector<VkDeviceMemory> LightBufferMemory;
	std::vector<VkDeviceMemory> SSAOKenrnelBufferMemory;

	VkSwapchainKHR swapChain;
	std::vector<VkImage> swapChainImages;
	VkDevice device;
	std::vector<VkCommandBuffer> commandBuffers;
	VkQueue graphicsQueue;
	VkQueue presentQueue;
	VkExtent2D swapChainExtent;

	VkPhysicalDevice physicalDevice;
	VkCommandPool commandPool;

	std::array<glm::vec3, 3> cameraMovement;

	const int MAX_FRAMES_IN_FLIGHT = 2;

	//To use right pair of semaphores, need to keep track of current frame
	size_t currentFrame = 0;

	float cameraFov;

};