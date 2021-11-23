
#include "vulkanengine.h"


void VE::vulkanEngine::initEngine() {

	/** Compile shaders */
	//std::system("C:/Users/Shahb/source/repos/VulkanTesting/VulkanTesting/shaders/compile.bat");

	std::cout << "Launching Engine " << std::endl;

	EngineWindow = std::make_unique<VE::Window>(2560, 1440, "title");
	EngineWindow->initWindow();

	CameraMovementInput = std::make_unique<Input>(EngineWindow->getWindow(), glm::vec3(0.5f, .1f, 11.0f),
		glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));

}

void VE::vulkanEngine::initVulkan() {
	
	std::cout << "Initialising Vulkan .. " << std::endl;
	createInstance();
	createSurface();
	Rendering();

}

void VE::vulkanEngine::mainloop() {

	while (!glfwWindowShouldClose(EngineWindow->getWindow())) {
		renderer->processInput(EngineWindow->getWindow());
		//glfwSetCursorPosCallback(EngineWindow->getWindow(), mouse_callback);
		renderer->drawFrame();
		glfwPollEvents();
	}
	vkDeviceWaitIdle(vkDevice->getDevice());
}

void VE::vulkanEngine::run() {

	initEngine();
	initVulkan();
	mainloop();
	cleanup();
}

