#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <vulkan/vulkan.h>


#include "SwapChain.h"
#include "Window.h"

namespace VE {

	class VulkanCore {

	public:
		VulkanCore(bool ValidationLayers) : validation_layers{ ValidationLayers } {}

		virtual void InitVulkan();
		void CreateInstance();

		virtual void CleanUp() = 0;
		virtual void Rendering() {}


		std::vector<const char*> GetRequiredExtensions();
		bool CheckValidationLayerSupport();
		VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
		void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);
		void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
		void SetupDebugMessenger();

		bool GetValidationLayerStatus() const { return validation_layers; }
		VkInstance GetInstance() const { return instance; }
		VkDebugUtilsMessengerEXT GetDebugMessenger() const { return debugMessenger; }

	private:
		bool validation_layers;
		VkInstance instance;

		std::unique_ptr<VE::VulkanDevice> device;
		std::unique_ptr<VE::SwapChain> swapChain;

		const std::vector<const char*> validationLayers = {
			"VK_LAYER_KHRONOS_validation"
		};

		VkDebugUtilsMessengerEXT debugMessenger;
	};


}