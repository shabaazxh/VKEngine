#include "Object.h"


void Object::loadModel(std::string modelPath) {
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warn, err;

	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, modelPath.c_str())) {
		throw std::runtime_error(warn + err);
	}

	for (const auto& shape : shapes) {
		for (const auto& index : shape.mesh.indices) {
			Vertex vertex{};

			indices.push_back(indices.size());

			vertex.pos = {
				attrib.vertices[3 * index.vertex_index + 0],
				attrib.vertices[3 * index.vertex_index + 1],
				attrib.vertices[3 * index.vertex_index + 2]
			};

			vertex.texCoord = {
				  attrib.texcoords[2 * index.texcoord_index + 0],
				  1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
			};

			vertex.Normal = {
				   attrib.normals[3 * index.normal_index + 0],
				   attrib.normals[3 * index.normal_index + 1],
				   attrib.normals[3 * index.normal_index + 2]
			};

			vertex.color = { 1.0f, 1.0f, 1.0f};
			vertices.push_back(vertex);
			
		}

	}
}