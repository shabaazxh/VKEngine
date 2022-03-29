#pragma once

#include <tiny_obj_loader.h>

#include <stdexcept>
#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>
#include "vulkan/vulkan.h"

#include <array>
#include <vector>
#include <iostream>
#include <unordered_map>


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

	bool operator==(const Vertex& other) const {
		return pos == other.pos && color == other.color && texCoord == other.texCoord;
	}
};

namespace std {
	template<> struct hash<Vertex> {
		size_t operator()(Vertex const& vertex) const {
			return ((hash<glm::vec3>()(vertex.pos) ^ (hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^ (hash<glm::vec2>()(vertex.texCoord) << 1);
		}
	};
}

struct UniformBufferObject {
	alignas(16) glm::mat4 model;
	alignas(16) glm::mat4 view;
	alignas(16) glm::mat4 proj;
	alignas(16) glm::vec4 cameraPostion;
	alignas(16) glm::vec3 albedo;
	alignas(16) glm::vec4 lightPositions[2];
	alignas(16) glm::vec4 lightColors[2];
	alignas(4) float metallic;
	alignas(4) float roughness;
	alignas(4) float ao;
	alignas(4) float time;
};

struct Light {
	alignas(16) glm::vec4 position;
	alignas(16) glm::vec4 objectColor;
	alignas(16) glm::vec4 lightColor;
	alignas(16) glm::vec4 viewPos;
	alignas(16) glm::mat4 lightSpaceMatrix;
	alignas(4) float Linear;
	alignas(4) float Quadratic;
};

struct KernelSample {
	alignas(16) glm::mat4 projection;
	alignas(16) glm::mat4 mvMatrix;
	alignas(16) glm::vec4 samples[64];
	alignas(16) glm::vec4 cameraEye;
	alignas(16) glm::vec4 cameraCenter;
	alignas(4) float z_far;
	alignas(4) float radius;
	alignas(4) float bias;
	alignas(4) float scale;
	alignas(4) float sampleDirections;
	alignas(4) float num_sample_steps;
	alignas(4) float sampling_step;
	alignas(1) bool crytekSSAO;
	alignas(4) float shadowScalar;
	alignas(4) float shadowContrast;
	alignas(4) float depthThreshold;
	alignas(4) int sampleAmount;
	alignas(4) int sampleTurns;
	alignas(4) float ambientLightLevel;
	alignas(1) bool HBAO;
	alignas(1) bool AlchemyAO;
};


class Object {
public:
	Object() = default;
	Object(std::vector<Vertex> v, std::vector<uint32_t> i) {
		this->vertices = v;
		this->indices = i;
	}

	Vertex getInstance() { return instance; }
	std::vector<Vertex> getVertexData() { return vertices; }
	std::vector<uint32_t> getIndexData() { return indices; }

	std::vector<Vertex> GetQuadVertex() { return QuadVertices; }
	std::vector<uint32_t> GetQuadIncies() { return QuadIndices; }

	void LoadModel(std::string modelPath);

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