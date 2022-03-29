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
#include "ImGuiUI.h"
#include "vendor/imgui/imgui.h"
#include "vendor/imgui/imgui_impl_vulkan.h"
#include "vendor/imgui/imgui_impl_glfw.h"
#include "VulkanCore.h"

namespace VE {

	static std::unique_ptr<Input> CameraMovementInput;

	class vulkanEngine : public VulkanCore  {

	public:
		
		struct MouseController {

		    inline static Input* MouseControl;

			static void callback(GLFWwindow* window, double x, double y)
			{
				MouseControl->mouse_callback(window, x, y);
			}
		};

		struct Models {
			Object Model;
		};

		vulkanEngine(bool enableValidationLayers) : VulkanCore(enableValidationLayers) {}

		void run();

		bool framebufferResized = false;
	private:

		void Rendering() override;

		void initEngine();
		void mainloop();
		void cleanupSwapChain();
		void CleanUp();
		void recreateSwapChain();
		void InitImGUI(GLFWwindow* window);

		bool enableValidationLayers = false;
		std::unique_ptr<VE::Window> EngineWindow;
		Input* mouseInput;
		std::unique_ptr<VE::VulkanDevice> vkDevice;
		std::unique_ptr<VE::SwapChain> swapChain;
		std::unique_ptr<Renderer> renderer;

		std::unique_ptr<Pipeline>  SSAOQuadPipeline;
		std::unique_ptr<Pipeline> HBAOQuadPipeline;
		std::unique_ptr<Pipeline> AlchemyAOQuadPipeline;

		std::unique_ptr<Pipeline>  SSAOBlurPipeline;
		std::unique_ptr<Pipeline>  QuadPipeline;
		std::unique_ptr<Pipeline>  MainModelPipeline;
		std::unique_ptr<Pipeline>  shadowPipeline;
		std::unique_ptr<Pipeline>  FloorPipeline;
		std::unique_ptr<Pipeline>  CubeMapPipeline;
		std::unique_ptr<Pipeline>  GeometryPassPipeline;

		std::unique_ptr<RenderPass>		renderPass;
		std::unique_ptr<Framebuffers>	frameBuffers;
		std::unique_ptr<CommandPool>	commandPool;
		std::unique_ptr<CommandBuffers>	commandBuffers;

		std::unique_ptr<Buffer>	MainModelBuffer;
		std::unique_ptr<Buffer>	FloorObjectBuffer;
		std::unique_ptr<Buffer> CubeMapBuffer;
		std::unique_ptr<Buffer>	SSAOQuadBuffer;
		std::unique_ptr<Buffer>	QuadBuffer;
		std::unique_ptr<Buffer>	LightBuffer;
		std::unique_ptr<Buffer> Light_2_Buffer;

		std::unique_ptr<DescriptorSetLayout>descriptorSetLayout;
		std::unique_ptr<Descriptors> descriptors;
		std::unique_ptr<ImageResource> ImageRes;
		std::unique_ptr<ImageResource> ImageResHelper;
		std::unique_ptr<ImGuiInt> imgui;

		// Models

		Models F1Car;
		Models QuadData;
		Models Floor;
		Models CubeMap;

	};

}


