#pragma once

#include <vulkan/vulkan.h>
#include <stdexcept>
#include <vector>
class Renderer {
public:
	Renderer(VkDevice device, VkSwapchainKHR swapChain, std::vector<VkCommandBuffer> commandBuffers, VkQueue graphicsQueue, VkQueue presentQueue);
	void createSemaphore();
	void drawFrame();

	VkSemaphore getAvailableSemaphore() { return imageAvaialbleSemaphore; }
	VkSemaphore getFinishedSemaphore() { return renderFinishedSemaphore; }

private:
	VkSemaphore imageAvaialbleSemaphore;
	VkSemaphore renderFinishedSemaphore;
	VkSwapchainKHR swapChain;
	VkDevice device;
	std::vector<VkCommandBuffer> commandBuffers;
	VkQueue graphicsQueue;
	VkQueue presentQueue;
};