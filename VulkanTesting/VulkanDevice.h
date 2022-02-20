#pragma once

#include "vulkan/vulkan.h"
#include <stdexcept>
#include <vector>
#include <optional>
#include <set>

#include "SwapChain.h"

namespace VE {

	const std::vector<const char*> deviceExtensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

	struct QueueFamilyIndices {
		std::optional<uint32_t> graphics;
		std::optional<uint32_t> present;

		bool isComplete() {
			return graphics.has_value() && present.has_value();
		}
	};

	class VulkanDevice {
	public:
		VulkanDevice(VkSurfaceKHR surface) : surface{ surface } {}
		VulkanDevice(VkInstance instance, VkSurfaceKHR surface) 
			: instance{ instance }, surface{ surface } { pickPhysicalDevice(instance); }

		void				pickPhysicalDevice(VkInstance& instance);
		void			    createDevice();

		// Support functions
		bool			    isDeviceSuitable(VkPhysicalDevice Gpu);
		bool				checkDeviceExtensionSupport(VkPhysicalDevice Gpu);
		QueueFamilyIndices	findQueueFamilies(VkPhysicalDevice Gpu);

		VkDevice			getDevice()        const { return device; }
		VkQueue				getGraphicsQueue() const { return graphics; }
		VkQueue				getPresentQueue()  const { return present; }

		VkPhysicalDevice	getPhysicalDevice() const { return Gpu; }

	private:
		VkDevice			device;
		VkPhysicalDevice	Gpu;
		VkInstance			instance;
		VkSurfaceKHR		surface;
		VkQueue				graphics;
		VkQueue				present;
	};


}