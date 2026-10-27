#include "Lattice.h"
#include <algorithm>

Lattice::Lattice(int size) {
	for (int i = 0; i < size * size; i++)
	{
		LatticeCell cell;
		cell.initialize();
		if (i < size || i >= size * size - size ) {
			cell.isBounceBackWall = true;
		}else
		if (i % size == 0) {
			cell.isInputWall = true;
		}else if ((i+1) % size == 0) {
			cell.isExitWall = true;
		}
		else {
			
			int x = i % size;
			int y = (i - x) / size;

			int mX = 50;
			int mY = 50;
			int r = 10;

			int dx = mX - x;
			int dy = mY - y;

			if (dx * dx + dy * dy <= r * r) {
				cell.isBounceBackWall = true;
			}
		
		}
		
		latticeObject.push_back(cell);
	}
	latticeSize = size;
}

void Lattice::nextTimeStep(float step) {

	for (int i = 0; i < latticeObject.size(); i++)
	{
		float cnt = 0.0;
		LatticeCell& cell = latticeObject[i];
		if (cell.isBounceBackWall) {
			int x = i % latticeSize;
			int y = (i - x) / latticeSize;
			cell.isBounceBackWall = false;
			for (int j = 1; j < 9; j++) {
				int targetX = x + cell.directions[j].x;
				int targetY = y + cell.directions[j].y;
				if (targetY > 0 && targetX > 0 && targetY < latticeSize && targetX < latticeSize) { //Problem mit der Dichte und Massenerhaltung
					if (!latticeObject[targetX + targetY * latticeSize].isBounceBackWall) {
						for (int k = 0; k < 9; k++) {
							latticeObject[x + y * latticeSize].distibution[k] += latticeObject[targetX + targetY * latticeSize].distibution[k];
						}
						cnt++;
					}
				}
			}
			for (int k = 0; k < 9; k++)
				latticeObject[x + y * latticeSize].distibution[k] /= cnt;
		}
		if (i < latticeSize || i >= latticeSize * latticeSize - latticeSize) {
			cell.isBounceBackWall = true;
		}
			else {

				int x = i % latticeSize;
				int y = (i - x) / latticeSize;

				int mX = 50;
				int mY = 50 + std::sin(step) * 20;
				int r = 10;

				int dx = mX - x;
				int dy = mY - y;

				if (dx * dx + dy * dy <= r * r) {
					cell.isBounceBackWall = true;
					for (int i = 0; i < 9; i++) cell.distibution[i] = 0.0f;
				}

			}
	}

	int oppositeDirection[] = {
	0, // 0: Ruhe
	5, // 1: rechts  -> links
	6, // 2: oben    -> unten
	7, // 3: links   -> rechts
	8, // 4: unten   -> oben
	1, // 5: rechts oben -> links unten
	2, // 6: links oben  -> rechts unten
	3, // 7: links unten -> rechts oben
	4  // 8: rechts unten -> links oben
	};

	for (int j = 0; j < latticeObject.size(); j++) {

		LatticeCell cell = latticeObject[j];

		if (cell.isBounceBackWall) {
			continue;
		}

		float rho = 0;
		float jx = 0;
		float jy = 0;
		float csSQ = 1.0f / 3.0f;
		for (int i = 0; i < 9; i++) {
			rho += cell.distibution[i];
			jx += cell.distibution[i] * cell.directions[i].x;
			jy += cell.distibution[i] * cell.directions[i].y;
		}
		glm::vec2 u = glm::vec2(jx / rho, jy / rho);
		float eq = 0;
		float fistar = 0;

		float viskus = 1.0f / 6.0f; //viskusität in Gitereinheiten

		float tau = viskus * 3.0f + 1.0f/2.0f; 

		for (int i = 0; i < 9; i++) {
			
			eq = cell.weights[i] * rho * (1+glm::dot(cell.directions[i], u) / csSQ + (glm::dot(cell.directions[i], u) * glm::dot(cell.directions[i], u))/(2*csSQ*csSQ)- glm::dot(u, u)/(2*csSQ));
			fistar = cell.distibution[i] - (1.0f / tau) * (cell.distibution[i] - eq);
			int x = j % latticeSize;
			int y = (j - x) / latticeSize;

			int targetX = x + cell.directions[i].x;
			int targetY = y + cell.directions[i].y;

			int k = i;

			if (cell.isInputWall) {
				if(i >= 6)  continue;
			}

			if (cell.isExitWall) {
					if (i >= 2 && i <= 4) continue;
			}

			if (latticeObject[targetX + targetY * latticeSize].isBounceBackWall)
			{
				targetX = x;
				targetY = y;
				k = oppositeDirection[i];
			}


			latticeObject[targetX + targetY * latticeSize].nextGendistibutions[k] = fistar;

		}
	}

	

	for (int i = 0; i < latticeObject.size(); ++i) {
		LatticeCell& cell = latticeObject[i];

		if (cell.isExitWall) {
			for (int j = 0; j < 9; ++j) {
				cell.nextGendistibutions[j] = latticeObject[i - 1].nextGendistibutions[j];
			}
		}
		else if (cell.isInputWall) {
			cell.applyInputBoundary();
		}
	}

	for (int i = 0; i < latticeObject.size(); i++) {
		latticeObject[i].switchGens();
	}

	updateLatticeToDraw();
}

