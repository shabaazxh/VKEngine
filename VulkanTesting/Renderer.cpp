#include "Renderer.h"


Renderer::Renderer(VkDevice device, VkSwapchainKHR swapChain, 
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
	bool frameBufferResized)
{

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
	this->Light_2_BufferMemory = Light_2_BufferMemory;
	this->window = window;
	this->frameInfo = frameInfo;
	this->frameBufferResized = frameBufferResized;

	Camera = Input(window, glm::vec3(0.0f, 0.0f, 10.0f),
		glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
}

void Renderer::drawFrame() {

	uint32_t imageIndex;
	VkResult result = vkAcquireNextImageKHR(device, swapChain, UINT64_MAX, imageAvaialbleSemaphore[currentFrame], VK_NULL_HANDLE, &imageIndex);

	if (result == VK_ERROR_OUT_OF_DATE_KHR) {
		return;
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
		throw std::runtime_error("failed to acquire swap chain image!");
	}

	if (imagesInFlight[imageIndex] != VK_NULL_HANDLE) {
		vkWaitForFences(device, 1, &imagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
	}

	//Mark the image as being in use by the frame
	imagesInFlight[imageIndex] = inFlightFences[currentFrame];

	updateCommandBuffer(imageIndex);

	//HEREEE
	updateUniformBuffers(imageIndex);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	
	VkSemaphore waitSemaphores[] = { imageAvaialbleSemaphore[currentFrame] };

	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	
	VkCommandBuffer cmds[] = { commandBuffers[imageIndex], ImGuiCommandBuffers[imageIndex] };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 2;
	submitInfo.pCommandBuffers = cmds; //submit command buffer that binds swap chain image just accquired as color attachment
	
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

	result = vkQueuePresentKHR(presentQueue, &presentInfo);

	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || frameBufferResized) {
		frameBufferResized = false;
		// recreateSwapChain();
	}
	else if (result != VK_SUCCESS) {
		throw std::runtime_error("failed to present swap chain image!");
	}

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
		Camera.setCameraPos(Camera.getCameraPos() += cameraSpeed * Camera.getCameraFront());
		//lightpos.y -= cameraSpeed;
		
	}

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		//cameraMovement[0] -= cameraSpeed * cameraMovement[1];
		//lightpos.y += cameraSpeed;
		Camera.setCameraPos(Camera.getCameraPos() -= cameraSpeed * Camera.getCameraFront());

	}

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		//cameraMovement[0] += glm::normalize(glm::cross(cameraMovement[1], cameraMovement[2])) * cameraSpeed;
		//lightpos.x += cameraSpeed;
		Camera.setCameraPos(Camera.getCameraPos() += glm::normalize(glm::cross(Camera.getCameraFront(), Camera.getcameraUp())) * cameraSpeed);
	}

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		//cameraMovement[0] -= glm::normalize(glm::cross(cameraMovement[1], cameraMovement[2])) * cameraSpeed;
		//lightpos.x -= cameraSpeed;
		Camera.setCameraPos(Camera.getCameraPos() -= glm::normalize(glm::cross(Camera.getCameraFront(), Camera.getcameraUp())) * cameraSpeed);
	}

	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		//cameraMovement[0].y += 0.1;
		//lightpos.z -= cameraSpeed;
		glm::vec3 currentPosition = Camera.getCameraPos();
		currentPosition.y += cameraSpeed;
		
		Camera.setCameraPos(currentPosition);
	}

	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
		//cameraMovement[0].y -= 0.1;
		//lightpos.z += cameraSpeed;
		//std::cout << cameraMovement[0].y << std::endl;
		glm::vec3 currentPosition = Camera.getCameraPos();
		currentPosition.y -= cameraSpeed;

		Camera.setCameraPos(currentPosition);
	}

	if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS) {
		//lightpos = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
	}

}

