#pragma once

#define GLEW_STATIC
#include <GL/glew.h>
#include <vector>
#include "vertex_buffer.h"
#include "Index_buffer.h"
#include <iostream>

struct Triangle3 {
	glm::vec3 a;
	glm::vec3 b;
	glm::vec3 c;
	glm::vec3 normal;
};

struct Model {
	VertexBuffer vBuffer;
	IndexBuffer iBuffer;
	uint32_t numIndices;
	std::vector <Triangle3> faces;

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
		Triangle3 triangle;
		for (int i = 2; i < numIndices; i += 3) {
			triangle.a = vertices[indices[i - 2]].position;
			triangle.b = vertices[indices[i - 1]].position;
			triangle.c = vertices[indices[i]].position;
			triangle.normal = glm::normalize(vertices[indices[i - 2]].normal + vertices[indices[i - 1]].normal + vertices[indices[i]].normal);
			faces.push_back(triangle);
		}
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

	bool influencedByWind;
	float weight; // kg

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
		ver.normal = glm::vec3(0.0f);

		numVertices++;

		vertices.push_back(ver);
	}

	void changeNormal(int vertex, glm::vec3 normal) {
		vertices[vertex].normal += normal;
	}

	std::vector<Vertex> getNormals() {
		std::vector<Vertex> vertexnormal;
		for (Vertex v : vertices) {
			Vertex w;
			w.color = glm::vec4(0.0, 0.9f, 0.9, 1.0f);
			w.position = v.position;
			vertexnormal.push_back(w);
			w.position = v.normal + v.position;
			vertexnormal.push_back(w);
		}

		for (int i = 2; i < numIndices; i += 3) {
			Vertex w;
			w.color = glm::vec4(0.9, 0.9f, 0, 1.0f);
			w.position = vertices[indices[i - 2]].position + vertices[indices[i - 1]].position + vertices[indices[i]].position;
			w.position = w.position / 3.0f;
			vertexnormal.push_back(w);
			w.position = w.position + glm::normalize(vertices[indices[i - 2]].normal + vertices[indices[i - 1]].normal + vertices[indices[i]].normal);
			vertexnormal.push_back(w);
		}

		return vertexnormal;
	}

	void normalize() {
		for (Vertex& v : vertices) {
			v.normal = glm::normalize(v.normal);
		}
	}

	void addIndice(int i1, int i2, int i3) {
		numIndices += 3;

		indices.push_back(i1);
		indices.push_back(i2);
		indices.push_back(i3);
	}


};