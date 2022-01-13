#include "ImGuiUI.h"

void ImGuiInt::ImGuiInit()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	io.IniFilename = NULL;

	ImGui_ImplGlfw_InitForVulkan(m_window, true);

	VE::VulkanDevice vkdevice(m_surface);
	VE::QueueFamilyIndices indices = vkdevice.findQueueFamilies(m_physicalDevice);

	ImGui_ImplVulkan_InitInfo init_info = {};
	init_info.Instance = m_instance;
	init_info.PhysicalDevice = m_physicalDevice;
	init_info.Device = m_device;
	init_info.Queue = m_graphics;
	init_info.QueueFamily = indices.graphics.value();
	init_info.DescriptorPool = m_ImGuiDescriptorPool;
	init_info.MinImageCount = 2;
	init_info.ImageCount = m_swapChainImages.size();
	init_info.Subpass = 0;
	init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
	ImGui_ImplVulkan_Init(&init_info, m_renderPass);

	ImGui::StyleColorsDark();

	CommandBuffers cBuffers;
	VkCommandBuffer commandBuffer = cBuffers.beginSingleTimeCommands(m_device, m_CommandPool);
	ImGui_ImplVulkan_CreateFontsTexture(commandBuffer);
	cBuffers.endSingleTimeCommnads(commandBuffer, m_device, m_CommandPool, m_graphics);
	ImGui_ImplVulkan_DestroyFontUploadObjects();

}

void ImGuiInt::ImGuiSetupWindow()
{
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

	if (showDemoWindow)
	{
		ImGui::ShowDemoWindow();
	}

	ImGui::Render();
}
