#pragma once
#include "defines.h"
#include <string>
#include <optional>
struct WindPoint {

	glm::vec3 position;
	glm::vec3 velocity;
	glm::vec3 initalposiiton;
	void reset() {
		position = initalposiiton;
		velocity = glm::vec3(0.0f);
	}

	

};

struct AirVertex {
	glm::vec3 position;
	glm::vec3 direction;
	glm::vec4 color;
	bool objface;
	float speed; //no unit "this Airvertex is speed more effected by wind"
};

enum class WindType
{
	Default,
	Custom,
	Nothing,
	zVortex
};

class AirHandler {
public:
	AirHandler(glm::vec3 smallPos, glm::vec3 bigPos, float spacing);
	void updateWindField(GameObject& obj);
	glm::vec3 getInfluece(glm::vec3 position);
	std::vector<AirVertex> windDirection;
	bool isInfluenced(glm::vec3 position);
	std::vector<Vertex> getWindBoundingBoxVertecies();
	std::vector<uint32_t> getWindBoundingBoxIndices();
	std::vector<WindPoint> createWindLayer(glm::vec3 sp, glm::vec3 bp, float windspacing);
	void plannedWinds(WindType type);
	void setVariablesWind(std::string equX, std::string equY, std::string equZ);
	void setNumbersWind(glm::vec3 direction);
	void calculatWindAround(GameObject& obj);
	glm::vec3 calculateDragAcceleration(
		const glm::vec3& particleVelocity,
		const glm::vec3& windVelocity,
		float airDensity,
		float area,
		float dragCoef,
		float mass);
private:
	float spacing;
	int entryPerRow;
	glm::vec3 smallPos;
	glm::vec3 bigPos;
	std::vector<Vertex> windBoundingBoxVertecies;
	std::vector<uint32_t> windBoundingBoxIndices;
	void addVertex(float x, float y, float z);
	void addIndice(float i, float j);
	float ray_intersects_triangle(const glm::vec3& ray_origin,const glm::vec3& ray_vector,const Triangle3& triangle, const float threshold);
};
