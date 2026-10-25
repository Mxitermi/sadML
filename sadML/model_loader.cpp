#include "model_loader.h"
#include "defines.h"
#include <fstream>
#include <vector>
#include <iterator>
#include <string>
#include <sstream>

std::vector<GameObject> ModelLoader::initializeGameobjects() {

    std::vector<GameObject> objects;

    for (PathPosition& current : pathForObjToLoad)
    {
        VertexCombo cube = loadObjFromFile(current.path);
        models.push_back(
            std::make_unique<Model>(
                cube.vertices.data(),
                cube.numVertices,
                cube.indices.data(),
                cube.numIndices
            )
        );
        

        if (current.loadOnStart) {
            current.pos = counter;
            counter++;
            Model* model = models.back().get();
            GameObject gameObject(model, current.id);
            gameObject.position = current.position;
            gameObject.originalPosition = current.position;
            gameObject.influencedByWind = false;
            gameObject.weight = 1.0f;

            objects.push_back(gameObject);
        }
    }

    return objects;


    
}

GameObject ModelLoader::addGameObject(std::string name, glm::vec3 position) {
    for (PathPosition& pp : pathForObjToLoad) {
        if (pp.name == name) {
            Model* model = models[pp.pos].get();
            nextID++;
            GameObject gameObject(model, nextID);
            gameObject.position = position;
            gameObject.scale = glm::vec3(1.0f);
            gameObject.originalPosition = position;

            return gameObject;
        }
    }
}

void ModelLoader::addObjectToLoad(const char* path, std::string name, glm::vec3 position, bool loadOnStart, int id) {
    if(id == -1) nextID++;
    pathForObjToLoad.push_back(PathPosition(path, position, name, nextID, 0,loadOnStart));

}

VertexCombo ModelLoader::loadObjFromFile(const char* path) {
     
    std::ifstream infile(path);

    std::string line;
    std::string type;
    float x, y, z;

    char b;
    int i1, i2, i3, t;
    int n1, n2, n3;

    VertexCombo combo;
    std::vector<glm::vec3> normals;

    while (std::getline(infile, line)) {

        std::stringstream ss(line);
        ss >> type;
        if (type == "v") {
            ss >> x >> y >> z;

            combo.addVertex(x, y, z);
        }

        if (type == "vn") {
            ss >> x >> y >> z;
 
            normals.push_back(glm::vec3(x, y, z));
        }

        if (type == "f") {
            ss >> i1 >> b >> t >> b >> n1 >> i2 >> b >> t >> b >> n2 >> i3 >> b >> t >> b >> n3;
            combo.addIndice(i1 - 1, i2 - 1, i3 - 1);
            combo.changeNormal(i1 - 1, normals[n1 - 1]);
            combo.changeNormal(i2 - 1, normals[n2 - 1]);
            combo.changeNormal(i3 - 1, normals[n3 - 1]);
        }
    }
    combo.normalize();

    return combo;
}

GameObject* ModelLoader::getObjWithId(uint64_t cId, std::vector<GameObject>& objects) {
    for (GameObject& obj : objects) {
        if (obj.id == cId) return &obj;
    }
    return nullptr;
}
