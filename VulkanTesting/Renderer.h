#pragma once

#include <vulkan/vulkan.h>
#include <stdexcept>
#include <vector>
#include "Buffer.h"
#include <memory>
#include <iostream>
#include <array>
#include "Input.h"
#include "vendor/imgui/imgui.h"
#include "vendor/imgui/imgui_impl_vulkan.h"
#include "vendor/imgui/imgui_impl_glfw.h"

namespace frame {
	struct frameData {
		VkRenderPass renderPass;
		std::vector<VkFramebuffer> framebuffer;
	};
}

class Renderer {

	
public:



	Renderer(VkDevice device, VkSwapchainKHR swapChain, 
		std::vector<VkCommandBuffer> commandBuffers, 
		VkQueue graphicsQueue, 
		VkQueue presentQueue, 
		std::vector<VkImage> swapChainImages, 
		VkExtent2D swapChainExtent, 
		VkPhysicalDevice physicalDevice, 
		VkCommandPool commandPool, 
		std::vector<VkDeviceMemory> uniformBufferMemory, 
		std::vector<VkDeviceMemory> LightBufferMemory,
		std::vector<VkDeviceMemory> Light_2_BufferMemory,
		std::vector<VkDeviceMemory> SSAOKenrnelBufferMemory,
		frame::frameData frameInfo,
		bool frameBufferResized);

	void createSyncObjects();
	void drawFrame();

	std::vector<VkSemaphore> getAvailableSemaphore() { return imageAvaialbleSemaphore; }
	std::vector<VkSemaphore> getFinishedSemaphore() { return renderFinishedSemaphore; }

	std::vector<VkFence> getFences() { return inFlightFences; }

	int getMaxFrames() { return MAX_FRAMES_IN_FLIGHT; }

	void processInput(GLFWwindow* window);

	void updateUniformBuffers(uint32_t currentImage);

	void updateCommandBuffer(uint32_t currentImage);

	Input& GetCamera() {
		return Camera;
	}

private:

	float lerp(float a, float b, float f) {
		return a + f * (b - a);
	}

	std::vector<VkSemaphore> imageAvaialbleSemaphore;
	std::vector<VkSemaphore> renderFinishedSemaphore;
	std::vector<VkFence> inFlightFences;
	std::vector<VkFence> imagesInFlight;

	std::vector<VkDeviceMemory> uniformBufferMemory;
	std::vector<VkDeviceMemory> LightBufferMemory;
	std::vector<VkDeviceMemory> SSAOKenrnelBufferMemory;
	std::vector<VkDeviceMemory> Light_2_BufferMemory;

	VkSwapchainKHR swapChain;
	std::vector<VkImage> swapChainImages;
	VkDevice device;
	std::vector<VkCommandBuffer> commandBuffers;
	std::vector<VkCommandBuffer> ImGuiCommandBuffers;
	VkQueue graphicsQueue;
	VkQueue presentQueue;
	VkExtent2D swapChainExtent;

	GLFWwindow* window;

	VkPhysicalDevice physicalDevice;
	VkCommandPool commandPool;

	Input Camera;

	const int MAX_FRAMES_IN_FLIGHT = 2;
	bool frameBufferResized;

	//To use right pair of semaphores, need to keep track of current frame
	size_t currentFrame = 0;

	//glm::vec4 lightpos = glm::vec4(-2.0f, 4.0f, 5.0f, 1.0f);
	float far_plane_light = 20.0f;

	frame::frameData frameInfo;
};