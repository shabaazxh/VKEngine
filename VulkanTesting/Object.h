#pragma once

#include <tiny_obj_loader.h>

#include <stdexcept>
#include <glm/glm.hpp>
#include "vulkan/vulkan.h"

#include <array>
#include <vector>


struct Vertex {
	glm::vec3 pos;
	glm::vec3 color;

	VkVertexInputBindingDescription getBindingDescription() {
		VkVertexInputBindingDescription bindingDescriptions{};
		bindingDescriptions.binding = 0;
		bindingDescriptions.stride = sizeof(Vertex);
		bindingDescriptions.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return bindingDescriptions;
	}

	std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions() {
		std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};

		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(Vertex, pos);

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(Vertex, color);

		return attributeDescriptions;
	}
};

struct UniformBufferObject {
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 proj;
	float time;
};

class Object {
public:
	Vertex getInstance() { return instance; }
	std::vector<Vertex> getTriangleData() { return vertices; }

	std::vector<uint32_t> getIndexData() { return indices; }

	/*std::vector<Vertex> getModelVert() { return modelVertices; }
	std::vector<uint32_t> getModelIndex() { return modelIndices; }*/

	void loadModel(std::string modelPath);
private:

	Vertex instance;
	/*std::vector<Vertex> vertices = {
	{{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}},
	{{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}},
	{{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}},
	{{-0.5f, 0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}}
	};*/

	/*std::vector<uint32_t> indices = {
		0,1,2,2,3,0
	};*/

	//std::vector<Vertex> vertices = {
	//	 { { -.5f, -.5f, -.5f }, { .9f, .9f, .9f } },
	//		{ { -.5f, .5f, .5f }, { .9f, .9f, .9f } },
	//		{ { -.5f, -.5f, .5f }, { .9f, .9f, .9f } },
	//		{ { -.5f, -.5f, -.5f }, { .9f, .9f, .9f } },
	//		{ { -.5f, .5f, -.5f }, { .9f, .9f, .9f } },
	//		{ { -.5f, .5f, .5f }, { .9f, .9f, .9f } },

	//		// right face (yellow)
	//		{ { .5f, -.5f, -.5f }, { .8f, .8f, .1f } },
	//		{ { .5f, .5f, .5f }, { .8f, .8f, .1f } },
	//		{ { .5f, -.5f, .5f }, { .8f, .8f, .1f } },
	//		{ { .5f, -.5f, -.5f }, { .8f, .8f, .1f } },
	//		{ { .5f, .5f, -.5f }, { .8f, .8f, .1f } },
	//		{ { .5f, .5f, .5f }, { .8f, .8f, .1f } },

	//		// top face (orange, remember y axis points down)
	//		{ { -.5f, -.5f, -.5f }, { .9f, .6f, .1f } },
	//		{ { .5f, -.5f, .5f }, { .9f, .6f, .1f } },
	//		{ { -.5f, -.5f, .5f }, { .9f, .6f, .1f } },
	//		{ { -.5f, -.5f, -.5f }, { .9f, .6f, .1f } },
	//		{ { .5f, -.5f, -.5f }, { .9f, .6f, .1f } },
	//		{ { .5f, -.5f, .5f }, { .9f, .6f, .1f } },

	//		// bottom face (red)
	//		{ { -.5f, .5f, -.5f }, { .8f, .1f, .1f } },
	//		{ { .5f, .5f, .5f }, { .8f, .1f, .1f } },
	//		{ { -.5f, .5f, .5f }, { .8f, .1f, .1f } },
	//		{ { -.5f, .5f, -.5f }, { .8f, .1f, .1f } },
	//		{ { .5f, .5f, -.5f }, { .8f, .1f, .1f } },
	//		{ { .5f, .5f, .5f }, { .8f, .1f, .1f } },

	//		// nose face (blue)
	//		{ { -.5f, -.5f, 0.5f }, { .1f, .1f, .8f } },
	//		{ { .5f, .5f, 0.5f }, { .1f, .1f, .8f } },
	//		{ { -.5f, .5f, 0.5f }, { .1f, .1f, .8f } },
	//		{ { -.5f, -.5f, 0.5f }, { .1f, .1f, .8f } },
	//		{ { .5f, -.5f, 0.5f }, { .1f, .1f, .8f } },
	//		{ { .5f, .5f, 0.5f }, { .1f, .1f, .8f } },

	//		// tail face (green)
	//		{ { -.5f, -.5f, -0.5f }, { .1f, .8f, .1f } },
	//		{ { .5f, .5f, -0.5f }, { .1f, .8f, .1f } },
	//		{ { -.5f, .5f, -0.5f }, { .1f, .8f, .1f } },
	//		{ { -.5f, -.5f, -0.5f }, { .1f, .8f, .1f } },
	//		{ { .5f, -.5f, -0.5f }, { .1f, .8f, .1f } },
	//		{ { .5f, .5f, -0.5f }, { .1f, .8f, .1f } },

	//};

	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;


};