void VE::vulkanEngine::Rendering() {

	std::cout << "Initialising Rendering resources" << std::endl;


	VkImage DepthpositionImage;
	VkImageView DepthpositionsImageView;
	VkDeviceMemory DepthpositionsMemory;

	vkDevice = std::make_unique<VE::VulkanDevice>(instance, surface);
	vkDevice->createDevice();

	swapChain = std::make_unique<VE::SwapChain>(instance, vkDevice->getDevice(), vkDevice->getPhysicalDevice(), surface, EngineWindow->getWindow());
	swapChain->createSwapChain();

	ImageResource DepthFormat(vkDevice->getPhysicalDevice());
	VkFormat shadowDepth = DepthFormat.findDepthFormat();

	/** RENDER PASSES **/
	renderPass = std::make_unique<RenderPass>(vkDevice->getDevice(), swapChain->getSwapChainImageFormat());
	renderPass->createSceneRenderPass(shadowDepth);
	renderPass->createShadowRenderPass(shadowDepth);
	renderPass->createQuadRenderPass();
	renderPass->createQuadRenderPass();
	renderPass->createGeometryPassRenderPass(shadowDepth);
	renderPass->createSSAORenderPass();
	renderPass->createSSAOBlurRenderPass();
	renderPass->createRenderPass_Positions(VK_FORMAT_R32G32B32A32_SFLOAT);

	/** DESCRIPTORSET LAYOUT **/
	descriptorSetLayout = std::make_unique<DescriptorSetLayout>(vkDevice->getDevice());
	descriptorSetLayout->createDescriptorSetLayout();
	descriptorSetLayout->createQuadDescriptorSetLayout();
	descriptorSetLayout->createSSAODescriptorSetLayout();
	descriptorSetLayout->createSSAOBlurDescriptorSetLayout();
	
	std::unique_ptr<Pipeline> positionsPipeline = std::make_unique<Pipeline>(vkDevice->getDevice(), renderPass->GetRenderPass(), swapChain->getSwapChainExtent(), descriptorSetLayout->GetSSAODescriptorSetLayout());
	positionsPipeline->createGraphicsPipelineOverlay("shaders/quad.vert.spv", "shaders/positionsOutput.frag.spv");

	GeometryPassPipeline = std::make_unique<Pipeline>(vkDevice->getDevice(), renderPass->GetGeometryPassRenderPass(), swapChain->getSwapChainExtent(), descriptorSetLayout->getDescriptorSetLayout());
	GeometryPassPipeline->createGeometryPassGraphicsPipeline("shaders/GeometryPass/ssao_geometry.vert.spv", "shaders/GeometryPass/ssao_geometry.frag.spv");

	SSAOQuadPipeline = std::make_unique<Pipeline>(vkDevice->getDevice(), renderPass->GetSSAORenderPass(), swapChain->getSwapChainExtent(), descriptorSetLayout->GetSSAODescriptorSetLayout());
	SSAOQuadPipeline->createGraphicsPipelineOverlay("shaders/ssao/SSAOQuad.vert.spv", "shaders/ssao/aao.frag.spv");

	SSAOBlurPipeline = std::make_unique<Pipeline>(vkDevice->getDevice(), renderPass->GetSSAOBlurRenderPass(), swapChain->getSwapChainExtent(), descriptorSetLayout->GetSSAOBlurDescriptorSetLayout());
	SSAOBlurPipeline->createGraphicsPipelineOverlay("shaders/ssao/SSAOBlur.vert.spv", "shaders/ssao/SSAOBlur.frag.spv");

	QuadPipeline = std::make_unique<Pipeline>(vkDevice->getDevice(), renderPass->GetQuadRenderPass(), swapChain->getSwapChainExtent(), descriptorSetLayout->GetQuadDescriptorSetLayout());
	QuadPipeline->createGraphicsPipelineOverlay("shaders/quad.vert.spv", "shaders/quad.frag.spv");

	shadowPipeline = std::make_unique<Pipeline>(vkDevice->getDevice(), renderPass->GetShadowRenderPass(), swapChain->getSwapChainExtent(), descriptorSetLayout->getDescriptorSetLayout());
	shadowPipeline->createGraphicsPipelineSingleShader("shaders/shadow/shadow.vert.spv");

	MainModelPipeline = std::make_unique<Pipeline>(vkDevice->getDevice(), renderPass->GetSceneRenderPass(), swapChain->getSwapChainExtent(), descriptorSetLayout->getDescriptorSetLayout());
	MainModelPipeline->createGraphicsPipeline("shaders/Object/vert.spv", "shaders/Object/frag.spv");

	FloorPipeline = std::make_unique<Pipeline>(vkDevice->getDevice(), renderPass->GetSceneRenderPass(), swapChain->getSwapChainExtent(), descriptorSetLayout->getDescriptorSetLayout());
	FloorPipeline->createGraphicsPipeline("shaders/Object/textureShader.vert.spv", "shaders/Object/textureShader.frag.spv");

	VE::QueueFamilyIndices indices = vkDevice->findQueueFamilies(vkDevice->getPhysicalDevice());
	commandPool = std::make_unique<CommandPool>(vkDevice->getDevice(), vkDevice->getPhysicalDevice(), surface, indices.graphics);
	commandPool->createCommandPool(indices.graphics.value());

	ImageRes = std::make_unique<ImageResource>(vkDevice->getDevice(), commandPool->getCommandPool(), vkDevice->getGraphicsQueue(), vkDevice->getPhysicalDevice(), swapChain->getSwapChainImageFormat());

	// positions images, for positions from depth buffer
	ImageRes->createImage(swapChain->getSwapChainExtent().width, swapChain->getSwapChainExtent().height,
		VK_FORMAT_R32G32B32A32_SFLOAT,
		VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, DepthpositionImage, DepthpositionsMemory);

	DepthpositionsImageView = ImageRes->createImageView(DepthpositionImage, VK_FORMAT_R32G32B32A32_SFLOAT, VK_IMAGE_ASPECT_COLOR_BIT);

	std::vector<VkImageView> positionsfb = { DepthpositionsImageView };

	ImageRes->createImageResources(swapChain->getSwapChainExtent());
	/** ------------------------ FRAME BUFFERS ------------------------- */
	frameBuffers = std::make_unique<Framebuffers>(vkDevice->getDevice(), swapChain->getSwapChainImageViews(), renderPass->GetSceneRenderPass(), swapChain->getSwapChainExtent());
	frameBuffers->createSwapChainFramebuffers(renderPass->GetQuadRenderPass(), ImageRes->getDepthImageView());
	frameBuffers->createShadowFramebuffer(renderPass->GetShadowRenderPass(), ImageRes->getshadowImageView());
	frameBuffers->createDisplaySceneFramebuffer(renderPass->GetSceneRenderPass(), ImageRes->GetSceneImageView(), ImageRes->getDepthImageView());
	frameBuffers->createGeometryPassFrameBuffer(renderPass->GetGeometryPassRenderPass(), ImageRes->GetGeometryImageView(), ImageRes->GetPositionImageView(), ImageRes->GetgNormalsImageView(), ImageRes->GetAlbedoImageView(), ImageRes->getDepthImageView());
	frameBuffers->createSSAOQuadFrameBuffer(renderPass->GetSSAORenderPass(), ImageRes->GetSSAOImageView());
	frameBuffers->createSSAOBlurQuadFrameBuffer(renderPass->GetSSAOBlurRenderPass(), ImageRes->GetSSAOBlurImageView());
	
	frameBuffers->createFramebuffer(renderPass->GetRenderPass(), positionsfb);

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
	FloorImageInfo.EmissionLocation = "Textures/white.png";

	ImageRes->createTextureImage(FloorImageInfo);
	ImageRes->createTextureSampler();
	ImageRes->createSceneSampler();
	ImageRes->createRepeatSampler();

	std::array<glm::vec3, 3> CameraMovement = { CameraMovementInput->getCameraPos(), CameraMovementInput->getCameraFront(), CameraMovementInput->getcameraUp() };

	Buffer positionsQuad = Buffer(vkDevice->getDevice(), vkDevice->getPhysicalDevice(), commandPool->getCommandPool(), vkDevice->getGraphicsQueue(), swapChain->getSwapChainImages(), swapChain->getSwapChainExtent());
	positionsQuad.createVertexBuffer(sizeof(QuadData.Model.GetQuadVertex()[0])* QuadData.Model.GetQuadVertex().size(), QuadData.Model.GetQuadVertex());
	positionsQuad.createIndexBuffer(sizeof(QuadData.Model.GetQuadIncies()[0])* QuadData.Model.GetQuadIncies().size(), QuadData.Model.GetQuadIncies());

	MainModelBuffer = std::make_unique<Buffer>(vkDevice->getDevice(), vkDevice->getPhysicalDevice(), commandPool->getCommandPool(), vkDevice->getGraphicsQueue(), swapChain->getSwapChainImages(), swapChain->getSwapChainExtent());
	
	F1Car.Model.LoadModel("Models/Winter.obj"); //f1_onthefloor f1carwithcubes
	Floor.Model.LoadModel("Models/floor.obj");

	LightBuffer = std::make_unique<Buffer>(vkDevice->getDevice(), vkDevice->getPhysicalDevice(), commandPool->getCommandPool(), vkDevice->getGraphicsQueue(), swapChain->getSwapChainImages(), swapChain->getSwapChainExtent());
	LightBuffer->createUniformBuffer(sizeof(Light));
	
	MainModelBuffer->createVertexBuffer(sizeof(F1Car.Model.getVertexData()[0]) * F1Car.Model.getVertexData().size(), F1Car.Model.getVertexData());
	MainModelBuffer->createIndexBuffer(sizeof(F1Car.Model.getIndexData()[0]) * F1Car.Model.getIndexData().size(), F1Car.Model.getIndexData());
	MainModelBuffer->createUniformBuffer(sizeof(UniformBufferObject));

	FloorObjectBuffer = std::make_unique<Buffer>(vkDevice->getDevice(), vkDevice->getPhysicalDevice(), commandPool->getCommandPool(), vkDevice->getGraphicsQueue(), swapChain->getSwapChainImages(), swapChain->getSwapChainExtent());
	FloorObjectBuffer->createVertexBuffer(sizeof(Floor.Model.getVertexData()[0]) * Floor.Model.getVertexData().size(), Floor.Model.getVertexData());
	FloorObjectBuffer->createIndexBuffer(sizeof(Floor.Model.getIndexData()[0])* Floor.Model.getIndexData().size(), Floor.Model.getIndexData());
	FloorObjectBuffer->createUniformBuffer(sizeof(UniformBufferObject));

	QuadBuffer = std::make_unique<Buffer>(vkDevice->getDevice(), vkDevice->getPhysicalDevice(), commandPool->getCommandPool(), vkDevice->getGraphicsQueue(), swapChain->getSwapChainImages(), swapChain->getSwapChainExtent());
	QuadBuffer->createVertexBuffer(sizeof(QuadData.Model.GetQuadVertex()[0]) * QuadData.Model.GetQuadVertex().size(), QuadData.Model.GetQuadVertex());
	QuadBuffer->createIndexBuffer(sizeof(QuadData.Model.GetQuadIncies()[0]) * QuadData.Model.GetQuadIncies().size(), QuadData.Model.GetQuadIncies());

	SSAOQuadBuffer = std::make_unique<Buffer>(vkDevice->getDevice(), vkDevice->getPhysicalDevice(), commandPool->getCommandPool(), vkDevice->getGraphicsQueue(), swapChain->getSwapChainImages(), swapChain->getSwapChainExtent());
	SSAOQuadBuffer->createVertexBuffer(sizeof(QuadData.Model.GetQuadVertex()[0]) * QuadData.Model.GetQuadVertex().size(), QuadData.Model.GetQuadVertex());
	SSAOQuadBuffer->createIndexBuffer(sizeof(QuadData.Model.GetQuadIncies()[0]) *  QuadData.Model.GetQuadIncies().size(),  QuadData.Model.GetQuadIncies());
	SSAOQuadBuffer->createUniformBuffer(sizeof(KernelSample));

	descriptors = std::make_unique<Descriptors>(
		vkDevice->getDevice(),
		swapChain->getSwapChainImages(),
		MainModelBuffer->getUniformBuffers(),
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
		F1Image.EmissionImageView,
		ImageRes->getDepthImageView(),
		DepthpositionsImageView);

	descriptors->createDescriptorPool();
	descriptors->createDescriptorSets();

	commandBuffers = std::make_unique<CommandBuffers>(
		vkDevice->getDevice(),
		frameBuffers->getSwapChainFramebuffers(),
		commandPool->getCommandPool(),
		renderPass->GetSceneRenderPass(),
		swapChain->getSwapChainExtent(),
		MainModelPipeline->getGraphicsPipeline(),
		MainModelPipeline->getComputePipeline(),
		vkDevice->getPhysicalDevice(),
		MainModelBuffer->getVertexBuffer(),
		MainModelBuffer->getIndexBuffer(),
		MainModelPipeline->getPipelineLayout(),
		descriptors->getDescriptorSets(),
		F1Car.Model.getVertexData(),
		shadowPipeline->getGraphicsPipeline(),
		frameBuffers->getShadowFramebuffer(),
		renderPass->GetShadowRenderPass(),
		shadowPipeline->getPipelineLayout(),
		shadowPipeline->getGraphicsPipeline(),
		Floor.Model.getVertexData(),
		FloorObjectBuffer->getVertexBuffer(),
		F1Car.Model.GetQuadIncies(),
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
		descriptors->GetFloorDescriptorSet(),
		positionsQuad.getVertexBuffer(),
		positionsQuad.getIndexBuffer(),
		frameBuffers->GetFramebufers(),
		renderPass->GetRenderPass(),
		positionsPipeline->getGraphicsPipeline(),
		positionsPipeline->getPipelineLayout(),
		F1Car.Model.getIndexData(),
		Floor.Model.getIndexData(),
		FloorObjectBuffer->getIndexBuffer());

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
		MainModelBuffer->getUniformBuffersMemory(),
		LightBuffer->getUniformBuffersMemory(),
		SSAOQuadBuffer->getUniformBuffersMemory(),
		CameraMovement);
	
	renderer->createSyncObjects();

}

