
#include "vulkanengine.h"


void VE::vulkanEngine::initEngine() {

	/** Compile shaders */
	//std::system("C:/Users/Shahb/source/repos/VulkanTesting/VulkanTesting/shaders/compile.bat");

	std::cout << "Launching Engine " << std::endl;

	EngineWindow = std::make_unique<VE::Window>(1920, 1080, "title");
	EngineWindow->initWindow();
	//glfwSetFramebufferSizeCallback(EngineWindow->getWindow(), framebufferResizeCallback);
}


void VE::vulkanEngine::mainloop() {

	while (!glfwWindowShouldClose(EngineWindow->getWindow())) {
		renderer->processInput(EngineWindow->getWindow());
		glfwSetCursorPosCallback(EngineWindow->getWindow(), &MouseController::callback);
		renderer->drawFrame();
		glfwPollEvents();
	}
	vkDeviceWaitIdle(vkDevice->getDevice());

}

void VE::vulkanEngine::run() {

	initEngine();
	VulkanCore::InitVulkan();
	EngineWindow->CreateSurface(VulkanCore::GetInstance());
	Rendering();
	mainloop();
	CleanUp();
}

void VE::vulkanEngine::Rendering() {

	std::cout << "Initialising Rendering resources" << std::endl;

	VkImage DepthpositionImage;
	VkImageView DepthpositionsImageView;
	VkDeviceMemory DepthpositionsMemory;

	vkDevice = std::make_unique<VE::VulkanDevice>(VulkanCore::GetInstance(), EngineWindow->getSurface());
	vkDevice->createDevice();

	swapChain = std::make_unique<VE::SwapChain>(VulkanCore::GetInstance(), vkDevice->getDevice(), vkDevice->getPhysicalDevice(), EngineWindow->getSurface(), EngineWindow->getWindow());
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
	renderPass->createImGuiRenderPass(shadowDepth);

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
	SSAOQuadPipeline->createGraphicsPipelineOverlay("shaders/ssao/SSAOQuad.vert.spv", "shaders/ssao/another.frag.spv");

	SSAOBlurPipeline = std::make_unique<Pipeline>(vkDevice->getDevice(), renderPass->GetSSAOBlurRenderPass(), swapChain->getSwapChainExtent(), descriptorSetLayout->GetSSAOBlurDescriptorSetLayout());
	SSAOBlurPipeline->createGraphicsPipelineOverlay("shaders/ssao/SSAOBlur.vert.spv", "shaders/ssao/SSAOBlur.frag.spv");

	QuadPipeline = std::make_unique<Pipeline>(vkDevice->getDevice(), renderPass->GetQuadRenderPass(), swapChain->getSwapChainExtent(), descriptorSetLayout->GetQuadDescriptorSetLayout());
	QuadPipeline->createGraphicsPipelineOverlay("shaders/quad.vert.spv", "shaders/quad.frag.spv");

	shadowPipeline = std::make_unique<Pipeline>(vkDevice->getDevice(), renderPass->GetShadowRenderPass(), swapChain->getSwapChainExtent(), descriptorSetLayout->getDescriptorSetLayout());
	shadowPipeline->createGraphicsPipelineSingleShader("shaders/shadow/shadow.vert.spv");

	MainModelPipeline = std::make_unique<Pipeline>(vkDevice->getDevice(), renderPass->GetSceneRenderPass(), swapChain->getSwapChainExtent(), descriptorSetLayout->getDescriptorSetLayout());
	MainModelPipeline->createGraphicsPipeline("shaders/Object/vert.spv", "shaders/Object/frag.spv");

	FloorPipeline = std::make_unique<Pipeline>(vkDevice->getDevice(), renderPass->GetSceneRenderPass(), swapChain->getSwapChainExtent(), descriptorSetLayout->getDescriptorSetLayout());
	FloorPipeline->createGraphicsPipeline("shaders/Object/vert.spv", "shaders/Object/frag.spv");

	CubeMapPipeline = std::make_unique<Pipeline>(vkDevice->getDevice(), renderPass->GetSceneRenderPass(), swapChain->getSwapChainExtent(), descriptorSetLayout->getDescriptorSetLayout());
	CubeMapPipeline->createGraphicsPipeline("shaders/Object/cubemap.vert.spv", "shaders/Object/cubemap.frag.spv");

	VE::QueueFamilyIndices indices = vkDevice->findQueueFamilies(vkDevice->getPhysicalDevice());
	commandPool = std::make_unique<CommandPool>(vkDevice->getDevice(), vkDevice->getPhysicalDevice(), EngineWindow->getSurface(), indices.graphics);
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

	ImageTools::HDRImage IBLImage{};
	IBLImage.HDRImageLocations = "Textures/GravelPlaza_Env.hdr";
	ImageRes->LoadHDRImage(IBLImage);

	ImageRes->createTextureImage(FloorImageInfo);
	ImageRes->createTextureSampler();
	ImageRes->createSceneSampler();
	ImageRes->createRepeatSampler();

	Buffer positionsQuad = Buffer(vkDevice->getDevice(), vkDevice->getPhysicalDevice(), commandPool->getCommandPool(), vkDevice->getGraphicsQueue(), swapChain->getSwapChainImages(), swapChain->getSwapChainExtent());
	positionsQuad.createVertexBuffer(sizeof(QuadData.Model.GetQuadVertex()[0])* QuadData.Model.GetQuadVertex().size(), QuadData.Model.GetQuadVertex());
	positionsQuad.createIndexBuffer(sizeof(QuadData.Model.GetQuadIncies()[0])* QuadData.Model.GetQuadIncies().size(), QuadData.Model.GetQuadIncies());

	F1Car.Model.LoadModel("Models/Winter.obj"); //f1_onthefloor f1carwithcubes
	Floor.Model.LoadModel("Models/floor.obj");
	CubeMap.Model.LoadModel("Models/CubeMap.obj");

	LightBuffer = std::make_unique<Buffer>(vkDevice->getDevice(), vkDevice->getPhysicalDevice(), commandPool->getCommandPool(), vkDevice->getGraphicsQueue(), swapChain->getSwapChainImages(), swapChain->getSwapChainExtent());
	LightBuffer->createUniformBuffer(sizeof(Light));

	Light_2_Buffer = std::make_unique<Buffer>(vkDevice->getDevice(), vkDevice->getPhysicalDevice(), commandPool->getCommandPool(), vkDevice->getGraphicsQueue(), swapChain->getSwapChainImages(), swapChain->getSwapChainExtent());
	Light_2_Buffer->createUniformBuffer(sizeof(Light));
	
	MainModelBuffer = std::make_unique<Buffer>(vkDevice->getDevice(), vkDevice->getPhysicalDevice(), commandPool->getCommandPool(), vkDevice->getGraphicsQueue(), swapChain->getSwapChainImages(), swapChain->getSwapChainExtent());
	MainModelBuffer->createVertexBuffer(sizeof(F1Car.Model.getVertexData()[0]) * F1Car.Model.getVertexData().size(), F1Car.Model.getVertexData());
	MainModelBuffer->createIndexBuffer(sizeof(F1Car.Model.getIndexData()[0]) * F1Car.Model.getIndexData().size(), F1Car.Model.getIndexData());
	MainModelBuffer->createUniformBuffer(sizeof(UniformBufferObject));

	FloorObjectBuffer = std::make_unique<Buffer>(vkDevice->getDevice(), vkDevice->getPhysicalDevice(), commandPool->getCommandPool(), vkDevice->getGraphicsQueue(), swapChain->getSwapChainImages(), swapChain->getSwapChainExtent());
	FloorObjectBuffer->createVertexBuffer(sizeof(Floor.Model.getVertexData()[0]) * Floor.Model.getVertexData().size(), Floor.Model.getVertexData());
	FloorObjectBuffer->createIndexBuffer(sizeof(Floor.Model.getIndexData()[0])* Floor.Model.getIndexData().size(), Floor.Model.getIndexData());
	FloorObjectBuffer->createUniformBuffer(sizeof(UniformBufferObject));

	// CubeMap buffer 
	CubeMapBuffer = std::make_unique<Buffer>(vkDevice->getDevice(), vkDevice->getPhysicalDevice(), commandPool->getCommandPool(), vkDevice->getGraphicsQueue(), swapChain->getSwapChainImages(), swapChain->getSwapChainExtent());
	CubeMapBuffer->createVertexBuffer(sizeof(CubeMap.Model.getVertexData()[0])* CubeMap.Model.getVertexData().size(), CubeMap.Model.getVertexData());
	CubeMapBuffer->createIndexBuffer(sizeof(CubeMap.Model.getIndexData()[0])* CubeMap.Model.getIndexData().size(), CubeMap.Model.getIndexData());
	CubeMapBuffer->createUniformBuffer(sizeof(UniformBufferObject));

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
		Light_2_Buffer->getUniformBuffers(),
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
		DepthpositionsImageView,
		IBLImage.HDRImageView);

	descriptors->createDescriptorPool();
	descriptors->ImGuiCreateDescriptorPool();
	descriptors->createDescriptorSets();

	imgui = std::make_unique<ImGuiInt>(
		EngineWindow->getWindow(),
		VulkanCore::GetInstance(),
		vkDevice->getPhysicalDevice(),
		vkDevice->getDevice(),
		vkDevice->getGraphicsQueue(),
		descriptors->GetImGuiDescriptorPool(),
		commandPool->getCommandPool(),
		swapChain->getSwapChainImages(),
		renderPass->GetQuadRenderPass(),
		EngineWindow->getSurface(),
		vkDevice->getPresentQueue()
		);

	imgui->ImGuiInit();

	commandBuffers = std::make_unique<CommandBuffers>(
		vkDevice->getDevice(),
		frameBuffers->getSwapChainFramebuffers(),
		vkDevice->getGraphicsQueue(),
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
		CubeMap.Model.getIndexData(),
		FloorObjectBuffer->getIndexBuffer(),
		CubeMapBuffer->getVertexBuffer(),
		CubeMapBuffer->getIndexBuffer(),
		CubeMapPipeline->getGraphicsPipeline(),
		CubeMapPipeline->getPipelineLayout());

	commandBuffers->createCommandBuffers();

	
	frame::frameData frameInformation{};
	frameInformation.framebuffer = frameBuffers->getSwapChainFramebuffers();
	frameInformation.renderPass = renderPass->GetImGuiRenderPass();

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
		Light_2_Buffer->getUniformBuffersMemory(),
		SSAOQuadBuffer->getUniformBuffersMemory(),
		frameInformation,
		framebufferResized);

	
	renderer->createSyncObjects();

	MouseController::MouseControl = &renderer->GetCamera();

}