void Lattice::updateLatticeToDraw()
{

	const float drawScale = 1.0f;

    latticeToDrawVertecies.clear();
    latticeToDrawIndices.clear();

	for (int i = 0; i < latticeSize; i++) {
		for (int j = 0; j < latticeSize; j++) {
			LatticeCell& cell = latticeObject[i + latticeSize * j];
			float ux = cell.ux();
			float uy = cell.uy();
			float g = std::sqrt(std::pow(ux, 2) + std::pow(uy, 2));
			g *= 40;
			float sigmoid = 1 / (1 + std::exp(-g));
			
			glm::vec4 color = glm::vec4(sigmoid, sigmoid, sigmoid, 1.0f);

			if(cell.isBounceBackWall) color = glm::vec4(0.0f, 0.8f, 0.0f, 1.0f);
			//if (cell.isInputWall) color = glm::vec4(0.0f, 0.3f, 0.8f, 1.0f);
			//if (cell.isExitWall) color = glm::vec4(8.0f, 0.3f, 0.0f, 1.0f);

			Vertex v;
			v.position = glm::vec3( -1 * (latticeSize / 2) + i * drawScale, -(latticeSize / 2) + j* drawScale, 0.0f);
			v.color = color;
			Vertex w;
			w.position = glm::vec3(-(latticeSize / 2) + (i + 1)* drawScale, -(latticeSize / 2) + j * drawScale, 0.0f);
			w.color = color;
			Vertex u;
			u.position = glm::vec3(-(latticeSize / 2) + i * drawScale, -(latticeSize / 2) + (j+1) * drawScale, 0.0f);
			u.color = color;
			Vertex o;
			o.position = glm::vec3(-(latticeSize / 2) + (i+1) * drawScale, -(latticeSize / 2) + (j+1)* drawScale, 0.0f);
			o.color = color;

			latticeToDrawVertecies.push_back(v);
			latticeToDrawVertecies.push_back(w);
			latticeToDrawVertecies.push_back(u);
			latticeToDrawVertecies.push_back(o);

			latticeToDrawIndices.push_back(latticeToDrawVertecies.size() - 4);
			latticeToDrawIndices.push_back(latticeToDrawVertecies.size() - 3);
			latticeToDrawIndices.push_back(latticeToDrawVertecies.size() - 2);
			latticeToDrawIndices.push_back(latticeToDrawVertecies.size() - 3);
			latticeToDrawIndices.push_back(latticeToDrawVertecies.size() - 2);
			latticeToDrawIndices.push_back(latticeToDrawVertecies.size() - 1);
		}
	}

    sizeVertecies = latticeToDrawVertecies.size();
    sizeIndices = latticeToDrawIndices.size();
}