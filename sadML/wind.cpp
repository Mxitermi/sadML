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
				AirVertex a;
				a.position = glm::vec3(x, y, z);
				a.direction = glm::vec3(0.0f, 0.0f, -1.0f);
				windDirection.push_back(a);
			}
		}
	}
}

glm::vec3 AirHandler::getInfluece(glm::vec3 position) {

	//später blend, also prozentuale anteil vom nächsten

	float posXB;
	float posXT;
	float posYB;
	float posYT;
	float posZB;
	float posZT;


	if (position.x > bigPos.x) return windDirection[0].direction;
	if (position.y > bigPos.y) return windDirection[0].direction;
	if (position.z > bigPos.z) return windDirection[0].direction;

	int indexXB = std::floor((position.x - smallPos.x) / spacing);
	int indexXT = std::ceil((position.x - smallPos.x) / spacing);
	posXB = smallPos.x + indexXB * spacing;
	posXT = smallPos.x + indexXT * spacing;
	float deltaX = std::abs(position.x - posXB);

	int indexYB = std::floor((position.y - smallPos.y) / spacing);
	int indexYT = std::ceil((position.y - smallPos.y) / spacing);
	posYB = smallPos.y + indexYB * spacing;
	posYT = smallPos.y + indexYT * spacing;
	float deltaY = std::abs(position.y - posYB);

	int indexZB = std::floor((position.z - smallPos.z) / spacing);
	int indexZT = std::ceil((position.z - smallPos.z) / spacing);
	posZB = smallPos.z + indexZB * spacing;
	posZT = smallPos.z + indexZT * spacing;
	float deltaZ = std::abs(position.z - posZB);

	int iXB = posXB / spacing + entryPerRow / 2 - 1;
	int iXT = posXT / spacing + entryPerRow / 2 - 1;
	int iYB = posYB / spacing + entryPerRow / 2 - 1;
	int iYT = posYT / spacing + entryPerRow / 2 - 1;
	int iZB = posZB / spacing + entryPerRow / 2 - 1;
	int iZT = posZT / spacing + entryPerRow / 2 - 1;

	int indexB = iXB + entryPerRow * iYB + entryPerRow * entryPerRow * iZB;
	int indexT = iXT + entryPerRow * iYT + entryPerRow * entryPerRow * iZT;


	glm::vec3 wind;
	glm::vec3 delta = glm::vec3(deltaX, deltaY, deltaZ);
	wind = windDirection[indexB].direction * delta + windDirection[indexT].direction * (glm::vec3(spacing) -delta);
	

	return glm::normalize(wind);
}

void AirHandler::calculatWindAround(GameObject& obj)
{
	for (AirVertex& v : windDirection)
	{
		if (v.position.x > obj.position.x - obj.scale.x &&
			v.position.x < obj.position.x + obj.scale.x &&

			v.position.y > obj.position.y - obj.scale.y &&
			v.position.y < obj.position.y + obj.scale.y &&

			v.position.z > obj.position.z - obj.scale.z &&
			v.position.z < obj.position.z + obj.scale.z)
		{
			v.direction = -v.direction;
		}
	}
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

void AirHandler::plannedWinds(WindType type) {
	if (type == WindType::Default) {
		for (AirVertex &a : windDirection) {
			a.direction.x = 0.0f;
			a.direction.y = 0.0f;
			a.direction.z = -1.0f;
			a.direction = glm::normalize(a.direction);
		}
	}else if (type == WindType::Nothing) {
		for (AirVertex& a : windDirection) {
			a.direction.x = 0.0f;
			a.direction.y = 0.0f;
			a.direction.z = 0.0f;
			a.direction = glm::normalize(a.direction);
		}
	}else if (type == WindType::zVortex) {
		for (AirVertex& a : windDirection) {
			a.direction.x = -a.position.y;
			a.direction.y = a.position.x;
			a.direction.z = -1.0f;
			a.direction = glm::normalize(a.direction);
		}
	}
}

void AirHandler::setNumbersWind(glm::vec3 direction) {
	for (AirVertex& a : windDirection) {
		 a.direction = glm::normalize(direction);
	}
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
			w.velocity = glm::vec3(0.0f);

			wind.push_back(w);
		}
	}

	return wind;
}

