#pragma once

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
#include "Window.h"
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

#include "VulkanDevice.h"
#include "SwapChain.h"
#include "VulkanTools.h"

namespace VE {
	class vulkanEngine {

		const std::vector<const char*> validationLayers = {
			"VK_LAYER_KHRONOS_validation"
		};

	public:

		struct Models {
			Object Model;
		};

		//Graphics pipelines here 
		struct {
			Pipeline pipeline;
			Pipeline x;
		} pipelines;

		vulkanEngine(bool enableValidationLayers) : enableValidationLayers{ enableValidationLayers } {}

		void							      run();

		VkInstance						      getInstance() const { return instance; }

	private:

		void							      initEngine();
		void							      initVulkan();
		void							      mainloop();
		void							      cleanup();

		void							      createInstance();
		void							      createSurface();
		void							      Rendering();

		// Validation Layers
		bool							      checkValidationLayerSupport();
		void							      setupDebugMessenger();
		void							      populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
		void							      DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);
		VkResult						      CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
		std::vector<const char*>		      GetRequiredExtensions();

		VkInstance						      instance;
		VkDevice						      device;
		VkSurfaceKHR				          surface;
		VkDebugUtilsMessengerEXT		      debugMessenger;
		bool						          enableValidationLayers = false;
		std::unique_ptr<VE::Window>           EngineWindow;
		std::unique_ptr<Input>			      CameraMovementInput;
		std::unique_ptr<VE::VulkanDevice>     vkDevice;
		std::unique_ptr<VE::SwapChain>        swapChain;
		std::unique_ptr<Renderer>		      renderer;

		std::unique_ptr<Pipeline>		      SSAOQuadPipeline;
		std::unique_ptr<Pipeline>		      SSAOBlurPipeline;
		std::unique_ptr<Pipeline>		      QuadPipeline;
		//std::unique_ptr<Pipeline>		      pipeline;
		std::unique_ptr<Pipeline>		      shadowPipeline;
		std::unique_ptr<Pipeline>		      FloorPipeline;
		std::unique_ptr<Pipeline>		      GeometryPassPipeline;

		std::unique_ptr<RenderPass>		      renderPass;
		std::unique_ptr<Framebuffers>	      frameBuffers;
		std::unique_ptr<CommandPool>	      commandPool;
		std::unique_ptr<CommandBuffers>	      commandBuffers;

		std::unique_ptr<Buffer>				  buffers;
		std::unique_ptr<Buffer>				  FloorObjectBuffer;
		std::unique_ptr<Buffer>				  SSAOQuadBuffer;
		std::unique_ptr<Buffer>				  QuadBuffer;
		std::unique_ptr<Buffer>				  LightBuffer;

		std::unique_ptr<DescriptorSetLayout>  descriptorSetLayout;
		std::unique_ptr<Descriptors>		  descriptors;
		std::unique_ptr<ImageResource>		  ImageRes;
		std::unique_ptr<ImageResource>		  ImageResHelper;

		// Models

		Models F1Car;
		Models QuadData;
		Models Floor;

	};
}