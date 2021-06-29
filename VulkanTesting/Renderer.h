#pragma once

#include <vulkan/vulkan.h>
#include <stdexcept>
#include <vector>
class Renderer {
public:
	Renderer(VkDevice device, VkSwapchainKHR swapChain, std::vector<VkCommandBuffer> commandBuffers, VkQueue graphicsQueue, VkQueue presentQueue, std::vector<VkImage> swapChainImages);
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

	VkSwapchainKHR swapChain;
	std::vector<VkImage> swapChainImages;
	VkDevice device;
	std::vector<VkCommandBuffer> commandBuffers;
	VkQueue graphicsQueue;
	VkQueue presentQueue;

	const int MAX_FRAMES_IN_FLIGHT = 2;

	//To use right pair of semaphores, need to keep track of current frame
	size_t currentFrame = 0;
};