void AirHandler::setVariablesWind(std::string equX, std::string equY, std::string equZ) {

	float dirX = 0;

	plannedWinds(WindType::Nothing);

	bool statePlus = true;
	bool stateMinus = false;
	bool stateMul = false;
	bool stateDiv = false;

	for (char c : equX)
	{
		if (c == 'x') {
			for (AirVertex& a : windDirection) {
				if (statePlus) a.direction.x += a.position.x;
				if (stateMinus) a.direction.x -= a.position.x;
				if (stateMul) a.direction.x *= a.position.x;
				if (stateDiv) {
					float pos = a.position.x;
					if (pos != 0)  a.direction.x /= pos;
				}
				a.direction = glm::normalize(a.direction);
			}
		}
		else if (c == 'y') {
			for (AirVertex& a : windDirection) {
				if (statePlus) a.direction.x += a.position.y;
				if (stateMinus) a.direction.x -= a.position.y;
				if (stateMul) a.direction.x *= a.position.y;
				if (stateDiv) {
					float pos = a.position.y;
					if (pos != 0)  a.direction.x /= pos;
				}
				a.direction = glm::normalize(a.direction);
			}
		}
		else if (c == 'z') {
			for (AirVertex& a : windDirection) {
				if (statePlus) a.direction.x += a.position.z;
				if (stateMinus) a.direction.x -= a.position.z;
				if (stateMul) a.direction.x *= a.position.z;
				if (stateDiv) {
					float pos = a.position.z;
					if (pos != 0)  a.direction.x /= pos;
				}
				a.direction = glm::normalize(a.direction);
			}
		}
		else if (c == '1') {
			for (AirVertex& a : windDirection) {
				if (statePlus) a.direction.x += 1;
				if (stateMinus) a.direction.x -= 1;
				if (stateMul) a.direction.x *= 1;
				if (stateDiv) a.direction.x /= 1;
				a.direction = glm::normalize(a.direction);
			}
		}
		else if (c == '+') {
			statePlus = true;
			stateMinus = false;
			stateMul = false;
			stateDiv = false;
		}
		else if (c == '-') {
			statePlus = false;
			stateMinus = true;
			stateMul = false;
			stateDiv = false;
		}
		else if (c == '*') {
			statePlus = false;
			stateMinus = false;
			stateMul = true;
			stateDiv = false;
		}
		else if (c == '/') {
			statePlus = false;
			stateMinus = false;
			stateMul = false;
			stateDiv = true;
		}
	}

	statePlus = true;
	stateMinus = false;
	stateMul = false;
	stateDiv = false;

	for (char c : equY)
	{
		if (c == 'x') {
			for (AirVertex& a : windDirection) {
				if (statePlus) a.direction.y += a.position.x;
				if (stateMinus) a.direction.y -= a.position.x;
				if (stateMul) a.direction.y *= a.position.x;
				if (stateDiv) {
					float pos = a.position.x;
					if (pos != 0)  a.direction.y /= pos;
				}
				a.direction = glm::normalize(a.direction);
			}
		}
		else if (c == 'y') {
			for (AirVertex& a : windDirection) {
				if (statePlus) a.direction.y += a.position.y;
				if (stateMinus) a.direction.y -= a.position.y;
				if (stateMul) a.direction.y *= a.position.y;
				if (stateDiv) {
					float pos = a.position.y;
					if (pos != 0)  a.direction.y /= pos;
				}
				a.direction = glm::normalize(a.direction);
			}
		}
		else if (c == 'z') {
			for (AirVertex& a : windDirection) {
				if (statePlus) a.direction.y += a.position.z;
				if (stateMinus) a.direction.y -= a.position.z;
				if (stateMul) a.direction.y *= a.position.z;
				if (stateDiv) {
					float pos = a.position.z;
					if (pos != 0)  a.direction.y /= pos;
				}
				a.direction = glm::normalize(a.direction);
			}
		}
		else if (c == '1') {
			for (AirVertex& a : windDirection) {
				if (statePlus) a.direction.y += 1;
				if (stateMinus) a.direction.y -= 1;
				if (stateMul) a.direction.y *= 1;
				if (stateDiv) a.direction.y /= 1;
				a.direction = glm::normalize(a.direction);
			}
		}
		else if (c == '+') {
			statePlus = true;
			stateMinus = false;
			stateMul = false;
			stateDiv = false;
		}
		else if (c == '-') {
			statePlus = false;
			stateMinus = true;
			stateMul = false;
			stateDiv = false;
		}
		else if (c == '*') {
			statePlus = false;
			stateMinus = false;
			stateMul = true;
			stateDiv = false;
		}
		else if (c == '/') {
			statePlus = false;
			stateMinus = false;
			stateMul = false;
			stateDiv = true;
		}
	}
	statePlus = true;
	stateMinus = false;
	stateMul = false;
	stateDiv = false;

	for (char c : equZ)
	{
		if (c == 'x') {
			for (AirVertex& a : windDirection) {
				if (statePlus) a.direction.z += a.position.x;
				if (stateMinus) a.direction.z -= a.position.x;
				if (stateMul) a.direction.z *= a.position.x;
				if (stateDiv) {
					float pos = a.position.x;
					if (pos != 0)  a.direction.z /= pos;
				}
				a.direction = glm::normalize(a.direction);
			}
		}
		else if (c == 'y') {
			for (AirVertex& a : windDirection) {
				if (statePlus) a.direction.z += a.position.y;
				if (stateMinus) a.direction.z -= a.position.y;
				if (stateMul) a.direction.z *= a.position.y;
				if (stateDiv) {
					float pos = a.position.y;
					if (pos != 0)  a.direction.z /= pos;
				}
				a.direction = glm::normalize(a.direction);
			}
		}
		else if (c == 'z') {
			for (AirVertex& a : windDirection) {
				if (statePlus) a.direction.z += a.position.z;
				if (stateMinus) a.direction.z -= a.position.z;
				if (stateMul) a.direction.z *= a.position.z;
				if (stateDiv) {
					float pos = a.position.z;
					if (pos != 0)  a.direction.z /= pos;
				}
				a.direction = glm::normalize(a.direction);
			}
		}
		else if (c == '1') {
			for (AirVertex& a : windDirection) {
				if (statePlus) a.direction.z += 1;
				if (stateMinus) a.direction.z -= 1;
				if (stateMul) a.direction.z *= 1;
				if (stateDiv) a.direction.z /= 1;
				a.direction = glm::normalize(a.direction);
			}
		}
		else if (c == '+') {
			statePlus = true;
			stateMinus = false;
			stateMul = false;
			stateDiv = false;
		}
		else if (c == '-') {
			statePlus = false;
			stateMinus = true;
			stateMul = false;
			stateDiv = false;
		}
		else if (c == '*') {
			statePlus = false;
			stateMinus = false;
			stateMul = true;
			stateDiv = false;
		}
		else if (c == '/') {
			statePlus = false;
			stateMinus = false;
			stateMul = false;
			stateDiv = true;
		}
	}

}

glm::vec3 AirHandler::calculateDragAcceleration(
	const glm::vec3& particleVelocity,
	const glm::vec3& windVelocity,
	float airDensity,
	float area,
	float dragCoef,
	float mass)
{
	glm::vec3 relVelocity = particleVelocity - windVelocity;
	float speed = glm::length(relVelocity);

	if (speed == 0.0f)
		return glm::vec3(0.0f);

	return -0.5f *
		airDensity *
		dragCoef *
		area *
		speed *
		relVelocity /
		mass;
}