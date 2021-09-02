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

struct SwapChainSupportDetails {
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};

struct QueueFamilyIndices {
	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentFamily;

	bool isComplete() {
		return graphicsFamily.has_value() && presentFamily.has_value();
	}
};


class vulkanEngine {
public:
	void run() {
		initWindow();
		initVulkan();
		mainloop();
		cleanup();
	}
private:
	GLFWwindow* window;

	VkInstance instance; 
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	VkDevice logicalDevice;

	VkQueue graphicsQueue;
	VkQueue presentQueue;

	VkSurfaceKHR surface;
	VkSwapchainKHR swapChain;
	std::vector<VkImage> swapChainImages; // <- retrieving the swap chain images
	VkFormat swapChainImageFormat; // <- store the swap chain format for later use
	VkExtent2D swapChainExtent; // <- store the swapchain extent for later use

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

	std::unique_ptr<Object> objectData;
	std::unique_ptr<Object> SecondModel;
	std::unique_ptr<DescriptorSetLayout> descriptorSetLayout;
	std::unique_ptr<Descriptors> descriptors;
	std::unique_ptr<Input> GameInput = std::make_unique<Input>();
	std::unique_ptr<ImageResource> ImageRes;
	std::unique_ptr<ImageResource> ImageResHelper;


	void initWindow() {
		glfwInit();

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
		window = glfwCreateWindow(1920, 1080, "title", nullptr, nullptr);

		GameInput->setCameraSettings(window, glm::vec3(0.5f, .1f, 11.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	}

	void initVulkan() {
		createInstance();
		//setupDebugMessenger();
		createSurface();
		pickPhysicalDevice();
		createLogicalDevice();
		createSwapChain();
		createImageViews();
		ImageResHelper = std::make_unique<ImageResource>(physicalDevice);
		VkFormat shadowDepth = ImageResHelper->findDepthFormat();
		renderPass = std::make_unique<RenderPass>(logicalDevice, swapChainImageFormat);

		renderPass->createSceneRenderPass(shadowDepth);
		
		renderPass->createShadowRenderPass(shadowDepth);

		renderPass->createQuadRenderPass();
		renderPass->createQuadRenderPass();

		renderPass->createGeometryPassRenderPass(shadowDepth);

		renderPass->createSSAORenderPass();

		renderPass->createSSAOBlurRenderPass();

		descriptorSetLayout = std::make_unique<DescriptorSetLayout>(logicalDevice);
		descriptorSetLayout->createDescriptorSetLayout();
		descriptorSetLayout->createQuadDescriptorSetLayout();
		descriptorSetLayout->createSSAODescriptorSetLayout();
		descriptorSetLayout->createSSAOBlurDescriptorSetLayout();

		GeometryPassPipeline = std::make_unique<Pipeline>(logicalDevice, renderPass->GetGeometryPassRenderPass(), swapChainExtent, descriptorSetLayout->getDescriptorSetLayout());
		GeometryPassPipeline->createGeometryPassGraphicsPipeline("C:/Users/Shahb/source/repos/VulkanTesting/VulkanTesting/shaders/GeometryPass/ssao_geometry.vert.spv", "C:/Users/Shahb/source/repos/VulkanTesting/VulkanTesting/shaders/GeometryPass/ssao_geometry.frag.spv");

		SSAOQuadPipeline = std::make_unique<Pipeline>(logicalDevice, renderPass->GetSSAORenderPass(), swapChainExtent, descriptorSetLayout->GetSSAODescriptorSetLayout());
		SSAOQuadPipeline->createGraphicsPipelineOverlay("C:/Users/Shahb/source/repos/VulkanTesting/VulkanTesting/shaders/ssao/SSAOQuad.vert.spv", "C:/Users/Shahb/source/repos/VulkanTesting/VulkanTesting/shaders/ssao/SSAOQuad.frag.spv");

		SSAOBlurPipeline = std::make_unique<Pipeline>(logicalDevice, renderPass->GetSSAOBlurRenderPass(), swapChainExtent, descriptorSetLayout->GetSSAOBlurDescriptorSetLayout());
		SSAOBlurPipeline->createGraphicsPipelineOverlay("C:/Users/Shahb/source/repos/VulkanTesting/VulkanTesting/shaders/ssao/SSAOBlur.vert.spv","C:/Users/Shahb/source/repos/VulkanTesting/VulkanTesting/shaders/ssao/SSAOBlur.frag.spv");

		QuadPipeline = std::make_unique<Pipeline>(logicalDevice, renderPass->GetQuadRenderPass(), swapChainExtent, descriptorSetLayout->GetQuadDescriptorSetLayout());
		QuadPipeline->createGraphicsPipelineOverlay("C:/Users/Shahb/source/repos/VulkanTesting/VulkanTesting/shaders/quad.vert.spv","C:/Users/Shahb/source/repos/VulkanTesting/VulkanTesting/shaders/quad.frag.spv");

		shadowPipeline = std::make_unique<Pipeline>(logicalDevice, renderPass->GetShadowRenderPass(), swapChainExtent, descriptorSetLayout->getDescriptorSetLayout());
		shadowPipeline->createGraphicsPipelineSingleShader("C:/Users/Shahb/source/repos/VulkanTesting/VulkanTesting/shaders/shadow/shadow.vert.spv");

		pipeline = std::make_unique<Pipeline>(logicalDevice, renderPass->GetSceneRenderPass(), swapChainExtent, descriptorSetLayout->getDescriptorSetLayout());
		pipeline->createGraphicsPipeline("C:/Users/Shahb/source/repos/VulkanTesting/VulkanTesting/shaders/Object/vert.spv", "C:/Users/Shahb/source/repos/VulkanTesting/VulkanTesting/shaders/Object/frag.spv");

		FloorPipeline = std::make_unique<Pipeline>(logicalDevice, renderPass->GetSceneRenderPass(), swapChainExtent, descriptorSetLayout->getDescriptorSetLayout());
		FloorPipeline->createGraphicsPipeline("C:/Users/Shahb/source/repos/VulkanTesting/VulkanTesting/shaders/Object/textureShader.vert.spv", "C:/Users/Shahb/source/repos/VulkanTesting/VulkanTesting/shaders/Object/textureShader.frag.spv");

		QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
		commandPool = std::make_unique<CommandPool>(logicalDevice, physicalDevice, surface, indices.graphicsFamily);
		commandPool->createCommandPool(indices.graphicsFamily.value());

		ImageRes = std::make_unique<ImageResource>(logicalDevice, commandPool->getCommandPool(), graphicsQueue, physicalDevice, swapChainImageFormat);
		ImageRes->createDepthResources(swapChainExtent);

		/** ------------------------ FRAME BUFFERS ------------------------- */
		frameBuffers = std::make_unique<Framebuffers>(logicalDevice, swapChainImagesViews, renderPass->GetSceneRenderPass(), swapChainExtent);

		frameBuffers->createSwapChainFramebuffers(renderPass->GetQuadRenderPass(), ImageRes->getDepthImageView());

		frameBuffers->createShadowFramebuffer(renderPass->GetShadowRenderPass(), ImageRes->getshadowImageView());

		frameBuffers->createDisplaySceneFramebuffer(renderPass->GetSceneRenderPass(), ImageRes->GetSceneImageView(), ImageRes->getDepthImageView());

		frameBuffers->createGeometryPassFrameBuffer(renderPass->GetGeometryPassRenderPass(), ImageRes->GetGeometryImageView(), ImageRes->GetPositionImageView(), ImageRes->GetgNormalsImageView(), ImageRes->GetAlbedoImageView(), ImageRes->getDepthImageView());

		frameBuffers->createSSAOQuadFrameBuffer(renderPass->GetSSAORenderPass(), ImageRes->GetSSAOImageView());

		frameBuffers->createSSAOBlurQuadFrameBuffer(renderPass->GetSSAOBlurRenderPass(), ImageRes->GetSSAOBlurImageView());


		ImageTools::imageInfo F1Image{};
		F1Image.DiffuseLocation = "C:/Users/Shahb/Desktop/white.png";
		F1Image.specularLocation = "C:/Users/Shahb/Desktop/white.png";
		F1Image.AOLocation = "C:/Users/Shahb/Desktop/white.png";
		F1Image.EmissionLocation = "C:/Users/Shahb/Desktop/white.png";
		ImageRes->createTextureImage(F1Image);

		ImageTools::imageInfo FloorImageInfo{};
		FloorImageInfo.DiffuseLocation = "C:/Users/Shahb/Desktop/white.png";
		FloorImageInfo.specularLocation = "C:/Users/Shahb/Desktop/white.png";
		FloorImageInfo.AOLocation = "C:/Users/Shahb/Desktop/white.png";
		FloorImageInfo.EmissionLocation= "C:/Users/Shahb/Desktop/white.png";

		ImageRes->createTextureImage(FloorImageInfo);

		ImageRes->createTextureSampler();
		ImageRes->createSceneSampler();
		ImageRes->createRepeatSampler();

		std::array<glm::vec3, 3> CameraMovement = { GameInput->getCameraPos(), GameInput->getCameraFront(), GameInput->getcameraUp() };
		
		objectData = std::make_unique<Object>();
		SecondModel = std::make_unique <Object>();
		buffers = std::make_unique<Buffer>(logicalDevice, physicalDevice, commandPool->getCommandPool(), graphicsQueue, swapChainImages, swapChainExtent, CameraMovement);
	
		objectData->loadModel("C:/Users/Shahb/Desktop/f1carwithcubes.obj"); //f1_onthefloor f1carwithcubes
		SecondModel->loadModel("C:/Users/Shahb/Desktop/floor.obj");

		buffers->createVertexBuffer(sizeof(objectData->getVertexData()[0]) * objectData->getVertexData().size(), objectData->getVertexData());
		buffers->createIndexBuffer(sizeof(objectData->getIndexData()[0]) * objectData->getIndexData().size(), objectData->getIndexData());
		buffers->createUniformBuffer();

		FloorObjectBuffer = std::make_unique<Buffer>(logicalDevice, physicalDevice, commandPool->getCommandPool(), graphicsQueue, swapChainImages, swapChainExtent, CameraMovement);
		FloorObjectBuffer->createVertexBuffer(sizeof(SecondModel->getVertexData()[0]) * SecondModel->getVertexData().size(), SecondModel->getVertexData());
		FloorObjectBuffer->createUniformBuffer();

		QuadBuffer = std::make_unique<Buffer>(logicalDevice, physicalDevice, commandPool->getCommandPool(), graphicsQueue, swapChainImages, swapChainExtent, CameraMovement);
		QuadBuffer->createVertexBuffer(sizeof(objectData->GetQuadVertex()[0]) * objectData->GetQuadVertex().size(), objectData->GetQuadVertex());
		QuadBuffer->createIndexBuffer(sizeof(objectData->GetQuadIncies()[0]) * objectData->GetQuadIncies().size(), objectData->GetQuadIncies());

		SSAOQuadBuffer = std::make_unique<Buffer>(logicalDevice, physicalDevice, commandPool->getCommandPool(), graphicsQueue, swapChainImages, swapChainExtent, CameraMovement);
		SSAOQuadBuffer->createVertexBuffer(sizeof(objectData->GetQuadVertex()[0]) * objectData->GetQuadVertex().size(), objectData->GetQuadVertex());
		SSAOQuadBuffer->createIndexBuffer(sizeof(objectData->GetQuadIncies()[0]) * objectData->GetQuadIncies().size(), objectData->GetQuadIncies());
		SSAOQuadBuffer->createUniformBuffer();

		descriptors = std::make_unique<Descriptors>(
			logicalDevice,
			swapChainImages,
			buffers->getUniformBuffers(),
			buffers->getLightBuffers(),
			SSAOQuadBuffer->GetSSAOUniformBuffer(),
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
			FloorObjectBuffer->getLightBuffers(),
			FloorImageInfo.textureImageView,
			FloorImageInfo.specularImageView,
			F1Image.AOImageView,
			F1Image.EmissionImageView);

		descriptors->createDescriptorPool();
		descriptors->createDescriptorSets();

		commandBuffers = std::make_unique<CommandBuffers>(
			logicalDevice,
			frameBuffers->getSwapChainFramebuffers(),
			commandPool->getCommandPool(),
			renderPass->GetSceneRenderPass(),
			swapChainExtent,
			pipeline->getGraphicsPipeline(),
			pipeline->getComputePipeline(),
			physicalDevice,
			buffers->getVertexBuffer(),
			buffers->getIndexBuffer(),
			pipeline->getPipelineLayout(),
			descriptors->getDescriptorSets(),
			objectData->getVertexData(),
			shadowPipeline->getGraphicsPipeline(),
			frameBuffers->getShadowFramebuffer(),
			renderPass->GetShadowRenderPass(),
			shadowPipeline->getPipelineLayout(),
			shadowPipeline->getGraphicsPipeline(),
			SecondModel->getVertexData(),
			FloorObjectBuffer->getVertexBuffer(),
			objectData->GetQuadIncies(),
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
			logicalDevice, swapChain, 
			commandBuffers->getCommandBuffers(), 
			graphicsQueue, 
			presentQueue, 
			swapChainImages, 
			swapChainExtent,
			physicalDevice,
			commandPool->getCommandPool(), 
			buffers->getUniformBuffersMemory(), 
			buffers->getLightBuffersMemory(), 
			SSAOQuadBuffer->GetSSAOUniformBufferMemory(),
			CameraMovement);
		renderer->createSyncObjects();

	}
	void mainloop() {
		while (!glfwWindowShouldClose(window)) {
			renderer->processInput(window);
			renderer->drawFrame();
			glfwPollEvents();
		}
		vkDeviceWaitIdle(logicalDevice);
	}
	void cleanupSwapChain() {

		// Framebuffers
		for (auto framebuffer : frameBuffers->getSwapChainFramebuffers()) {
			vkDestroyFramebuffer(logicalDevice, framebuffer, nullptr);
		}
		vkDestroyFramebuffer(logicalDevice, frameBuffers->GetDisplaySceneFramebuffer(), nullptr);
		vkDestroyFramebuffer(logicalDevice, frameBuffers->getShadowFramebuffer(), nullptr);
		vkDestroyFramebuffer(logicalDevice, frameBuffers->GetGeometryPassFrameBuffer(), nullptr);
		vkDestroyFramebuffer(logicalDevice, frameBuffers->GetSceneFrameBuffer(), nullptr);
		vkDestroyFramebuffer(logicalDevice, frameBuffers->GetSSAOFramebuffer(), nullptr);
		vkDestroyFramebuffer(logicalDevice, frameBuffers->GetSSAOBlurFramebuffer(), nullptr);

		//Free command buffers 
		vkFreeCommandBuffers(logicalDevice, commandPool->getCommandPool(), static_cast<uint32_t>(commandBuffers->getCommandBuffers
			().size()), commandBuffers->getCommandBuffers().data());

		// Pipeline
		vkDestroyPipeline(logicalDevice, pipeline->getGraphicsPipeline(), nullptr);
		vkDestroyPipeline(logicalDevice, shadowPipeline->getGraphicsPipeline(), nullptr);
		vkDestroyPipeline(logicalDevice, FloorPipeline->getGraphicsPipeline(), nullptr);
		vkDestroyPipeline(logicalDevice, QuadPipeline->getGraphicsPipeline(), nullptr);
		vkDestroyPipeline(logicalDevice, GeometryPassPipeline->getGraphicsPipeline(), nullptr);
		vkDestroyPipeline(logicalDevice, SSAOBlurPipeline->getGraphicsPipeline(), nullptr);
		vkDestroyPipeline(logicalDevice, SSAOQuadPipeline->getGraphicsPipeline(), nullptr);


		// Pipeline Layout
		vkDestroyPipelineLayout(logicalDevice, shadowPipeline->getPipelineLayout(), nullptr);
		vkDestroyPipelineLayout(logicalDevice, pipeline->getPipelineLayout(), nullptr);
		vkDestroyPipelineLayout(logicalDevice, FloorPipeline->getPipelineLayout(), nullptr);
		vkDestroyPipelineLayout(logicalDevice, QuadPipeline->getPipelineLayout(), nullptr);
		vkDestroyPipelineLayout(logicalDevice, GeometryPassPipeline->getPipelineLayout(), nullptr);
		vkDestroyPipelineLayout(logicalDevice, SSAOBlurPipeline->getPipelineLayout(), nullptr);
		vkDestroyPipelineLayout(logicalDevice, SSAOQuadPipeline->getPipelineLayout(), nullptr);

		// Render pass
		vkDestroyRenderPass(logicalDevice, renderPass->GetSceneRenderPass(), nullptr);
		vkDestroyRenderPass(logicalDevice, renderPass->GetShadowRenderPass(), nullptr);
		vkDestroyRenderPass(logicalDevice, renderPass->GetGeometryPassRenderPass(), nullptr);
		vkDestroyRenderPass(logicalDevice, renderPass->GetQuadRenderPass(), nullptr);
		vkDestroyRenderPass(logicalDevice, renderPass->GetSSAORenderPass(), nullptr);
		vkDestroyRenderPass(logicalDevice, renderPass->GetSSAOBlurRenderPass(), nullptr);

		//Destroy swapchain image views
		for (auto imageView : swapChainImagesViews) {
			vkDestroyImageView(logicalDevice, imageView, nullptr);
		}

		//Destroy the swapchain
		vkDestroySwapchainKHR(logicalDevice, swapChain, nullptr);

		//Destroy uniform buffers 
		for (size_t i = 0; i < swapChainImages.size(); i++) {
			vkDestroyBuffer(logicalDevice, buffers->getUniformBuffers()[i], nullptr);
			vkFreeMemory(logicalDevice, buffers->getUniformBuffersMemory()[i], nullptr);
		}

		vkDestroyDescriptorPool(logicalDevice, descriptors->getDescriptorPool(), nullptr);
	}

	void cleanup() {
		if (enableValidationLayers) {
			DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
		}

		cleanupSwapChain();

		//Texture resources destroy
		vkDestroySampler(logicalDevice, ImageRes->getSampler(), nullptr);
		vkDestroySampler(logicalDevice, ImageRes->GetSceneSampler(), nullptr);
		vkDestroySampler(logicalDevice, ImageRes->GetRepeatSampler(), nullptr);

		// Image 
		vkDestroyImage(logicalDevice, ImageRes->getNormImage(), nullptr);
		vkDestroyImage(logicalDevice, ImageRes->getTextureImage(), nullptr);
		vkDestroyImage(logicalDevice, ImageRes->getDepthImage(), nullptr);
		vkDestroyImage(logicalDevice, ImageRes->GetSceneImage(), nullptr);
		vkDestroyImage(logicalDevice, ImageRes->getShadowImage(), nullptr);

		// Image view
		vkDestroyImageView(logicalDevice, ImageRes->getTextureImageView(), nullptr);
		vkDestroyImageView(logicalDevice, ImageRes->getNormImageView(), nullptr);
		vkDestroyImageView(logicalDevice, ImageRes->getshadowImageView(), nullptr);

		// Image memory
		vkFreeMemory(logicalDevice, ImageRes->getTextureImageMemory(), nullptr);
		vkFreeMemory(logicalDevice, ImageRes->getDethMemory(), nullptr);
		vkFreeMemory(logicalDevice, ImageRes->GetSceneImageMemory(), nullptr);
		vkFreeMemory(logicalDevice, ImageRes->getShadowMemory(), nullptr);

		vkDestroyBuffer(logicalDevice, buffers->getIndexBuffer(), nullptr);
		vkFreeMemory(logicalDevice, buffers->getIndexBufferMemory(), nullptr);

		// Buffer
		vkDestroyBuffer(logicalDevice, buffers->getVertexBuffer(), nullptr);
		vkFreeMemory(logicalDevice, buffers->getVertexBufferMemory(), nullptr);

		// Quad buffer
		vkDestroyBuffer(logicalDevice, QuadBuffer->getVertexBuffer(), nullptr);
		vkFreeMemory(logicalDevice, QuadBuffer->getVertexBufferMemory(), nullptr);
		vkDestroyBuffer(logicalDevice, QuadBuffer->getIndexBuffer(), nullptr);
		vkFreeMemory(logicalDevice, QuadBuffer->getIndexBufferMemory(), nullptr);

		vkDestroyBuffer(logicalDevice, FloorObjectBuffer->getVertexBuffer(), nullptr);
		vkFreeMemory(logicalDevice, FloorObjectBuffer->getVertexBufferMemory(), nullptr);

		vkDestroyDescriptorSetLayout(logicalDevice, descriptorSetLayout->getDescriptorSetLayout(), nullptr);
		vkDestroyDescriptorSetLayout(logicalDevice, descriptorSetLayout->GetQuadDescriptorSetLayout(), nullptr);

		for (size_t i = 0; i < renderer->getMaxFrames(); i++) {
			vkDestroySemaphore(logicalDevice, renderer->getFinishedSemaphore()[i], nullptr);
			vkDestroySemaphore(logicalDevice, renderer->getAvailableSemaphore()[i], nullptr);
			vkDestroyFence(logicalDevice, renderer->getFences()[i], nullptr);
		}

		vkDestroyCommandPool(logicalDevice, commandPool->getCommandPool(), nullptr);

		//Destroy the logical device
		vkDestroyDevice(logicalDevice, nullptr);
		//Destroy surface to window
		vkDestroySurfaceKHR(instance, surface, nullptr);
		//Destroy Vulkan instance
		vkDestroyInstance(instance, nullptr);

		glfwDestroyWindow(window);
		glfwTerminate();

	}

	void recreateSwapChain() {
		vkDeviceWaitIdle(logicalDevice);

		cleanupSwapChain();

		createSwapChain();
		createImageViews();
		VkFormat shadowDepth = ImageResHelper->findDepthFormat();
		renderPass->createSceneRenderPass(shadowDepth);
		renderPass->createShadowRenderPass(shadowDepth);
		pipeline->createGraphicsPipeline("C:/Users/Shahb/source/repos/VulkanTesting/VulkanTesting/shaders/vert.spv", "C:/Users/Shahb/source/repos/VulkanTesting/VulkanTesting/shaders/frag.spv");
		shadowPipeline->createGraphicsPipelineSingleShader("C:/Users/Shahb/source/repos/VulkanTesting/VulkanTesting/shaders/shadow.vert.spv");
		//depth resources
		ImageRes->createDepthResources(swapChainExtent);
		frameBuffers->createSwapChainFramebuffers(renderPass->GetQuadRenderPass(),ImageRes->getDepthImageView());
		frameBuffers->createShadowFramebuffer(renderPass->GetShadowRenderPass(), ImageRes->getshadowImageView());

		buffers->createUniformBuffer();
		descriptors->createDescriptorPool();
		descriptors->createDescriptorSets();
		commandBuffers->createCommandBuffers();

	}

	void createInstance() {

		if (enableValidationLayers && !checkValidationLayerSupport()) {
			throw std::runtime_error("validation layers requested, but not available!");
		}

		VkApplicationInfo appInfo{};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "Vulkan Engine";
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "No Engine";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_0;

		VkInstanceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;

		auto extensions = GetRequiredExtensions();
		createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		createInfo.ppEnabledExtensionNames = extensions.data();

		VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
		if (enableValidationLayers) {
			createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
			createInfo.ppEnabledLayerNames = validationLayers.data();

			/*populateDebugMessengerCreateInfo(debugCreateInfo);
			createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;*/
		}
		else {
			createInfo.enabledLayerCount = 0;
			createInfo.pNext = nullptr;
		}

		if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
			throw std::runtime_error("failed to create vulkan instance!");
		}

	}

	void createSurface() {
		if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS) {
			throw std::runtime_error("failed to create window surface!");
		}
	}

