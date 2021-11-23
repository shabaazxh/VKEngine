#include "Renderer.h"


Renderer::Renderer(VkDevice device, VkSwapchainKHR swapChain, std::vector<VkCommandBuffer> commandBuffers, 
	VkQueue graphicsQueue, VkQueue presentQueue, std::vector<VkImage> swapChainImages, 
	VkExtent2D swapChainExtent, VkPhysicalDevice physicalDevice, VkCommandPool commandPool, 
	std::vector<VkDeviceMemory> uniformBufferMemory, 
	std::vector<VkDeviceMemory> LightBufferMemory,
	std::vector<VkDeviceMemory> SSAOKenrnelBufferMemory,
	std::array<glm::vec3,3> cameraMovement) {

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
	this->LightBufferMemory = LightBufferMemory;
	this->SSAOKenrnelBufferMemory = SSAOKenrnelBufferMemory;
	this->cameraMovement = cameraMovement;
}

void Renderer::drawFrame() {	

	uint32_t imageIndex;
	vkAcquireNextImageKHR(device, swapChain, UINT64_MAX, imageAvaialbleSemaphore[currentFrame], VK_NULL_HANDLE, &imageIndex);

	if (imagesInFlight[imageIndex] != VK_NULL_HANDLE) {
		vkWaitForFences(device, 1, &imagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
	}

	//Mark the image as being in use by the frame
	imagesInFlight[imageIndex] = inFlightFences[currentFrame];

	//HEREEE
	updateUniformBuffers(imageIndex);

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


	float cameraSpeed = 0.1f;

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
		cameraMovement[0].y += 0.1;
	}

	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
		cameraMovement[0].y -= 0.1;
		//std::cout << cameraMovement[0].y << std::endl;
	}

}

void Renderer::updateUniformBuffers(uint32_t currentImage)
{

	static auto startTime = std::chrono::high_resolution_clock::now();

	auto currentTime = std::chrono::high_resolution_clock::now();
	float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
	float spaceTime = glfwGetTime() / 5;

	float near_plane = 1.0f, far_plane = 30.f;

	float lightFOV = 45.0f;

	glm::vec4 lightpos = glm::vec4(-2.0f, 4.0f, 5.0f, 1.0f); //-2.0f, 9.0f, -8.0f, 1.0f :: //-2.0f, 5.0f, 5.0f, 1.0f
	// light pos before change -> -2.0f, 4.0f, 5.0f, 1.0f
	// SSAO samples 
	KernelSample ks{};
	std::uniform_real_distribution<float> randomFloats(0.0, 1.0);
	std::default_random_engine generator;
	std::vector<glm::vec4> ssaoKernel;

	for (unsigned int i = 0; i < 64; ++i) {

		glm::vec4 sample(
			randomFloats(generator) * 2.0 - 1.0, 
			randomFloats(generator) * 2.0 - 1.0, 
			randomFloats(generator), 1.0); // remove the * 2.0 - 1.0 from z axis to get rid of sphere occlusion
		sample = glm::normalize(sample);
		sample *= randomFloats(generator);

		float scale = float(i) / 64.0;
		scale = lerp(0.1f, 1.0f, scale * scale);
		sample *= scale;
		ssaoKernel.push_back(sample);
	}

	// SSAO samples -> Transfer to the shader
	for (unsigned int i = 0; i < 64; i++) {
		ks.samples[i] = ssaoKernel[i];
	}

	// Model, View & Projection
	UniformBufferObject ubo{};
	ubo.model = glm::mat4(1.0f);
	ubo.model = glm::translate(ubo.model, glm::vec3(0.0f, 0.0f, 0.0f));
	// camera movement : cameraMovement[0], cameraMovement[0] * cameraMovement[1], cameraMovement[2]
	ubo.view = glm::lookAt(cameraMovement[0], cameraMovement[0] * cameraMovement[1], cameraMovement[2]);
	ubo.proj = glm::perspective(glm::radians(40.0f), swapChainExtent.width / (float)swapChainExtent.height, 0.1f, 1000.0f);
	ubo.proj[1][1] *= -1;

	//glm::vec3(0.5f, .1f, 11.0f),
	//glm::vec3(0.0f, 0.0f, -1.0f)
	ks.mvMatrix = glm::mat4(ubo.model * ubo.view);
	//ks.projection = inverse(ubo.proj);
	ks.cameraEye = glm::vec4(cameraMovement[0], 1.0f);
	ks.cameraCenter = glm::vec4(cameraMovement[1], 1.0);
	ks.z_far = 1000;

	// Handling Lighting
	Light lighting{};
	lighting.position = glm::vec4(lightpos);
	/*lighting.position.x = 1.0f + sin(glfwGetTime()) * 2.0f;
	lighting.position.y = sin(glfwGetTime() / 2.0f) * 1.0f;
	lighting.position.z = 1.0f + sin(glfwGetTime() / 2.0f) * 5.0f;*/
	//std::cout << lighting.position.y << std::endl;
	lighting.lightColor = glm::vec4(0.5f, 0.5f, 0.5f, 1.0); //0.5f, 0.5f, 0.5f, 1.0
	lighting.objectColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	lighting.viewPos = glm::vec4(cameraMovement[0], 0.0f);
	lighting.invertedNormals = false;
	lighting.Linear = 0.09;
	lighting.Quadratic = 0.032;


	glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, 10.0f, -10.0f, near_plane, far_plane);
	//glm::mat4 lightProjection = glm::perspective(glm::radians(45.f), 1.0f, near_plane, far_plane);
	glm::mat4 lightView = glm::lookAt(glm::vec3(lighting.position), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 depthModelMatrix = glm::mat4(1.0f);
	lighting.lightSpaceMatrix = lightProjection * lightView * depthModelMatrix;

	// Mapping to GPU
	void* data;
	vkMapMemory(device, uniformBufferMemory[currentImage], 0, sizeof(ubo), 0, &data);
	memcpy(data, &ubo, sizeof(ubo));
	vkUnmapMemory(device, uniformBufferMemory[currentImage]);

	vkMapMemory(device, LightBufferMemory[currentImage], 0, sizeof(lighting), 0, &data);
	memcpy(data, &lighting, sizeof(lighting));
	vkUnmapMemory(device, LightBufferMemory[currentImage]);

	vkMapMemory(device, SSAOKenrnelBufferMemory[currentImage], 0, sizeof(ks), 0, &data);
	memcpy(data, &ks, sizeof(ks));
	vkUnmapMemory(device, SSAOKenrnelBufferMemory[currentImage]);

}
