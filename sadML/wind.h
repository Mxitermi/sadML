#pragma once
#include "defines.h"


class AirHandler {
public:
	AirHandler(glm::vec3 smallPos, glm::vec3 bigPos, float spacing);
	glm::vec3 getInfluece(glm::vec3 position);
	std::vector<glm::vec3> windDirection;
	bool isInfluenced(glm::vec3 position);
	std::vector<Vertex> getWindBoundingBoxVertecies();
	std::vector<uint32_t> getWindBoundingBoxIndices();
private:
	float spacing;
	int entryPerRow;
	glm::vec3 smallPos;
	glm::vec3 bigPos;
	std::vector<Vertex> windBoundingBoxVertecies;
	std::vector<uint32_t> windBoundingBoxIndices;
	void addVertex(float x, float y, float z);
	void addIndice(float i, float j);
};