void Renderer::updateUniformBuffers(uint32_t currentImage)
{

	static auto startTime = std::chrono::high_resolution_clock::now();

	auto currentTime = std::chrono::high_resolution_clock::now();
	float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
	float spaceTime = glfwGetTime() / 5;

	float near_plane = 1.0f, far_plane = 20.0f;

	float lightFOV = 45.0f;

	//glm::vec4 lightpos = glm::vec4(-2.0f, -4.0f, -2.0f, 1.0f); //-2.0f, 9.0f, -8.0f, 1.0f :: //-2.0f, 5.0f, 5.0f, 1.0f
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
			randomFloats(generator) * 2.0 - 1.0, 0.0); // remove the * 2.0 - 1.0 from z axis to get rid of sphere occlusion
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
	glm::vec4 lightpos = glm::vec4(-2.0f, 4.0f, 2.0f, 1.0f);

	// Model, View & Projection
	UniformBufferObject ubo{};
	ubo.model = glm::mat4(1.0f);
	ubo.model = glm::translate(ubo.model, glm::vec3(0.0f, 0.0f, 0.0f));
	// camera movement : cameraMovement[0], cameraMovement[0] * cameraMovement[1], cameraMovement[2]
	ubo.view = glm::lookAt(Camera.getCameraPos(), Camera.getCameraPos() + Camera.getCameraFront(), Camera.getcameraUp());
	ubo.proj = glm::perspective(glm::radians(40.0f), swapChainExtent.width / (float)swapChainExtent.height, 0.1f, 1000.0f);
	ubo.proj[1][1] *= -1;
	ubo.albedo = glm::vec3(1.0f, 1.0f, 1.0f);
	ubo.ao = 1.0f;
	ubo.roughness = 0.05f;
	ubo.metallic = 0.5f;

	//glm::vec3(0.5f, .1f, 11.0f),
	//glm::vec3(0.0f, 0.0f, -1.0f)
	ks.mvMatrix = glm::mat4(ubo.model * ubo.view);
	ks.cameraEye = glm::vec4(Camera.getCameraPos(), 1.0f);
	ks.cameraCenter = glm::vec4(Camera.getCameraFront(), 1.0);
	ks.z_far = 1000;

	// Handling Lighting
	Light lighting{};
	lighting.position = glm::vec4(lightpos);
	lighting.position.x = 1.0f + sin(glfwGetTime()) * 2.0f;
	lighting.position.y = sin(glfwGetTime() / 2.0f) * 1.0f;
	//lighting.position.x = 1.0f + sin(glfwGetTime()) * 2.0f;
	//lighting.position.y = 1.0f + sin(glfwGetTime() / 2.0f) * 1.0f;
	//lighting.position.z = 1.0f + sin(glfwGetTime() / 2.0f) * 1.0f;
	//std::cout << lighting.position.x << " " << lighting.position.y << " " << lighting.position.z << std::endl;
	lighting.lightColor = glm::vec4(1.0, 1.0f, 1.0, 1.0); //0.5f, 0.5f, 0.5f, 1.0
	lighting.objectColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	lighting.viewPos = glm::vec4(Camera.getCameraPos(), 0.0f);
	lighting.invertedNormals = false;
	lighting.Linear = 0.09;
	lighting.Quadratic = 0.032;

	Light Light_2{};
	Light_2.position = glm::vec4(lightpos);
	Light_2.position.x = 1.0f + sin(glfwGetTime()) * 2.0f;
	Light_2.position.y = sin(glfwGetTime() / 2.0f) * 1.0f;
	Light_2.position.z = 1.0f + sin(glfwGetTime()) * 2.0f;
	Light_2.lightColor = glm::vec4(0.0, 1.0f, 0.0, 1.0); //0.5f, 0.5f, 0.5f, 1.0
	Light_2.objectColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	Light_2.viewPos = glm::vec4(Camera.getCameraPos(), 0.0f);

	ubo.lightPositions[0] = lighting.position;
	ubo.lightPositions[1] = Light_2.position;
	ubo.lightColors[0] = lighting.lightColor;
	ubo.lightColors[1] = Light_2.lightColor;

	// Shadows 
	glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, 10.0f, -10.0f, near_plane, far_plane);
	//glm::mat4 lightProjection = glm::perspective(glm::radians(45.0f), 1.0f, near_plane, far_plane);
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

	vkMapMemory(device, Light_2_BufferMemory[currentImage], 0, sizeof(Light_2), 0, &data);
	memcpy(data, &lighting, sizeof(lighting));
	vkUnmapMemory(device, Light_2_BufferMemory[currentImage]);

	vkMapMemory(device, SSAOKenrnelBufferMemory[currentImage], 0, sizeof(ks), 0, &data);
	memcpy(data, &ks, sizeof(ks));
	vkUnmapMemory(device, SSAOKenrnelBufferMemory[currentImage]);

}

void Renderer::updateCommandBuffer(uint32_t currentImage)
{
	ImGuiCommandBuffers.resize(commandBuffers.size());

	ImGui_ImplVulkan_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	static bool windowOpened = true;
	static bool showDemoWindow = false;
	if (ImGui::Begin("Rendertime", &windowOpened, 0))
	{
		ImGui::Text("Frametime: %f", 1000.0f / ImGui::GetIO().Framerate);
		ImGui::Checkbox("Show ImGui demo window", &showDemoWindow);
		ImGui::End();
	}

	ImGui::Render();
	ImDrawData* main_draw_data = ImGui::GetDrawData();

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = (uint32_t)ImGuiCommandBuffers.size();
	

	if (vkAllocateCommandBuffers(device, &allocInfo, ImGuiCommandBuffers.data()) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate command buffers");
	}

	//Starting recording command buffer
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	
	//Start recording
	if (vkBeginCommandBuffer(ImGuiCommandBuffers[currentImage], &beginInfo) != VK_SUCCESS) {
		throw std::runtime_error("Failed to begin recording command buffer!");
	}

	VkRenderPassBeginInfo imguiRenderPass{};
	imguiRenderPass.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	imguiRenderPass.renderPass = frameInfo.renderPass;
	imguiRenderPass.framebuffer = frameInfo.framebuffer[currentImage];
	imguiRenderPass.renderArea.offset = { 0,0 };
	imguiRenderPass.renderArea.extent = swapChainExtent;

	std::array<VkClearValue, 2> imguiclearValues{};
	imguiclearValues[0].color = { 0.3f, 0.5f, .7f, 1.0f };
	imguiclearValues[1].depthStencil = { 1.0f, 0 };

	imguiRenderPass.clearValueCount = static_cast<uint32_t>(imguiclearValues.size());
	imguiRenderPass.pClearValues = imguiclearValues.data();

	vkCmdBeginRenderPass(ImGuiCommandBuffers[currentImage], &imguiRenderPass, VK_SUBPASS_CONTENTS_INLINE);

	ImGui_ImplVulkan_RenderDrawData(main_draw_data, ImGuiCommandBuffers[currentImage]);

	vkCmdEndRenderPass(ImGuiCommandBuffers[currentImage]);

	if (vkEndCommandBuffer(ImGuiCommandBuffers[currentImage]) != VK_SUCCESS) {
		throw std::runtime_error("failed to record command buffer");
	}
}