	void pickPhysicalDevice() {
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

		if (deviceCount == 0) {
			throw std::runtime_error("failed to find GPUs with Vulkan support!");
		}
		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

		for (const auto& device : devices) {
			if (isDeviceSuitable(device)) {
				physicalDevice = device;
				break;
			}
		}
		if (physicalDevice == VK_NULL_HANDLE) {
			throw std::runtime_error("failed to find a suitable GPU!");
		}
	}

	void createLogicalDevice() {
		QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };


		float queuePriority = 1.0f;
		for (uint32_t queueFamily : uniqueQueueFamilies) {
			VkDeviceQueueCreateInfo queueCreateInfo{};
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex = queueFamily;
			queueCreateInfo.queueCount = 1;
			queueCreateInfo.pQueuePriorities = &queuePriority;
			queueCreateInfos.push_back(queueCreateInfo);
		}

		VkPhysicalDeviceFeatures deviceFeatures{};
		deviceFeatures.samplerAnisotropy = VK_TRUE;

		/* Creating the logical device */
		VkDeviceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
		createInfo.pQueueCreateInfos = queueCreateInfos.data();
		createInfo.pEnabledFeatures = &deviceFeatures;
		createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
		createInfo.ppEnabledExtensionNames = deviceExtensions.data();
		createInfo.pEnabledFeatures = 0;

