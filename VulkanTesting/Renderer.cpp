#include "Renderer.h"


Renderer::Renderer(VkDevice device, VkSwapchainKHR swapChain, std::vector<VkCommandBuffer> commandBuffers, VkQueue graphicsQueue, VkQueue presentQueue, std::vector<VkImage> swapChainImages, VkExtent2D swapChainExtent, VkPhysicalDevice physicalDevice, VkCommandPool commandPool, std::vector<VkDeviceMemory> uniformBufferMemory, std::array<glm::vec3,3> cameraMovement) {
	this->device = device;
	this->swapChain = swapChain;
	this->commandBuffers = commandBuffers;
	this->graphicsQueue = graphicsQueue;
	this->presentQueue = presentQueue;
	this->swapChainImages = swapChainImages;
	this->swapChainExtent = swapChainExtent;
	this->physicalDevice = physicalDevice;
	this->commandPool = commandPool;
	this->uniformBufferMemory = uniformBufferMemory;
	this->cameraMovement = cameraMovement;
}

void Renderer::drawFrame() {	

	std::unique_ptr<Buffer> bufferAssets = std::make_unique<Buffer>(device, physicalDevice, commandPool, graphicsQueue, swapChainImages, swapChainExtent, uniformBufferMemory, cameraMovement);

	uint32_t imageIndex;
	vkAcquireNextImageKHR(device, swapChain, UINT64_MAX, imageAvaialbleSemaphore[currentFrame], VK_NULL_HANDLE, &imageIndex);

	if (imagesInFlight[imageIndex] != VK_NULL_HANDLE) {
		vkWaitForFences(device, 1, &imagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
	}

	//Mark the image as being in use by the frame
	imagesInFlight[imageIndex] = inFlightFences[currentFrame];

	//HEREEE
	bufferAssets->updateUniformBuffers(imageIndex);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	
	VkSemaphore waitSemaphores[] = { imageAvaialbleSemaphore[currentFrame] };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffers[imageIndex]; //submit command buffer that binds swap chain image just accquired as color attachment

	VkSemaphore signalSemaphores[] = { renderFinishedSemaphore[currentFrame] };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	vkResetFences(device, 1, &inFlightFences[currentFrame]);

	if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS) {
		throw std::runtime_error("failed to submit draw command buffer!");
	}

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	VkSwapchainKHR swapChains[] = { swapChain };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &imageIndex;

	vkQueuePresentKHR(presentQueue, &presentInfo);

	vkQueueWaitIdle(presentQueue); //wait for work to finish before submitting 

	currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void Renderer::createSyncObjects() {

	imageAvaialbleSemaphore.resize(MAX_FRAMES_IN_FLIGHT);
	renderFinishedSemaphore.resize(MAX_FRAMES_IN_FLIGHT);
	inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
	imagesInFlight.resize(swapChainImages.size(), VK_NULL_HANDLE);

	VkSemaphoreCreateInfo semaphoreInfo{};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &imageAvaialbleSemaphore[i]) != VK_SUCCESS ||
			vkCreateSemaphore(device, &semaphoreInfo, nullptr, &renderFinishedSemaphore[i]) != VK_SUCCESS ||
			vkCreateFence(device, &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to create synchronization objects for a frame!");
		}
		
	}
}

void Renderer::processInput(GLFWwindow* window) {

	float cameraSpeed = 0.005f;

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		cameraMovement[0] += cameraSpeed * cameraMovement[1];
	}

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		cameraMovement[0] -= cameraSpeed * cameraMovement[1];
	}

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		cameraMovement[0] += glm::normalize(glm::cross(cameraMovement[1], cameraMovement[2])) * cameraSpeed;
	}

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		cameraMovement[0] -= glm::normalize(glm::cross(cameraMovement[1], cameraMovement[2])) * cameraSpeed;
	}

	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		cameraMovement[0].y += 0.003;
	}

	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
		cameraMovement[0].y -= 0.003;
	}






}