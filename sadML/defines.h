#pragma once

#define GLEW_STATIC
#include <GL/glew.h>
#include <vector>

struct Vertex {
	float x;
	float y;
	float z;

	float r;
	float g;
	float b;
	float a;
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
		ver.x = x;
		ver.y = y;
		ver.z = z;

		ver.r = 0.4f;
		ver.g = 0.4f;
		ver.b = 0.4f;
		ver.a = 1.0f;

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