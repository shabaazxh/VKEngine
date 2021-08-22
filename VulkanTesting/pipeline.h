#pragma once

#include <vector>
#include "shader.h"
#include <vulkan/vulkan.h>
#include "Object.h"
#include <memory>
#include <GLFW/glfw3.h>

class Pipeline {
	public:
		Pipeline(VkDevice device, VkRenderPass renderPass,VkExtent2D swapChainExtent, VkDescriptorSetLayout descriptorSetLayout);
		
		void createGraphicsPipeline(std::string vertFile, std::string fragFile);
		void createGraphicsPipelineOverlay(std::string vertFile, std::string fragFile);
		void createGraphicsPipelineSingleShader(std::string vertFile);
		void createGeometryPassGraphicsPipeline(std::string vertFile, std::string fragFile);


		VkPipelineLayout getPipelineLayout() { return pipelineLayout; }
		VkPipeline getGraphicsPipeline() { return graphicsPipeline; }

		void createComputePipeline(std::string computeFile);
		VkPipeline getComputePipeline() { return computePipeline; }

	private:
		VkDevice device;
		VkExtent2D swapChainExtent;
		VkPipelineLayout pipelineLayout;
		VkRenderPass renderPass;
		VkPipeline graphicsPipeline;
		VkPipeline computePipeline;
		VkDescriptorSetLayout descriptorSetLayout;
};