void VE::vulkanEngine::cleanupSwapChain() {

	// Destroy Graphics pipelines
	vkDestroyPipeline(vkDevice->getDevice(), shadowPipeline->getGraphicsPipeline(), nullptr);
	vkDestroyPipeline(vkDevice->getDevice(), FloorPipeline->getGraphicsPipeline(), nullptr);
	vkDestroyPipeline(vkDevice->getDevice(), GeometryPassPipeline->getGraphicsPipeline(), nullptr);
	vkDestroyPipeline(vkDevice->getDevice(), QuadPipeline->getGraphicsPipeline(), nullptr);
	vkDestroyPipeline(vkDevice->getDevice(), SSAOQuadPipeline->getGraphicsPipeline(), nullptr);
	vkDestroyPipeline(vkDevice->getDevice(), SSAOBlurPipeline->getGraphicsPipeline(), nullptr);

	// Destroy PipelineLayout
	vkDestroyPipelineLayout(vkDevice->getDevice(), shadowPipeline->getPipelineLayout(), nullptr);
	vkDestroyPipelineLayout(vkDevice->getDevice(), FloorPipeline->getPipelineLayout(), nullptr);
	vkDestroyPipelineLayout(vkDevice->getDevice(), GeometryPassPipeline->getPipelineLayout(), nullptr);
	vkDestroyPipelineLayout(vkDevice->getDevice(), QuadPipeline->getPipelineLayout(), nullptr);
	vkDestroyPipelineLayout(vkDevice->getDevice(), SSAOQuadPipeline->getPipelineLayout(), nullptr);
	vkDestroyPipelineLayout(vkDevice->getDevice(), SSAOBlurPipeline->getPipelineLayout(), nullptr);

	// Destroy render pass
	vkDestroyRenderPass(vkDevice->getDevice(), renderPass->GetShadowRenderPass(), nullptr);
	vkDestroyRenderPass(vkDevice->getDevice(), renderPass->GetRenderPass(), nullptr);
	vkDestroyRenderPass(vkDevice->getDevice(), renderPass->GetSceneRenderPass(), nullptr);
	vkDestroyRenderPass(vkDevice->getDevice(), renderPass->GetSSAOBlurRenderPass(), nullptr);
	vkDestroyRenderPass(vkDevice->getDevice(), renderPass->GetSSAORenderPass(), nullptr);
	vkDestroyRenderPass(vkDevice->getDevice(), renderPass->GetGeometryPassRenderPass(), nullptr);
}