		if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &logicalDevice) != VK_SUCCESS) {
			throw std::runtime_error("failed to create logical device!");
		}

		vkGetDeviceQueue(logicalDevice, indices.graphicsFamily.value(), 0, &graphicsQueue);
		vkGetDeviceQueue(logicalDevice, indices.presentFamily.value(), 0, &presentQueue);
	}

	void createSwapChain() {
		SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice);

		VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
		VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
		VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

		//amount of images in the swap chain
		uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;

		//make sure not to exceed the max number of images 
		if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
			imageCount = swapChainSupport.capabilities.maxImageCount;
		}

		VkSwapchainCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = surface;
		createInfo.minImageCount = imageCount;
		createInfo.imageFormat = surfaceFormat.format;
		createInfo.imageColorSpace = surfaceFormat.colorSpace;
		createInfo.imageExtent = extent;
		createInfo.imageArrayLayers = 1; //always one unless it's stereoscopic 3d application
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		//how to handle swap chain images that will be used across multiple families

		QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
		uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

		if (indices.graphicsFamily != indices.presentFamily) {
			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT; //image is owned by one queue family at a time
			createInfo.queueFamilyIndexCount = 2;
			createInfo.pQueueFamilyIndices = queueFamilyIndices;
		}
		else {
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE; //images can be used across multiple families 
			createInfo.queueFamilyIndexCount = 0;
			createInfo.pQueueFamilyIndices = nullptr;
		}

		createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode = presentMode;
		createInfo.clipped = VK_TRUE;
		createInfo.oldSwapchain = VK_NULL_HANDLE;

		if (vkCreateSwapchainKHR(logicalDevice, &createInfo, nullptr, &swapChain) != VK_SUCCESS) {
			throw std::runtime_error("failed to create swap chain!");
		}

		vkGetSwapchainImagesKHR(logicalDevice, swapChain, &imageCount, nullptr);
		swapChainImages.resize(imageCount);
		vkGetSwapchainImagesKHR(logicalDevice, swapChain, &imageCount, swapChainImages.data());

		swapChainImageFormat = surfaceFormat.format;
		swapChainExtent = extent;
	}

	void createImageViews() {
		//resize to fit all the image views we will be creating -> make inline with swapchanimages amount
		swapChainImagesViews.resize(swapChainImages.size());

		for (size_t i = 0; i < swapChainImages.size(); i++) {
			VkImageViewCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			createInfo.image = swapChainImages[i];
			createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			createInfo.format = swapChainImageFormat;
			createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			createInfo.subresourceRange.baseMipLevel = 0;
			createInfo.subresourceRange.levelCount = 1;
			createInfo.subresourceRange.baseArrayLayer = 0;
			createInfo.subresourceRange.layerCount = 1;

			if (vkCreateImageView(logicalDevice, &createInfo, nullptr, &swapChainImagesViews[i]) != VK_SUCCESS) {
				throw std::runtime_error("failed to create image views!");
			}
		}
	}

	bool isDeviceSuitable(VkPhysicalDevice device) {
		QueueFamilyIndices indices = findQueueFamilies(device);


		bool extensionSupport = checkDeviceExtensionSupport(device);

		bool swapChainAdequate = false;
		if (extensionSupport) {
			SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
			swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
		}

		VkPhysicalDeviceFeatures supportedFeatures;
		vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

		return indices.isComplete() && extensionSupport && swapChainAdequate &&
			supportedFeatures.samplerAnisotropy;
	}

	bool checkDeviceExtensionSupport(VkPhysicalDevice device) {
		uint32_t extensionCount;
		vkEnumerateDeviceExtensionProperties(device, nullptr ,&extensionCount, nullptr);

		std::vector<VkExtensionProperties> availableExtensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

		std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

		for (const auto& extension : availableExtensions) {
			requiredExtensions.erase(extension.extensionName);
		}

		return requiredExtensions.empty();
	}

	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device) {
		QueueFamilyIndices indices;

		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

		int i = 0;
		for (const auto& queueFamily : queueFamilies) {
			if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
				indices.graphicsFamily = i;
			}
			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
			if (presentSupport) {
				indices.presentFamily = i;
			}

			if (indices.isComplete()) {
				break;
			}
			i++;
		}

		return indices;
	}

	SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device) {
		SwapChainSupportDetails details;

		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

		if (formatCount != 0) {
			details.formats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
		}

		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

		if (presentModeCount != 0) {
			details.presentModes.resize(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
		}

		return details;
	}

	//Surface format for the swapchain -> getting the right colours
	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {

		for (const auto& availableFormat : availableFormats) {
			if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
				return availableFormat;
			}
		}
		return availableFormats[0]; //else just take the first one available 
	}

	//Swap chain present modes
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
		for (const auto& availablePresentMode : availablePresentModes) {
			if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
				return availablePresentMode;
			}
		}
		return VK_PRESENT_MODE_FIFO_KHR;
	}

	//Matching swap chain extent to the resolution of the swap chain images -> same res as window
	//need to set VkSurfaceCapabilitiesKHR currentExtent member 
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilites) {
		if (capabilites.currentExtent.width != UINT32_MAX) {
			return capabilites.currentExtent;
		}
		else {
			int width, height;
			glfwGetFramebufferSize(window, &width, &height);

			VkExtent2D actualExtent = {
				static_cast<uint32_t>(width),
				static_cast<uint32_t>(height)
			};

			actualExtent.width = std::clamp(actualExtent.width, capabilites.minImageExtent.width, 
				capabilites.maxImageExtent.width);
			actualExtent.height = std::clamp(actualExtent.height, capabilites.minImageExtent.height,
				capabilites.maxImageExtent.height);

			return actualExtent;
		}

	}


	// Validation layers
	bool checkValidationLayerSupport() {
		uint32_t layerCount;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

		std::vector<VkLayerProperties> availableLayers(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

		for (const char* layerName : validationLayers) {
			bool layerFound = false;

			for (const auto& layerProperties : availableLayers) {
				if (strcmp(layerName, layerProperties.layerName) == 0) {
					layerFound = true;
					break;
				}
			}

			if (!layerFound) {
				return false;
			}
		}
	
		return true;
	}

	// Debug callback -- Get list of required extensions
	std::vector<const char*> GetRequiredExtensions() {
		uint32_t glfwExtensionCount = 0;

		const char** glfwExtensions;
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

		if (enableValidationLayers) {
			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		}
		return extensions;
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

		vkSetDebugUtilsObjectNameEXT(logicalDevice, &name);
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

int main() {
	vulkanEngine engine;

	try {
		engine.run();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}

