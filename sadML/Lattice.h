#pragma once

#include "defines.h";
#include <vector>

class Lattice {
public:  
	Lattice(int size); //even
    void updateLatticeToDraw();
    void nextTimeStep(float step);
	struct LatticeCell {
		glm::vec2 directions[9]; 
		float weights[9]; 
        float distibution[9];
        float nextGendistibutions[9];
        bool isBounceBackWall = false;
        bool isInputWall = false;
        bool isExitWall = false;
        void initialize() {
            directions[0] = glm::vec2(0.0f);
            directions[1] = glm::vec2(0.0f, 1.0f);
            directions[2] = glm::vec2(1.0f, 1.0f);
            directions[3] = glm::vec2(1.0f, 0.0f);
            directions[4] = glm::vec2(1.0f, -1.0f);
            directions[5] = glm::vec2(0.0f, -1.0f);
            directions[6] = glm::vec2(-1.0f, -1.0f);
            directions[7] = glm::vec2(-1.0f, 0.0f);
            directions[8] = glm::vec2(-1.0f, 1.0f);
            
            weights[0] = 4 / 9.0f;
            weights[1] = 1 / 9.0f;
            weights[2] = 1 / 36.0f;
            weights[3] = 1 / 9.0f;
            weights[4] = 1 / 36.0f;
            weights[5] = 1 / 9.0f;
            weights[6] = 1 / 36.0f;
            weights[7] = 1 / 9.0f;
            weights[8] = 1 / 36.0f;
            
            applyInputBoundary();
           }

        void applyInputBoundary() {
            const float rho = 1.0f;
            const float ux = 0.05f;
            const float uy = 0.01f;

            for (int i = 0; i < 9; i++) {
                float cx = directions[i].x;
                float cy = directions[i].y;

                float cu = cx * ux + cy * uy;
                float u2 = ux * ux + uy * uy;

                float feq = weights[i] * rho *
                    (1.0f + 3.0f * cu
                        + 4.5f * cu * cu
                        - 1.5f * u2);
                distibution[i] = feq;
                nextGendistibutions[i] = feq;
            }
        }

        float ux() {
            float jx = 0.0f;
            for (int i = 0; i < 9; i++) {
                jx += distibution[i] * directions[i].x;
            }
            return jx / density();
        }

        float uy() {
            float jy = 0.0f;
            for (int i = 0; i < 9; i++) {
                jy += distibution[i] * directions[i].y;
            }
            return jy / density();
        }

        float density() {
            float rho = 0.0f;
            for (float f : distibution) {
                rho += f;
            }
            return rho;
        }

        void switchGens() {
            std::swap(distibution, nextGendistibutions);
        }
        };
    std::vector<Vertex> latticeToDrawVertecies;
    std::vector<uint32_t> latticeToDrawIndices;

    size_t sizeIndices = 0;
    size_t sizeVertecies = 0;
	
private:
    int latticeSize;
    std::vector<LatticeCell> latticeObject;
};