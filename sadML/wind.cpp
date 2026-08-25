#include "wind.h"
#include <iostream>
#include <cmath>



AirHandler::AirHandler(glm::vec3 sp, glm::vec3 bp, float s) {

	smallPos = sp;
	bigPos = bp;
	spacing = s;
	
	addVertex(sp.x, sp.y, sp.z);
	addVertex(sp.x, sp.y, bp.z);
	addVertex(sp.x, bp.y, bp.z);
	addVertex(sp.x, bp.y, sp.z);
	addVertex(bp.x, bp.y, sp.z);
	addVertex(bp.x, sp.y, sp.z);
	addVertex(bp.x, sp.y, bp.z);
	addVertex(bp.x, bp.y, bp.z);

	addIndice(0, 5);
	addIndice(0, 1);
	addIndice(0, 3);
	addIndice(2, 1);
	addIndice(2, 7);
	addIndice(2, 3);
	addIndice(6, 5);
	addIndice(6, 1);
	addIndice(6, 7);
	addIndice(4, 5);
	addIndice(4, 7);
	addIndice(4, 3);

	for (float z = smallPos.z; z <= bigPos.z + spacing; z += spacing) {
		entryPerRow++;
		for (float y = smallPos.y; y <= bigPos.y + spacing; y += spacing) {
			for (float x = smallPos.x; x <= bigPos.x + spacing; x += spacing) {
				windDirection.push_back(glm::vec3(z-x+y, x, y +y * x));
			}
		}
	}
}

glm::vec3 AirHandler::getInfluece(glm::vec3 position) {

	//später blend, also prozentuale anteil vom nächsten

	float posX;
	float posY;
	float posZ;

	if (position.x > bigPos.x) return windDirection[0];
	if (position.y > bigPos.y) return windDirection[0];
	if (position.z > bigPos.z) return windDirection[0];

	int indexXF = std::round((position.x - smallPos.x) / spacing);
	posX = smallPos.x + indexXF * spacing;

	int indexYF = std::round((position.y - smallPos.y) / spacing);
	posY = smallPos.y + indexYF * spacing;

	int indexZF = std::round((position.z - smallPos.z) / spacing);
	posZ = smallPos.z + indexZF * spacing;

	int iX = posX / spacing + entryPerRow / 2 - 1;
	int iY = posY / spacing + entryPerRow / 2 - 1;
	int iZ = posZ / spacing + entryPerRow / 2 - 1;

	int index = iX + entryPerRow * iY + entryPerRow * entryPerRow * iZ;

	//std::cout << "Should: " << posX << " " << posY << " " << posZ << std::endl;

	return windDirection[index];
}

bool AirHandler::isInfluenced(glm::vec3 position) {
	if (position.x > bigPos.x) return false;
	if (position.y > bigPos.y) return false;
	if (position.z > bigPos.z) return false;

	if (position.x < smallPos.x) return false;
	if (position.y < smallPos.y) return false;
	if (position.z < smallPos.z) return false;
	return true;
}

std::vector<Vertex> AirHandler::getWindBoundingBoxVertecies() {
	return windBoundingBoxVertecies;
}

std::vector<uint32_t> AirHandler::getWindBoundingBoxIndices() {
	return windBoundingBoxIndices;
}

void AirHandler::addVertex(float x, float y, float z)
{
	Vertex v;
	v.position = glm::vec3(x, y, z);
	v.color = glm::vec4(1.0f);
	windBoundingBoxVertecies.push_back(v);
};

void AirHandler::addIndice(float i, float j) {
	windBoundingBoxIndices.push_back(i);
	windBoundingBoxIndices.push_back(j);
}

std::vector<WindPoint> AirHandler::createWindLayer(glm::vec3 sp, glm::vec3 bp, float windspacing)
{
	std::vector<WindPoint> wind;

	for (float y = sp.y; y <= bp.y; y += windspacing)
	{
		for (float x = sp.x; x <= bp.x; x += windspacing)
		{
			WindPoint w;
			w.position = glm::vec3(x, y, bp.z);
			w.initalposiiton = w.position;
			w.momentum = glm::vec3(0.0f);

			wind.push_back(w);
		}
	}

	return wind;
}