#pragma once

#include "defines.h";
#include <iostream>
#include <vector>
#include <iterator>
#include <memory>

class ModelLoader {
public:
	VertexCombo loadObjFromFile(const char* path);
	std::vector<GameObject> initializeGameobjects();
	void addObjectToLoad(const char* path,std::string name, glm::vec3 position = glm::vec3(0.0f), bool loadOnStart = true, int id = -1);
	GameObject* getObjWithId(uint64_t cId, std::vector<GameObject>& objects);
	GameObject addGameObject(std::string name, glm::vec3 position);
protected:
	struct PathPosition {
		const char* path;
		glm::vec3 position;
		std::string name;
		int id;
		int pos;
		bool loadOnStart;
	};
	std::vector<PathPosition> pathForObjToLoad;
	std::vector<std::unique_ptr<Model>> models;
	int counter = 0;
	uint64_t nextID = 1000;
};