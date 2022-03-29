#pragma once

#include "vendor/imgui/imgui.h"
#include "vendor/imgui/imgui_impl_vulkan.h"
#include "vendor/imgui/imgui_impl_glfw.h"
#include "vendor/imgui/imstb_textedit.h"

#include "GLFW/glfw3.h"

#include "commandBuffers.h"
#include "VulkanDevice.h"

#include <vector>

class ImGuiInt {
public:

	ImGuiInt(GLFWwindow* window, VkInstance instance, VkPhysicalDevice physicalDevice, VkDevice device,
		VkQueue graphics, VkDescriptorPool ImGuiDescriptorPool, VkCommandPool commandPool, 
		std::vector<VkImage> swapChainImages,
		VkRenderPass renderPass, VkSurfaceKHR surface, VkQueue presentQueue)
		: m_window{ window }, m_instance{ instance }, m_physicalDevice{ physicalDevice }, m_device{ device },
		m_graphics{ graphics }, m_ImGuiDescriptorPool{ ImGuiDescriptorPool }, m_swapChainImages{ swapChainImages },
		m_renderPass{ renderPass }, m_CommandPool{ commandPool }, m_surface{ surface },
		m_presentQueue{ presentQueue } {}


	void ImGuiInit(std::unique_ptr<VE::VulkanDevice>& vkdevice);
	void ImGuiSetupWindow();


private:

	GLFWwindow* m_window;
	VkInstance m_instance;
	VkPhysicalDevice m_physicalDevice;
	VkDevice m_device;
	VkQueue m_graphics;
	VkQueue m_presentQueue;
	VkDescriptorPool m_ImGuiDescriptorPool;
	std::vector<VkImage> m_swapChainImages;
	VkRenderPass m_renderPass;
	VkCommandPool m_CommandPool;
	VkSurfaceKHR m_surface;
};