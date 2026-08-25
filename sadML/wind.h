#pragma once
#include "defines.h"
struct WindPoint {
	
	glm::vec3 position;
	glm::vec3 momentum;
	glm::vec3 initalposiiton;
	void reset() {
		position = initalposiiton;
		momentum = glm::vec3(0.0f);
	}
};

class AirHandler {
public:
	AirHandler(glm::vec3 smallPos, glm::vec3 bigPos, float spacing);
	glm::vec3 getInfluece(glm::vec3 position);
	std::vector<glm::vec3> windDirection;
	bool isInfluenced(glm::vec3 position);
	std::vector<Vertex> getWindBoundingBoxVertecies();
	std::vector<uint32_t> getWindBoundingBoxIndices();
	std::vector<WindPoint> createWindLayer(glm::vec3 sp, glm::vec3 bp, float windspacing);
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