void VE::vulkanEngine::recreateSwapChain()
{
	ImageResource DepthFormat(vkDevice->getPhysicalDevice());
	VkFormat shadowDepth = DepthFormat.findDepthFormat();

	int width = 0, height = 0;
	glfwGetFramebufferSize(EngineWindow->getWindow(), &width, &height);
	while (width == 0 || height == 0) {
		glfwGetFramebufferSize(EngineWindow->getWindow(), &width, &height);
		glfwWaitEvents();
	}


	vkDeviceWaitIdle(vkDevice->getDevice());

	cleanupSwapChain();
	swapChain->createSwapChain();
	ImageRes->createImageResources(swapChain->getSwapChainExtent());


	/** RENDER PASSES **/
	renderPass->createSceneRenderPass(shadowDepth);
	renderPass->createShadowRenderPass(shadowDepth);
	renderPass->createQuadRenderPass();
	renderPass->createQuadRenderPass();
	renderPass->createGeometryPassRenderPass(shadowDepth);
	renderPass->createSSAORenderPass();
	renderPass->createSSAOBlurRenderPass();
	renderPass->createRenderPass_Positions(VK_FORMAT_R32G32B32A32_SFLOAT);
	renderPass->createImGuiRenderPass(shadowDepth);

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


void VE::vulkanEngine::CleanUp() {

	if (VulkanCore::GetValidationLayerStatus()) {
		DestroyDebugUtilsMessengerEXT(VulkanCore::GetInstance(), VulkanCore::GetDebugMessenger(), nullptr);
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
	vkDestroyImage(vkDevice->getDevice(), ImageRes->GetGeometryImage(), nullptr);
	vkDestroyImage(vkDevice->getDevice(), ImageRes->GetAlbedoImage(), nullptr);
	vkDestroyImage(vkDevice->getDevice(), ImageRes->GetNoiseImage(), nullptr);
	vkDestroyImage(vkDevice->getDevice(), ImageRes->GetPositionImage(), nullptr);
	vkDestroyImage(vkDevice->getDevice(), ImageRes->GetSpecImage(), nullptr);
	vkDestroyImage(vkDevice->getDevice(), ImageRes->GetSSImage(), nullptr);
	vkDestroyImage(vkDevice->getDevice(), ImageRes->GetSSAOImage(), nullptr);
	vkDestroyImage(vkDevice->getDevice(), ImageRes->GetSSAOBlurImage(), nullptr);

	// Image view
	vkDestroyImageView(vkDevice->getDevice(), ImageRes->getTextureImageView(), nullptr);
	vkDestroyImageView(vkDevice->getDevice(), ImageRes->getNormImageView(), nullptr);
	vkDestroyImageView(vkDevice->getDevice(), ImageRes->getshadowImageView(), nullptr);
	vkDestroyImageView(vkDevice->getDevice(), ImageRes->getDepthImageView(), nullptr);
	vkDestroyImageView(vkDevice->getDevice(), ImageRes->GetAlbedoImageView(), nullptr);
	vkDestroyImageView(vkDevice->getDevice(), ImageRes->GetGeometryImageView(), nullptr);
	vkDestroyImageView(vkDevice->getDevice(), ImageRes->GetgNormalsImageView(), nullptr);
	vkDestroyImageView(vkDevice->getDevice(), ImageRes->GetPositionImageView(), nullptr);
	vkDestroyImageView(vkDevice->getDevice(), ImageRes->GetNoiseImageView(), nullptr);
	vkDestroyImageView(vkDevice->getDevice(), ImageRes->GetSceneImageView(), nullptr);
	vkDestroyImageView(vkDevice->getDevice(), ImageRes->GetSpecImageView(), nullptr);
	vkDestroyImageView(vkDevice->getDevice(), ImageRes->GetSSAOImageView(), nullptr);

	// Image memory
	vkFreeMemory(vkDevice->getDevice(), ImageRes->getTextureImageMemory(), nullptr);
	vkFreeMemory(vkDevice->getDevice(), ImageRes->getDethMemory(), nullptr);
	vkFreeMemory(vkDevice->getDevice(), ImageRes->GetSceneImageMemory(), nullptr);
	vkFreeMemory(vkDevice->getDevice(), ImageRes->getShadowMemory(), nullptr);
	vkFreeMemory(vkDevice->getDevice(), ImageRes->GetAlbedoImageMemory(), nullptr);
	vkFreeMemory(vkDevice->getDevice(), ImageRes->GetGeometryImageMemory(), nullptr);
	vkFreeMemory(vkDevice->getDevice(), ImageRes->GetgNormalsImageMemory(), nullptr);
	vkFreeMemory(vkDevice->getDevice(), ImageRes->GetPositionImageMemory(), nullptr);
	vkFreeMemory(vkDevice->getDevice(), ImageRes->GetNoiseImageMemory(), nullptr);
	vkFreeMemory(vkDevice->getDevice(), ImageRes->GetSceneImageMemory(), nullptr);
	if (ImageRes->GetSpecImageeMemory() != VK_NULL_HANDLE) {
		vkFreeMemory(vkDevice->getDevice(), ImageRes->GetSpecImageeMemory(), nullptr);

	}
	vkFreeMemory(vkDevice->getDevice(), ImageRes->GetSSAOBlurImageMemory(), nullptr);

	// Main model buffer
	vkDestroyBuffer(vkDevice->getDevice(), MainModelBuffer->getVertexBuffer(), nullptr);
	vkFreeMemory(vkDevice->getDevice(), MainModelBuffer->getVertexBufferMemory(), nullptr);

	// Quad buffer
	vkDestroyBuffer(vkDevice->getDevice(), QuadBuffer->getVertexBuffer(), nullptr);
	vkFreeMemory(vkDevice->getDevice(), QuadBuffer->getVertexBufferMemory(), nullptr);
	vkDestroyBuffer(vkDevice->getDevice(), QuadBuffer->getIndexBuffer(), nullptr);
	vkFreeMemory(vkDevice->getDevice(), QuadBuffer->getIndexBufferMemory(), nullptr);

	// SSAO Quad Buffer
	vkDestroyBuffer(vkDevice->getDevice(), SSAOQuadBuffer->getVertexBuffer(), nullptr);
	vkFreeMemory(vkDevice->getDevice(), SSAOQuadBuffer->getVertexBufferMemory(), nullptr);
	vkDestroyBuffer(vkDevice->getDevice(), SSAOQuadBuffer->getIndexBuffer(), nullptr);
	vkFreeMemory(vkDevice->getDevice(), SSAOQuadBuffer->getIndexBufferMemory(), nullptr);

	vkDestroyBuffer(vkDevice->getDevice(), FloorObjectBuffer->getVertexBuffer(), nullptr);
	vkFreeMemory(vkDevice->getDevice(), FloorObjectBuffer->getVertexBufferMemory(), nullptr);

	// Destroy Light uniform buffer
	for (size_t i = 0; i < LightBuffer->getUniformBuffers().size(); i++) {
		vkDestroyBuffer(vkDevice->getDevice(), LightBuffer->getUniformBuffers()[i], nullptr);
		vkFreeMemory(vkDevice->getDevice(), LightBuffer->getUniformBuffersMemory()[i], nullptr);
	}
	
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
	vkDestroySurfaceKHR(VulkanCore::GetInstance(), EngineWindow->getSurface(), nullptr);
	//Destroy Vulkan instance
	vkDestroyInstance(VulkanCore::GetInstance(), nullptr);

	glfwDestroyWindow(EngineWindow->getWindow());
	glfwTerminate();

}

void VE::vulkanEngine::InitImGUI(GLFWwindow* window)
{
	VkDescriptorPoolSize pool_sizes[] =
	{
		{ VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
		{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
		{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
		{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
	};

	VkDescriptorPoolCreateInfo pool_info = {};
	pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
	pool_info.maxSets = 1000;
	pool_info.poolSizeCount = std::size(pool_sizes);
	pool_info.pPoolSizes = pool_sizes;

	VkDescriptorPool imguipool;
	if (vkCreateDescriptorPool(vkDevice->getDevice(), &pool_info, nullptr, &imguipool) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create descriptor pool for imgui");
	}

}

