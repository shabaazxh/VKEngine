#pragma once
#include "vulkan/vulkan.h"
#include <stdexcept>
#include <vector>
#include <optional>
#include <set>
#include <iostream>

namespace VE {

	
	namespace Tools {

				
	}

	namespace Validation {

		static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
			VkDebugUtilsMessageTypeFlagsEXT messageType,
			const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
			void* pUserData) {

			std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

			return VK_FALSE;
		}
	}

} // namespace VE