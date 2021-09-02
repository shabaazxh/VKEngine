#pragma once

#include <tiny_obj_loader.h>

#include <stdexcept>
#include <glm/glm.hpp>
#include "vulkan/vulkan.h"

#include <array>
#include <vector>
#include <iostream>


struct Vertex {
	glm::vec3 pos;
	glm::vec3 color;
	glm::vec3 Normal;
	glm::vec2 texCoord;

	VkVertexInputBindingDescription getBindingDescription() {

		VkVertexInputBindingDescription bindingDescriptions{};
		bindingDescriptions.binding = 0;
		bindingDescriptions.stride = sizeof(Vertex);
		bindingDescriptions.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return bindingDescriptions;
	}

	std::array<VkVertexInputAttributeDescription, 4> getAttributeDescriptions() {
		std::array<VkVertexInputAttributeDescription, 4> attributeDescriptions{};

		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(Vertex, pos);

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(Vertex, color);

		attributeDescriptions[2].binding = 0;
		attributeDescriptions[2].location = 2;
		attributeDescriptions[2].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[2].offset = offsetof(Vertex, Normal);

		attributeDescriptions[3].binding = 0;
		attributeDescriptions[3].location = 3;
		attributeDescriptions[3].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[3].offset = offsetof(Vertex, texCoord);

		return attributeDescriptions;
	}
};

struct UniformBufferObject {
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 proj;
	float time;
};

struct Light {
	alignas(16) glm::vec4 position;
	alignas(16) glm::vec4 objectColor;
	alignas(16) glm::vec4 lightColor;
	alignas(16) glm::vec4 viewPos;
	alignas(16) glm::mat4 lightSpaceMatrix;
	alignas(16) bool invertedNormals;
	alignas(16) float Linear;
	alignas(16) float Quadratic;

};

	struct KernelSample {
		alignas(16) glm::vec4 samples[64];
		alignas(16) glm::mat4 projection;
	};

class Object {
public:
	Vertex getInstance() { return instance; }
	std::vector<Vertex> getVertexData() { return vertices; }
	std::vector<uint32_t> getIndexData() { return indices; }

	std::vector<Vertex> GetQuadVertex() { return QuadVertices; }
	std::vector<uint32_t> GetQuadIncies() { return QuadIndices; }

	void loadModel(std::string modelPath);
private:

	const std::vector<Vertex> QuadVertices = {
		 {{-1.0f,-1.0f, 0.0f}, {1.0f, 0.0f, 0.0f},  {0.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
		 {{1.0f, -1.0f, 0.0f}, {0.0f, 1.0f, 0.0f},  {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
		 {{1.0f,  1.0f, 0.0f}, {0.0f, 0.0f, 1.0f},  {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},
		 {{-1.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 1.0f},  {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f}}
	};

	const std::vector<uint32_t> QuadIndices = {
		0,1,2,2,3,0
	};

	Vertex instance;

	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
};