void VE::vulkanEngine::cleanup() {

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

	vkDestroyBuffer(vkDevice->getDevice(), MainModelBuffer->getIndexBuffer(), nullptr);
	vkFreeMemory(vkDevice->getDevice(), MainModelBuffer->getIndexBufferMemory(), nullptr);

	// Buffer
	vkDestroyBuffer(vkDevice->getDevice(), MainModelBuffer->getVertexBuffer(), nullptr);
	vkFreeMemory(vkDevice->getDevice(), MainModelBuffer->getVertexBufferMemory(), nullptr);

	// Quad buffer
	vkDestroyBuffer(vkDevice->getDevice(), QuadBuffer->getVertexBuffer(), nullptr);
	vkFreeMemory(vkDevice->getDevice(), QuadBuffer->getVertexBufferMemory(), nullptr);
	vkDestroyBuffer(vkDevice->getDevice(), QuadBuffer->getIndexBuffer(), nullptr);
	vkFreeMemory(vkDevice->getDevice(), QuadBuffer->getIndexBufferMemory(), nullptr);

	vkDestroyBuffer(vkDevice->getDevice(), FloorObjectBuffer->getVertexBuffer(), nullptr);
	vkFreeMemory(vkDevice->getDevice(), FloorObjectBuffer->getVertexBufferMemory(), nullptr);

	// Destroy descriptor set layouts
	vkDestroyDescriptorSetLayout(vkDevice->getDevice(), descriptorSetLayout->getDescriptorSetLayout(), nullptr);
	vkDestroyDescriptorSetLayout(vkDevice->getDevice(), descriptorSetLayout->GetQuadDescriptorSetLayout(), nullptr);
	vkDestroyDescriptorSetLayout(vkDevice->getDevice(), descriptorSetLayout->GetSSAODescriptorSetLayout(), nullptr);
	vkDestroyDescriptorSetLayout(vkDevice->getDevice(), descriptorSetLayout->GetSSAOBlurDescriptorSetLayout(), nullptr);

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

}

void VE::vulkanEngine::createSurface() {

	if (glfwCreateWindowSurface(instance, EngineWindow->getWindow(), nullptr, &surface) != VK_SUCCESS) {
		throw std::runtime_error("failed to create window surface!");
	}
}

VkResult VE::vulkanEngine::CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {

	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr) {
		return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
	}
	else {
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}

}

void VE::vulkanEngine::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
	
	createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = VE::Validation::debugCallback;
}

void VE::vulkanEngine::setupDebugMessenger() {

	if (!enableValidationLayers) return;

	VkDebugUtilsMessengerCreateInfoEXT createInfo;
	populateDebugMessengerCreateInfo(createInfo);

	if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
		throw std::runtime_error("failed to set up debug messenger!");
	}
}

void VE::vulkanEngine::DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
	
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func != nullptr) {
		func(instance, debugMessenger, pAllocator);
	}
}

bool VE::vulkanEngine:: checkValidationLayerSupport() {

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

std::vector<const char*> VE::vulkanEngine::GetRequiredExtensions() {

	uint32_t glfwExtensionCount = 0;

	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

	if (enableValidationLayers) {
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}
	return extensions;
}

void VE::vulkanEngine::createInstance() {

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
