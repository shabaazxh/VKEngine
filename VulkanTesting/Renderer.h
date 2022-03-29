#pragma once

#include <vulkan/vulkan.h>
#include <stdexcept>
#include <vector>
#include "Buffer.h"
#include <memory>
#include <iostream>
#include <array>
#include "Input.h"
#include "VulkanTools.h"
#include "vendor/imgui/imgui.h"
#include "vendor/imgui/imgui_impl_vulkan.h"
#include "vendor/imgui/imgui_impl_glfw.h"
#include "commandBuffers.h"

namespace frame {
	struct frameData {
		VkRenderPass renderPass;
		std::vector<VkFramebuffer> framebuffer;
	};
}

//namespace ImGuiControlsInfo {
//	float radius = 0.02f;
//}

class Renderer {

	
public:

	struct SSAOTools {
		float radius = 1.0f;
		float tangent_bias = 0.3f;
		float scale;
		float sampleDirections = 8.0f;
		float num_sample_steps = 6.0f;
		float sampling_step = 0.04f;
		float shadowScalar = 1.0f;
		float shadowContrast = 0.5f;
		float depthThreshold = 0.000f;
		int sampleAmount = 16;
		int sampleTurns = 15;
		float ambientLightLevel = 1.0f;
		glm::vec4 lightPosition = glm::vec4(-2.0f, 4.0f, 0.0f, 1.0f);
		bool enableHBAO;
		bool enableCrytekSSAO;
		bool enableAlchemyAO;
	};

	SSAOTools SSAOController{};

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
		bool frameBufferResized,
		bool LockMouse,
		std::unique_ptr<CommandBuffers> commands);

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

	float radius;

	std::unique_ptr<CommandBuffers> cb;

};