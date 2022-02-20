#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>


#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <vulkan/vulkan.h>

#include <iostream>
#include <vector>
#include <optional>
#include <set>
#include <cstdint>
#include <algorithm>
#include <memory>
#include <array>

//Class
#include "Window.h"
#include "pipeline.h"
#include "RenderPass.h"
#include "Framebuffers.h"
#include "commandPool.h"
#include "commandBuffers.h"
#include "Renderer.h"
#include "Object.h"
#include "Buffer.h"
#include "DescriptorSetLayout.h"
#include "Descriptors.h"
#include "Input.h"
#include "Image.h"
#include "VulkanDevice.h"
#include "SwapChain.h"

const std::vector<const char*> validationLayers = {
	 "VK_LAYER_KHRONOS_validation"
};

#ifdef NDEBUG
	const bool enableValidationLayers = false;
#else
	const bool enableValidationLayers = true;
#endif // DEBUG


VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr) {
		return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
	}
	else {
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

//next thing to do -- VERTEX BUFFERS -- Staging buffer == 
const std::vector<const char*> deviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

struct QueueFamilyIndices {
	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentFamily;

	bool isComplete() {
		return graphicsFamily.has_value() && presentFamily.has_value();
	}
};


class vulkanEngine2 {
public:
	~vulkanEngine2() {
		delete EngineWindow;
		EngineWindow = nullptr;
	}
	void run() {
		initEngine();
		initVulkan();
		mainloop();
		cleanup();
	}
private:

	VkInstance instance; 

	VkSurfaceKHR surface;

	std::vector<VkImageView> swapChainImagesViews; // <- an ImageView is just a view into the image describes how to access the image and which part of the image to access

	VkDebugUtilsMessengerEXT debugMessenger;

	//Instance class
	std::unique_ptr<Pipeline> SSAOQuadPipeline;
	std::unique_ptr<Pipeline> SSAOBlurPipeline;
	std::unique_ptr<Pipeline> QuadPipeline;
	std::unique_ptr<Pipeline> pipeline;
	std::unique_ptr<Pipeline> shadowPipeline;
	std::unique_ptr<Pipeline> FloorPipeline;
	std::unique_ptr<Pipeline> GeometryPassPipeline;

	std::unique_ptr<RenderPass> renderPass;
	std::unique_ptr<Framebuffers> frameBuffers;
	std::unique_ptr<CommandPool> commandPool;
	std::unique_ptr<CommandBuffers> commandBuffers;
	std::unique_ptr<Renderer> renderer;

	std::unique_ptr<Buffer> buffers;
	std::unique_ptr<Buffer> FloorObjectBuffer;
	std::unique_ptr<Buffer> SSAOQuadBuffer;
	std::unique_ptr<Buffer> QuadBuffer;
	std::unique_ptr<Buffer> LightBuffer;

	std::unique_ptr<Object> SecondModel;
	std::unique_ptr<DescriptorSetLayout> descriptorSetLayout;
	std::unique_ptr<Descriptors> descriptors;
	std::unique_ptr<Input> GameInput = std::make_unique<Input>();
	std::unique_ptr<ImageResource> ImageRes;
	std::unique_ptr<ImageResource> ImageResHelper;

	VE::Window* EngineWindow = nullptr;

	std::unique_ptr<VE::VulkanDevice> vkDevice;
	std::unique_ptr<VE::SwapChain> swapChain;

	void initEngine() {

		//std::system("C:/Users/Shahb/source/repos/VulkanTesting/VulkanTesting/shaders/compile.bat");
		EngineWindow = new VE::Window(2560, 1440, "title");
		EngineWindow->initWindow();

		//GameInput->setCameraSettings(EngineWindow->getWindow(), glm::vec3(0.5f, .1f, 11.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	}

	void initVulkan() {

		//createInstance();
		//setupDebugMessenger();
		//createSurface();

		vkDevice = std::make_unique<VE::VulkanDevice>(instance, surface);
		vkDevice->createDevice();
		
		swapChain = std::make_unique<VE::SwapChain>(instance,vkDevice->getDevice(),vkDevice->getPhysicalDevice(), surface, EngineWindow->getWindow());
		swapChain->createSwapChain();

		ImageResource DepthFormat(vkDevice->getPhysicalDevice());
		VkFormat shadowDepth = DepthFormat.findDepthFormat();

		renderPass = std::make_unique<RenderPass>(vkDevice->getDevice(), swapChain->getSwapChainImageFormat());
		renderPass->createSceneRenderPass(shadowDepth);
		renderPass->createShadowRenderPass(shadowDepth);
		renderPass->createQuadRenderPass();
		renderPass->createQuadRenderPass();
		renderPass->createGeometryPassRenderPass(shadowDepth);
		renderPass->createSSAORenderPass();
		renderPass->createSSAOBlurRenderPass();

		descriptorSetLayout = std::make_unique<DescriptorSetLayout>(vkDevice->getDevice());
		descriptorSetLayout->createDescriptorSetLayout();
		descriptorSetLayout->createQuadDescriptorSetLayout();
		descriptorSetLayout->createSSAODescriptorSetLayout();
		descriptorSetLayout->createSSAOBlurDescriptorSetLayout();

		GeometryPassPipeline = std::make_unique<Pipeline>(vkDevice->getDevice(), renderPass->GetGeometryPassRenderPass(), swapChain->getSwapChainExtent(), descriptorSetLayout->getDescriptorSetLayout());
		GeometryPassPipeline->createGeometryPassGraphicsPipeline("shaders/GeometryPass/ssao_geometry.vert.spv", "shaders/GeometryPass/ssao_geometry.frag.spv");

		SSAOQuadPipeline = std::make_unique<Pipeline>(vkDevice->getDevice(), renderPass->GetSSAORenderPass(), swapChain->getSwapChainExtent(), descriptorSetLayout->GetSSAODescriptorSetLayout());
		SSAOQuadPipeline->createGraphicsPipelineOverlay("shaders/ssao/SSAOQuad.vert.spv", "shaders/ssao/SSAOQuad.frag.spv");

		SSAOBlurPipeline = std::make_unique<Pipeline>(vkDevice->getDevice(), renderPass->GetSSAOBlurRenderPass(), swapChain->getSwapChainExtent(), descriptorSetLayout->GetSSAOBlurDescriptorSetLayout());
		SSAOBlurPipeline->createGraphicsPipelineOverlay("shaders/ssao/SSAOBlur.vert.spv","shaders/ssao/SSAOBlur.frag.spv");

		QuadPipeline = std::make_unique<Pipeline>(vkDevice->getDevice(), renderPass->GetQuadRenderPass(), swapChain->getSwapChainExtent(), descriptorSetLayout->GetQuadDescriptorSetLayout());
		QuadPipeline->createGraphicsPipelineOverlay("shaders/quad.vert.spv","shaders/quad.frag.spv");

		shadowPipeline = std::make_unique<Pipeline>(vkDevice->getDevice(), renderPass->GetShadowRenderPass(), swapChain->getSwapChainExtent(), descriptorSetLayout->getDescriptorSetLayout());
		shadowPipeline->createGraphicsPipelineSingleShader("shaders/shadow/shadow.vert.spv");

		pipeline = std::make_unique<Pipeline>(vkDevice->getDevice(), renderPass->GetSceneRenderPass(), swapChain->getSwapChainExtent(), descriptorSetLayout->getDescriptorSetLayout());
		pipeline->createGraphicsPipeline("shaders/Object/vert.spv", "shaders/Object/frag.spv");

		FloorPipeline = std::make_unique<Pipeline>(vkDevice->getDevice(), renderPass->GetSceneRenderPass(), swapChain->getSwapChainExtent(), descriptorSetLayout->getDescriptorSetLayout());
		FloorPipeline->createGraphicsPipeline("shaders/Object/textureShader.vert.spv", "shaders/Object/textureShader.frag.spv");

		VE::QueueFamilyIndices indices = vkDevice->findQueueFamilies(vkDevice->getPhysicalDevice());
		commandPool = std::make_unique<CommandPool>(vkDevice->getDevice(), vkDevice->getPhysicalDevice(), surface, indices.graphics);
		commandPool->createCommandPool(indices.graphics.value());

		ImageRes = std::make_unique<ImageResource>(vkDevice->getDevice(), commandPool->getCommandPool(), vkDevice->getGraphicsQueue(), vkDevice->getPhysicalDevice(), swapChain->getSwapChainImageFormat());
		ImageRes->createImageResources(swapChain->getSwapChainExtent());

		/** ------------------------ FRAME BUFFERS ------------------------- */
		frameBuffers = std::make_unique<Framebuffers>(vkDevice->getDevice(), swapChain->getSwapChainImageViews(), renderPass->GetSceneRenderPass(), swapChain->getSwapChainExtent());

		frameBuffers->createSwapChainFramebuffers(renderPass->GetQuadRenderPass(), ImageRes->getDepthImageView());

		frameBuffers->createShadowFramebuffer(renderPass->GetShadowRenderPass(), ImageRes->getshadowImageView());

		frameBuffers->createDisplaySceneFramebuffer(renderPass->GetSceneRenderPass(), ImageRes->GetSceneImageView(), ImageRes->getDepthImageView());

		frameBuffers->createGeometryPassFrameBuffer(renderPass->GetGeometryPassRenderPass(), ImageRes->GetGeometryImageView(), ImageRes->GetPositionImageView(), ImageRes->GetgNormalsImageView(), ImageRes->GetAlbedoImageView(), ImageRes->getDepthImageView());

		frameBuffers->createSSAOQuadFrameBuffer(renderPass->GetSSAORenderPass(), ImageRes->GetSSAOImageView());

		frameBuffers->createSSAOBlurQuadFrameBuffer(renderPass->GetSSAOBlurRenderPass(), ImageRes->GetSSAOBlurImageView());


		ImageTools::imageInfo F1Image{};
		F1Image.DiffuseLocation = "Textures/white.png";
		F1Image.specularLocation = "Textures/white.png";
		F1Image.AOLocation = "Textures/white.png";
		F1Image.EmissionLocation = "Textures/white.png";
		ImageRes->createTextureImage(F1Image);

		ImageTools::imageInfo FloorImageInfo{};
		FloorImageInfo.DiffuseLocation = "Textures/white.png";
		FloorImageInfo.specularLocation = "Textures/white.png";
		FloorImageInfo.AOLocation = "Textures/white.png";
		FloorImageInfo.EmissionLocation= "Textures/white.png";

		ImageRes->createTextureImage(FloorImageInfo);

		ImageRes->createTextureSampler();
		ImageRes->createSceneSampler();
		ImageRes->createRepeatSampler();

		std::array<glm::vec3, 3> CameraMovement = { GameInput->getCameraPos(), GameInput->getCameraFront(), GameInput->getcameraUp() };
		
		Object objectData;
		SecondModel = std::make_unique <Object>();
		buffers = std::make_unique<Buffer>(vkDevice->getDevice(), vkDevice->getPhysicalDevice(), commandPool->getCommandPool(), vkDevice->getGraphicsQueue(), swapChain->getSwapChainImages(), swapChain->getSwapChainExtent());
	
		objectData.loadModel("Models/f1carwithcubes.obj"); //f1_onthefloor f1carwithcubes
		SecondModel->loadModel("Models/floor.obj");

		LightBuffer = std::make_unique<Buffer>(vkDevice->getDevice(), vkDevice->getPhysicalDevice(), commandPool->getCommandPool(), vkDevice->getGraphicsQueue(), swapChain->getSwapChainImages(), swapChain->getSwapChainExtent());
		LightBuffer->createUniformBuffer(sizeof(Light));

		buffers->createVertexBuffer(sizeof(objectData.getVertexData()[0]) * objectData.getVertexData().size(), objectData.getVertexData());
		buffers->createIndexBuffer(sizeof(objectData.getIndexData()[0]) * objectData.getIndexData().size(), objectData.getIndexData());
		buffers->createUniformBuffer(sizeof(UniformBufferObject));

		FloorObjectBuffer = std::make_unique<Buffer>(vkDevice->getDevice(), vkDevice->getPhysicalDevice(), commandPool->getCommandPool(), vkDevice->getGraphicsQueue(), swapChain->getSwapChainImages(), swapChain->getSwapChainExtent());
		FloorObjectBuffer->createVertexBuffer(sizeof(SecondModel->getVertexData()[0]) * SecondModel->getVertexData().size(), SecondModel->getVertexData());
		FloorObjectBuffer->createUniformBuffer(sizeof(UniformBufferObject));

		QuadBuffer = std::make_unique<Buffer>(vkDevice->getDevice(), vkDevice->getPhysicalDevice(), commandPool->getCommandPool(), vkDevice->getGraphicsQueue(), swapChain->getSwapChainImages(), swapChain->getSwapChainExtent());
		QuadBuffer->createVertexBuffer(sizeof(objectData.GetQuadVertex()[0]) * objectData.GetQuadVertex().size(), objectData.GetQuadVertex());
		QuadBuffer->createIndexBuffer(sizeof(objectData.GetQuadIncies()[0]) * objectData.GetQuadIncies().size(), objectData.GetQuadIncies());

		SSAOQuadBuffer = std::make_unique<Buffer>(vkDevice->getDevice(), vkDevice->getPhysicalDevice(), commandPool->getCommandPool(), vkDevice->getGraphicsQueue(), swapChain->getSwapChainImages(), swapChain->getSwapChainExtent());
		SSAOQuadBuffer->createVertexBuffer(sizeof(objectData.GetQuadVertex()[0]) * objectData.GetQuadVertex().size(), objectData.GetQuadVertex());
		SSAOQuadBuffer->createIndexBuffer(sizeof(objectData.GetQuadIncies()[0]) * objectData.GetQuadIncies().size(), objectData.GetQuadIncies());
		SSAOQuadBuffer->createUniformBuffer(sizeof(KernelSample));

		descriptors = std::make_unique<Descriptors>(
			vkDevice->getDevice(),
			swapChain->getSwapChainImages(),
			buffers->getUniformBuffers(),
			LightBuffer->getUniformBuffers(),
			SSAOQuadBuffer->getUniformBuffers(),
			descriptorSetLayout->getDescriptorSetLayout(),
			descriptorSetLayout->GetQuadDescriptorSetLayout(),
			descriptorSetLayout->GetSSAODescriptorSetLayout(),
			descriptorSetLayout->GetSSAOBlurDescriptorSetLayout(),
			ImageRes->GetSSAOImageView(),
			ImageRes->GetAlbedoImageView(),
			ImageRes->GetSSAOSamplingImageView(),
			ImageRes->getTextureImageView(),
			ImageRes->getNormImageView(),
			ImageRes->getshadowImageView(),
			ImageRes->GetPositionImageView(),
			ImageRes->GetgNormalsImageView(),
			ImageRes->GetNoiseImageView(),
			ImageRes->GetSSAOBlurImageView(),
			ImageRes->GetGeometryImageView(),
			ImageRes->getSampler(),
			ImageRes->GetSceneSampler(),
			ImageRes->GetSceneImageView(),
			ImageRes->GetRepeatSampler(),
			F1Image.textureImageView,
			F1Image.specularImageView,
			FloorObjectBuffer->getUniformBuffers(),
			FloorImageInfo.textureImageView,
			FloorImageInfo.specularImageView,
			F1Image.AOImageView,
			F1Image.EmissionImageView);

		descriptors->createDescriptorPool();
		descriptors->createDescriptorSets();

		commandBuffers = std::make_unique<CommandBuffers>(
			vkDevice->getDevice(),
			frameBuffers->getSwapChainFramebuffers(),
			commandPool->getCommandPool(),
			renderPass->GetSceneRenderPass(),
			swapChain->getSwapChainExtent(),
			pipeline->getGraphicsPipeline(),
			pipeline->getComputePipeline(),
			vkDevice->getPhysicalDevice(),
			buffers->getVertexBuffer(),
			buffers->getIndexBuffer(),
			pipeline->getPipelineLayout(),
			descriptors->getDescriptorSets(),
			objectData.getVertexData(),
			shadowPipeline->getGraphicsPipeline(),
			frameBuffers->getShadowFramebuffer(),
			renderPass->GetShadowRenderPass(),
			shadowPipeline->getPipelineLayout(),
			shadowPipeline->getGraphicsPipeline(),
			SecondModel->getVertexData(),
			FloorObjectBuffer->getVertexBuffer(),
			objectData.GetQuadIncies(),
			QuadBuffer->getVertexBuffer(),
			QuadBuffer->getIndexBuffer(),
			QuadPipeline->getGraphicsPipeline(),
			QuadPipeline->getPipelineLayout(),
			renderPass->GetQuadRenderPass(),
			frameBuffers->GetDisplaySceneFramebuffer(),
			descriptors->GetSceneDescriptorSets(),
			GeometryPassPipeline->getGraphicsPipeline(),
			GeometryPassPipeline->getPipelineLayout(),
			renderPass->GetGeometryPassRenderPass(),
			frameBuffers->GetGeometryPassFrameBuffer(),
			SSAOQuadPipeline->getGraphicsPipeline(),
			SSAOQuadPipeline->getPipelineLayout(),
			SSAOQuadBuffer->getVertexBuffer(),
			SSAOQuadBuffer->getIndexBuffer(),
			renderPass->GetSSAORenderPass(),
			frameBuffers->GetSSAOFramebuffer(),
			descriptors->GetSSAODescriptorSets(),
			SSAOBlurPipeline->getGraphicsPipeline(),
			SSAOBlurPipeline->getPipelineLayout(),
			descriptors->GetSSAOBlurDescriptorSets(),
			frameBuffers->GetSSAOBlurFramebuffer(),
			renderPass->GetSSAOBlurRenderPass(),
			FloorPipeline->getGraphicsPipeline(),
			FloorPipeline->getPipelineLayout(),
			descriptors->GetFloorDescriptorSet());

		commandBuffers->createCommandBuffers();

		renderer = std::make_unique<Renderer>(
			vkDevice->getDevice(), 
			swapChain->getSwapChain(),
			commandBuffers->getCommandBuffers(), 
			vkDevice->getGraphicsQueue(),
			vkDevice->getPresentQueue(),
			swapChain->getSwapChainImages(),
			swapChain->getSwapChainExtent(),
			vkDevice->getPhysicalDevice(),
			commandPool->getCommandPool(), 
			buffers->getUniformBuffersMemory(), 
			LightBuffer->getUniformBuffersMemory(), 
			SSAOQuadBuffer->getUniformBuffersMemory(),
			CameraMovement);
		renderer->createSyncObjects();

	}
	void mainloop() {
		while (!glfwWindowShouldClose(EngineWindow->getWindow())) {
			renderer->processInput(EngineWindow->getWindow());
			renderer->drawFrame();
			glfwPollEvents();
		}
		vkDeviceWaitIdle(vkDevice->getDevice());
	}
	void cleanupSwapChain() {

		// Framebuffers
		for (auto framebuffer : frameBuffers->getSwapChainFramebuffers()) {
			vkDestroyFramebuffer(vkDevice->getDevice(), framebuffer, nullptr);
		}
		vkDestroyFramebuffer(vkDevice->getDevice(), frameBuffers->GetDisplaySceneFramebuffer(), nullptr);
		vkDestroyFramebuffer(vkDevice->getDevice(), frameBuffers->getShadowFramebuffer(), nullptr);
		vkDestroyFramebuffer(vkDevice->getDevice(), frameBuffers->GetGeometryPassFrameBuffer(), nullptr);
		vkDestroyFramebuffer(vkDevice->getDevice(), frameBuffers->GetSceneFrameBuffer(), nullptr);
		vkDestroyFramebuffer(vkDevice->getDevice(), frameBuffers->GetSSAOFramebuffer(), nullptr);
		vkDestroyFramebuffer(vkDevice->getDevice(), frameBuffers->GetSSAOBlurFramebuffer(), nullptr);

		//Free command buffers 
		vkFreeCommandBuffers(vkDevice->getDevice(), commandPool->getCommandPool(), static_cast<uint32_t>(commandBuffers->getCommandBuffers
			().size()), commandBuffers->getCommandBuffers().data());

		// Pipeline
		vkDestroyPipeline(vkDevice->getDevice(), pipeline->getGraphicsPipeline(), nullptr);
		vkDestroyPipeline(vkDevice->getDevice(), shadowPipeline->getGraphicsPipeline(), nullptr);
		vkDestroyPipeline(vkDevice->getDevice(), FloorPipeline->getGraphicsPipeline(), nullptr);
		vkDestroyPipeline(vkDevice->getDevice(), QuadPipeline->getGraphicsPipeline(), nullptr);
		vkDestroyPipeline(vkDevice->getDevice(), GeometryPassPipeline->getGraphicsPipeline(), nullptr);
		vkDestroyPipeline(vkDevice->getDevice(), SSAOBlurPipeline->getGraphicsPipeline(), nullptr);
		vkDestroyPipeline(vkDevice->getDevice(), SSAOQuadPipeline->getGraphicsPipeline(), nullptr);


		// Pipeline Layout
		vkDestroyPipelineLayout(vkDevice->getDevice(), shadowPipeline->getPipelineLayout(), nullptr);
		vkDestroyPipelineLayout(vkDevice->getDevice(), pipeline->getPipelineLayout(), nullptr);
		vkDestroyPipelineLayout(vkDevice->getDevice(), FloorPipeline->getPipelineLayout(), nullptr);
		vkDestroyPipelineLayout(vkDevice->getDevice(), QuadPipeline->getPipelineLayout(), nullptr);
		vkDestroyPipelineLayout(vkDevice->getDevice(), GeometryPassPipeline->getPipelineLayout(), nullptr);
		vkDestroyPipelineLayout(vkDevice->getDevice(), SSAOBlurPipeline->getPipelineLayout(), nullptr);
		vkDestroyPipelineLayout(vkDevice->getDevice(), SSAOQuadPipeline->getPipelineLayout(), nullptr);

		// Render pass
		vkDestroyRenderPass(vkDevice->getDevice(), renderPass->GetSceneRenderPass(), nullptr);
		vkDestroyRenderPass(vkDevice->getDevice(), renderPass->GetShadowRenderPass(), nullptr);
		vkDestroyRenderPass(vkDevice->getDevice(), renderPass->GetGeometryPassRenderPass(), nullptr);
		vkDestroyRenderPass(vkDevice->getDevice(), renderPass->GetQuadRenderPass(), nullptr);
		vkDestroyRenderPass(vkDevice->getDevice(), renderPass->GetSSAORenderPass(), nullptr);
		vkDestroyRenderPass(vkDevice->getDevice(), renderPass->GetSSAOBlurRenderPass(), nullptr);

		//Destroy swapchain image views
		for (auto imageView : swapChainImagesViews) {
			vkDestroyImageView(vkDevice->getDevice(), imageView, nullptr);
		}

		//Destroy the swapchain
		vkDestroySwapchainKHR(vkDevice->getDevice(), swapChain->getSwapChain(), nullptr);

		//Destroy uniform buffers 
		for (size_t i = 0; i < swapChain->getSwapChainImages().size(); i++) {
			vkDestroyBuffer(vkDevice->getDevice(), buffers->getUniformBuffers()[i], nullptr);
			vkFreeMemory(vkDevice->getDevice(), buffers->getUniformBuffersMemory()[i], nullptr);
		}

		vkDestroyDescriptorPool(vkDevice->getDevice(), descriptors->getDescriptorPool(), nullptr);
	}

	void cleanup() {

		if (enableValidationLayers) {
			DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
		}

		cleanupSwapChain();

		//Texture resources destroy
		vkDestroySampler(vkDevice->getDevice(), ImageRes->getSampler(), nullptr);
		vkDestroySampler(vkDevice->getDevice(), ImageRes->GetSceneSampler(), nullptr);
		vkDestroySampler(vkDevice->getDevice(), ImageRes->GetRepeatSampler(), nullptr);

		// Image 
		vkDestroyImage(vkDevice->getDevice(), ImageRes->getNormImage(), nullptr);
		vkDestroyImage(vkDevice->getDevice(), ImageRes->getTextureImage(), nullptr);
		vkDestroyImage(vkDevice->getDevice(), ImageRes->getDepthImage(), nullptr);
		vkDestroyImage(vkDevice->getDevice(), ImageRes->GetSceneImage(), nullptr);
		vkDestroyImage(vkDevice->getDevice(), ImageRes->getShadowImage(), nullptr);

		// Image view
		vkDestroyImageView(vkDevice->getDevice(), ImageRes->getTextureImageView(), nullptr);
		vkDestroyImageView(vkDevice->getDevice(), ImageRes->getNormImageView(), nullptr);
		vkDestroyImageView(vkDevice->getDevice(), ImageRes->getshadowImageView(), nullptr);

		// Image memory
		vkFreeMemory(vkDevice->getDevice(), ImageRes->getTextureImageMemory(), nullptr);
		vkFreeMemory(vkDevice->getDevice(), ImageRes->getDethMemory(), nullptr);
		vkFreeMemory(vkDevice->getDevice(), ImageRes->GetSceneImageMemory(), nullptr);
		vkFreeMemory(vkDevice->getDevice(), ImageRes->getShadowMemory(), nullptr);

		vkDestroyBuffer(vkDevice->getDevice(), buffers->getIndexBuffer(), nullptr);
		vkFreeMemory(vkDevice->getDevice(), buffers->getIndexBufferMemory(), nullptr);

		// Buffer
		vkDestroyBuffer(vkDevice->getDevice(), buffers->getVertexBuffer(), nullptr);
		vkFreeMemory(vkDevice->getDevice(), buffers->getVertexBufferMemory(), nullptr);

		// Quad buffer
		vkDestroyBuffer(vkDevice->getDevice(), QuadBuffer->getVertexBuffer(), nullptr);
		vkFreeMemory(vkDevice->getDevice(), QuadBuffer->getVertexBufferMemory(), nullptr);
		vkDestroyBuffer(vkDevice->getDevice(), QuadBuffer->getIndexBuffer(), nullptr);
		vkFreeMemory(vkDevice->getDevice(), QuadBuffer->getIndexBufferMemory(), nullptr);

		vkDestroyBuffer(vkDevice->getDevice(), FloorObjectBuffer->getVertexBuffer(), nullptr);
		vkFreeMemory(vkDevice->getDevice(), FloorObjectBuffer->getVertexBufferMemory(), nullptr);

		vkDestroyDescriptorSetLayout(vkDevice->getDevice(), descriptorSetLayout->getDescriptorSetLayout(), nullptr);
		vkDestroyDescriptorSetLayout(vkDevice->getDevice(), descriptorSetLayout->GetQuadDescriptorSetLayout(), nullptr);

		for (size_t i = 0; i < renderer->getMaxFrames(); i++) {
			vkDestroySemaphore(vkDevice->getDevice(), renderer->getFinishedSemaphore()[i], nullptr);
			vkDestroySemaphore(vkDevice->getDevice(), renderer->getAvailableSemaphore()[i], nullptr);
			vkDestroyFence(vkDevice->getDevice(), renderer->getFences()[i], nullptr);
		}

		vkDestroyCommandPool(vkDevice->getDevice(), commandPool->getCommandPool(), nullptr);

		//Destroy the logical device
		vkDestroyDevice(vkDevice->getDevice(), nullptr);
		//Destroy surface to window
		vkDestroySurfaceKHR(instance, surface, nullptr);
		//Destroy Vulkan instance
		vkDestroyInstance(instance, nullptr);

		glfwDestroyWindow(EngineWindow->getWindow());
		glfwTerminate();

		delete EngineWindow;
		EngineWindow = nullptr;

	}

	void recreateSwapChain() {

		vkDeviceWaitIdle(vkDevice->getDevice());

		cleanupSwapChain();

		swapChain->createSwapChain();

		VkFormat shadowDepth = ImageResHelper->findDepthFormat();
		renderPass->createSceneRenderPass(shadowDepth);
		renderPass->createShadowRenderPass(shadowDepth);
		pipeline->createGraphicsPipeline("shaders/vert.spv", "shaders/frag.spv");
		shadowPipeline->createGraphicsPipelineSingleShader("shaders/shadow.vert.spv");
		//depth resources
		ImageRes->createImageResources(swapChain->getSwapChainExtent());
		frameBuffers->createSwapChainFramebuffers(renderPass->GetQuadRenderPass(),ImageRes->getDepthImageView());
		frameBuffers->createShadowFramebuffer(renderPass->GetShadowRenderPass(), ImageRes->getshadowImageView());

		buffers->createUniformBuffer(sizeof(UniformBufferObject));
		descriptors->createDescriptorPool();
		descriptors->createDescriptorSets();
		commandBuffers->createCommandBuffers();

	}

	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData) {

		std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

		return VK_FALSE;
	}

	void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
		createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		createInfo.pfnUserCallback = debugCallback;

		VkDebugUtilsObjectNameInfoEXT name{};
		name.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
		name.pNext = nullptr;
		name.objectType = VK_OBJECT_TYPE_IMAGE_VIEW;
		name.objectHandle = (uint64_t)ImageRes->GetNoiseImageView();
		name.pObjectName = "noiseImageView";

		vkSetDebugUtilsObjectNameEXT(vkDevice->getDevice(), &name);
	}

	void setupDebugMessenger() {
		if (!enableValidationLayers) return;

		VkDebugUtilsMessengerCreateInfoEXT createInfo;
		populateDebugMessengerCreateInfo(createInfo);

		if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
			throw std::runtime_error("failed to set up debug messenger!");
		}
	}

	void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
		auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
		if (func != nullptr) {
			func(instance, debugMessenger, pAllocator);
		}
	}
};

//int main() {
//	vulkanEngine engine;
//
//	try {
//		engine.run();
//	}
//	catch (const std::exception& e) {
//		std::cerr << e.what() << std::endl;
//		return EXIT_FAILURE;
//	}
//	return EXIT_SUCCESS;
//}

