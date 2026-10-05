#pragma once

#define GLEW_STATIC
#include <GL/glew.h>
#include <vector>
#include "vertex_buffer.h"
#include "Index_buffer.h"



struct Model {
	VertexBuffer vBuffer;
	IndexBuffer iBuffer;
	uint32_t numIndices;

	Model(
		Vertex* vertices,
		uint32_t numVertices,
		uint32_t* indices,
		uint32_t numIndices
	)
		: vBuffer(vertices, numVertices),
		iBuffer(indices, numIndices, sizeof(uint32_t)),
		numIndices(numIndices)
	{
	}
};

struct GameObject {
	Model* model;

	int id;

	glm::vec3 originalPosition;
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;

	glm::vec3 velocity;

	GameObject(Model* m, int i)
		: model(m),
		position(0.0f),
		rotation(0.0f),
		scale(1.0f)
	{
		id = i;
		velocity = glm::vec3(0.0f);
	}

	void reset() {
		position = originalPosition;
		velocity = glm::vec3(0.0f);
	}


};


struct VertexCombo {
    uint32_t numIndices;
    uint32_t numVertices;

    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

	VertexCombo() {
		numIndices = 0;
		numVertices = 0;
	}

	void addVertex(float x, float y, float z) {
		Vertex ver;
		
		ver.position = glm::vec3(x, y, z);
		ver.color = glm::vec4(0.3, 0.3, 0.3, 1.0);

		numVertices++;

		vertices.push_back(ver);
	}


	void addIndice(int i1, int i2, int i3) {
		numIndices += 3;

		indices.push_back(i1);
		indices.push_back(i2);
		indices.push_back(i3);
	}


};