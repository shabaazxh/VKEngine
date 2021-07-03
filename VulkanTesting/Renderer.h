#pragma once

#include <vulkan/vulkan.h>
#include <stdexcept>
#include <vector>
#include "Buffer.h"
#include <memory>
#include <iostream>


class Renderer {
public:
	Renderer(VkDevice device, VkSwapchainKHR swapChain, 
		std::vector<VkCommandBuffer> commandBuffers, 
		VkQueue graphicsQueue, VkQueue presentQueue, 
		std::vector<VkImage> swapChainImages, VkExtent2D swapChainExtent, VkPhysicalDevice physicalDevice, VkCommandPool commandPool, std::vector<VkDeviceMemory> uniformBufferMemory);
	void createSyncObjects();
	void drawFrame();

	std::vector<VkSemaphore> getAvailableSemaphore() { return imageAvaialbleSemaphore; }
	std::vector<VkSemaphore> getFinishedSemaphore() { return renderFinishedSemaphore; }

	std::vector<VkFence> getFences() { return inFlightFences; }

	int getMaxFrames() { return MAX_FRAMES_IN_FLIGHT; }

private:
	std::vector<VkSemaphore> imageAvaialbleSemaphore;
	std::vector<VkSemaphore> renderFinishedSemaphore;
	std::vector<VkFence> inFlightFences;
	std::vector<VkFence> imagesInFlight;

	std::vector<VkDeviceMemory> uniformBufferMemory;

	VkSwapchainKHR swapChain;
	std::vector<VkImage> swapChainImages;
	VkDevice device;
	std::vector<VkCommandBuffer> commandBuffers;
	VkQueue graphicsQueue;
	VkQueue presentQueue;
	VkExtent2D swapChainExtent;

	VkPhysicalDevice physicalDevice;
	VkCommandPool commandPool;

	const int MAX_FRAMES_IN_FLIGHT = 2;

	//To use right pair of semaphores, need to keep track of current frame
	size_t currentFrame = 0;

};