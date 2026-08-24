#pragma once

#include "defines.h";
#include <iostream>
#include <vector>
#include <iterator>
#include <memory>

class ModelLoader {
public:
	VertexCombo loadObjFromFile(const char* path);
	std::vector<GameObject> createGameObjects();
	void addObjectToLoad(const char* path, glm::vec3 position = glm::vec3(0.0f), int id = nextID++);
	GameObject* getObjWithId(uint64_t cId, std::vector<GameObject>& objects);
protected:
	struct PathPosition {
		const char* path;
		glm::vec3 position;
		int id;
	};
	std::vector<PathPosition> pathForObjToLoad;
	std::vector<std::unique_ptr<Model>> models;
	
public:
	static uint64_t nextID;
};