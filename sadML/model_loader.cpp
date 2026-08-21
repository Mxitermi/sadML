#include "model_loader.h";
#include "defines.h";
#include <fstream>
#include <vector>
#include <iterator>
#include <string>
#include <sstream>


VertexCombo ModelLoader::loadObjFromFile(const char* path) {
    
    std::ifstream infile(path);

    std::string line;
    char type;
    float x, y, z;

    char b;
    int i1, i2, i3, t;

    VertexCombo combo;

    while (std::getline(infile, line)) {

        std::stringstream ss(line);
        ss >> type;
        if (type == 'v') {
            ss >> x >> y >> z;

            combo.addVertex(x, y, z);
        }
            
        if (type == 'f') {
            ss >> i1 >> b >> t >> b >> t >> i2 >> b >> t >> b >> t >> i3 >> b >> t >> b >> t;
            combo.addIndice(i1 - 1, i2 - 1, i3 - 1);
        }
    }
    
    
    
    return combo;
}

