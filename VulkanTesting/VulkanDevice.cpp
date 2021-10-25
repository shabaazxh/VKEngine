#include "VulkanDevice.h"

bool VE::VulkanDevice::isDeviceSuitable(VkPhysicalDevice Gpu)
{
	QueueFamilyIndices indices = findQueueFamilies(Gpu);

	bool extensionSupport = checkDeviceExtensionSupport(Gpu);

	SwapChain swapChainQuery(instance, device, Gpu, surface, nullptr);
	
	bool swapChainAdequate = false;
	if (extensionSupport) {
		VE::SwapChainSupportDetails swapChainSupport = swapChainQuery.querySwapChainSupport(Gpu);
		swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
	}

	VkPhysicalDeviceFeatures SupportedFeatures;
	vkGetPhysicalDeviceFeatures(Gpu, &SupportedFeatures);

	return indices.isComplete() && extensionSupport && swapChainAdequate && 
		SupportedFeatures.samplerAnisotropy;
}

bool VE::VulkanDevice::checkDeviceExtensionSupport(VkPhysicalDevice Gpu)
{
	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties(Gpu, nullptr, &extensionCount, nullptr);

	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(Gpu, nullptr, &extensionCount, availableExtensions.data());

	std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

	for (const auto& extension : availableExtensions) {
		requiredExtensions.erase(extension.extensionName);
	}

	return requiredExtensions.empty();
}

void VE::VulkanDevice::pickPhysicalDevice(VkInstance& instance)
{
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

	if (deviceCount == 0) {
		throw std::runtime_error("failed to find GPUs that support Vulkan");
	}

	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

	for (const auto& device : devices) {
		if (isDeviceSuitable(device)) {
			Gpu = device;
			break;
		}
	}
	if (Gpu == VK_NULL_HANDLE) {
		throw std::runtime_error("failed to find a suitable GPU!");
	}
}

VE::QueueFamilyIndices VE::VulkanDevice::findQueueFamilies(VkPhysicalDevice Gpu)
{
	QueueFamilyIndices indices;

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(Gpu, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(Gpu, &queueFamilyCount, queueFamilies.data());

	int i = 0;
	for (const auto& queueFamily : queueFamilies) {
		if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			indices.graphics = i;
		}
		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(Gpu, i, surface, &presentSupport);
		if (presentSupport) {
			indices.present = i;
		}

		if (indices.isComplete()) {
			break;
		}
		i++;
	}
	return indices;
}


void VE::VulkanDevice::createDevice()
{
	QueueFamilyIndices indices = findQueueFamilies(Gpu);

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<uint32_t> uniqueQueueFamilies = { indices.graphics.value(), indices.present.value() };


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

	if (vkCreateDevice(Gpu, &createInfo, nullptr, &device) != VK_SUCCESS) {
		throw std::runtime_error("failed to create logical device");
	}

	vkGetDeviceQueue(device, indices.graphics.value(), 0, &graphics);
	vkGetDeviceQueue(device, indices.present.value(), 0, &present);
}
