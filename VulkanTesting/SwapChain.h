#pragma once

#include <GLFW/glfw3.h>
#include "vulkan/vulkan.h"

#include <stdexcept>
#include <vector>
#include <optional>
#include <algorithm>
#include <iostream>
#include <cstdint>


#include "VulkanDevice.h"



namespace VE {

	struct SwapChainSupportDetails {
		VkSurfaceCapabilitiesKHR capabilites;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};

	class SwapChain {

	public:



		SwapChain(VkInstance instance,VkDevice device, VkPhysicalDevice Gpu, VkSurfaceKHR surface, GLFWwindow* window) 
			: instance{ instance }, device { device }, Gpu{ Gpu }, surface{ surface }, window{ window } {}

		void					 createSwapChain();
		VkSurfaceFormatKHR		 SwapChainSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
		VkPresentModeKHR		 SwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
		VkExtent2D				 SwapExtent(const VkSurfaceCapabilitiesKHR& capabilites);

		SwapChainSupportDetails  querySwapChainSupport(VkPhysicalDevice Gpu);

		VkSwapchainKHR           getSwapChain()            const { return swapChain; }
		VkFormat				 getSwapChainImageFormat() const { return swapChainImageFormat; }
		VkExtent2D				 getSwapChainExtent()      const { return swapChainExtent; }
		std::vector<VkImage>	 getSwapChainImages()      const { return swapChainImages; }
		std::vector<VkImageView> getSwapChainImageViews()  const { return swapChainImageViews; }

	private:
		VkPhysicalDevice		 Gpu;
		VkDevice				 device;
		VkSurfaceKHR			 surface;
		GLFWwindow*				 window;
		VkInstance				 instance;
		
		VkSwapchainKHR			 swapChain;
		std::vector<VkImage>	 swapChainImages;
		std::vector<VkImageView> swapChainImageViews;
		VkFormat				 swapChainImageFormat;
		VkExtent2D				 swapChainExtent;

	};
}