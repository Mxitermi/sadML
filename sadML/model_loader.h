#pragma once

#include "defines.h";
#include <iostream>
#include <vector>
#include <iterator>

class ModelLoader {
public:
	VertexCombo loadObjFromFile(const char* path);
    /*
    Vertex heart[] = {
         Vertex{0.0f, -1.0f, 0.0f,
                1.0f, 0.0f, 0.0f, 1.0f},
         Vertex{0.0f, 0.5f, 0.0f,
                1.0f, 0.0f, 0.0f, 1.0f},
        Vertex{-1.0f, 0.5f, 0.0f,
                1.0f, 0.0f, 0.0f, 1.0f},
        Vertex{1.0f, 0.5f, 0.0f,
                1.0f, 0.0f, 0.0f, 1.0f},
        Vertex{-0.75f, 0.75f, 0.0f,
                1.0f, 0.0f, 0.0f, 1.0f},
        Vertex{-0.3f, 0.75f, 0.0f,
                1.0f, 0.0f, 0.0f, 1.0f},

        Vertex{0.75f, 0.75f, 0.0f,
                1.0f, 0.0f, 0.0f, 1.0f},
        Vertex{0.3f, 0.75f, 0.0f,
                1.0f, 0.0f, 0.0f, 1.0f},
    };

    uint32_t fi[] = {

        0, 1, 2,
        0, 2, 3,
        3, 4, 2,
        2, 4, 5,
        2, 5, 6,
        2, 7, 8,
        2, 8, 1,
        2, 7, 6,
        6, 10, 9,
        6, 7, 10,
        9, 11, 12,
        9, 10, 12,
        10, 13, 12

    };

    uint32_t heart2[]{
        0, 1, 2,
        0, 1, 3,
        1, 2, 4,
        1, 4, 5,
        1, 6, 7,
        1, 3, 6
    };

    uint32_t i[]{
        4, 2, 0,
        2, 7, 3,
        6, 5, 7,
        1, 7, 5,
        0, 3, 1,
        4, 1, 5
    };
*/
};