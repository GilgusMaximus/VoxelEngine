#include "DiamondSquare.h"
#include <ctime>
#include <cmath>
#include <algorithm>
#include <iostream>

#define IDX(x, y, w) ((x) + (y) * (w))

DiamondSquare::DiamondSquare() {
	
}



DiamondSquare::~DiamondSquare() {
}

float DiamondSquare::createRandomFloat(std::default_random_engine &rndEngine, float midPoint, float roughness) {

	std::normal_distribution<float> distribution(midPoint, roughness);
	float randomD = distribution(rndEngine);
	while (randomD > roughness || randomD < -roughness)
		randomD = distribution(rndEngine);
	return randomD;
}

float DiamondSquare::diamond(int x, int y, int s, float* field, int fieldWidth) {
	float cornerSum = 0.0f;
	cornerSum += field[IDX(x - s, y + s, fieldWidth)];
	cornerSum += field[IDX(x - s, y - s, fieldWidth)];
	cornerSum += field[IDX(x + s, y + s, fieldWidth)];
	cornerSum += field[IDX(x + s, y - s, fieldWidth)];
	float midValue = cornerSum / 4.0f;
	return midValue;
}

float DiamondSquare::square(int x, int y, int s, float* field, int fieldWidth) {
	int counter = 0;
	float cornerSum = 0.0f;
	if (x - s >= 0 && x - s < fieldWidth) {
		cornerSum += field[IDX(x - s, y, fieldWidth)];
		counter++;
	}
	if (y - s >= 0 && y - s < fieldWidth) {
		cornerSum += field[IDX(x, y - s, fieldWidth)];
		counter++;
	}
	if (x + s >= 0 && x + s < fieldWidth) {
		cornerSum += field[IDX(x + s, y, fieldWidth)];
		counter++;
	}
	if (y + s >= 0 && y + s < fieldWidth) {
		cornerSum += field[IDX(x, y + s, fieldWidth)];
		counter++;
	}
	float midValue = cornerSum / counter;
	return midValue;
}

std::vector<float> DiamondSquare::createHeightfield(int resolution, float roughness, std::default_random_engine& rndEngine) {

	int heightOrLength = resolution + 1;

	int fieldSize = (heightOrLength)*(heightOrLength);
	//std::vector<float> heightField(fieldSize);
	float* heightField = new float[fieldSize];

	

	float sigma = roughness;

	//Initialization of corner values
	heightField[IDX(0, 0, heightOrLength)] = createRandomFloat(rndEngine, 0.f, 1.f);
	//std::cout << heightField[IDX(0, 0, heightOrLength)] << "....." << IDX(0, 0, heightOrLength);
	heightField[IDX(heightOrLength - 1, 0, heightOrLength)] = createRandomFloat(rndEngine, 0.f, 1.f);
	//std::cout << heightField[IDX(heightOrLength - 1, 0, heightOrLength)] << "....." << IDX(heightOrLength - 1, 0, heightOrLength);
	heightField[IDX(0, heightOrLength - 1, heightOrLength)] = createRandomFloat(rndEngine, 0.f, 1.f);
	//std::cout << heightField[IDX(0, heightOrLength - 1, heightOrLength)] << "....." << IDX(0, heightOrLength - 1, heightOrLength);
	heightField[IDX(heightOrLength - 1, heightOrLength - 1, heightOrLength)] = createRandomFloat(rndEngine, 0.f, 1.f);
	//std::cout << heightField[IDX(heightOrLength - 1, heightOrLength - 1, heightOrLength)] << "....." << IDX(heightOrLength - 1, heightOrLength - 1, heightOrLength) ;


	int r = heightOrLength;
	float i = 1.0f;
	for (int s = r / 2; s >= 1; s /= 2) {
		//Diamond part
		for (int y = s; y < heightOrLength; y += 2 * s) {
			for (int x = s; x < heightOrLength; x += 2 * s) {
				heightField[IDX(x, y, heightOrLength)] = diamond(x, y, s, heightField, heightOrLength);

				heightField[IDX(x, y, heightOrLength)] += createRandomFloat(rndEngine, 0.f, powf(sigma, i));
			}
		}
		//Square part
		for (int y = s; y < heightOrLength; y += 2 * s) {
			for (int x = s; x < heightOrLength; x += 2 * s) {
				heightField[IDX(x - s, y, heightOrLength)] = square(x - s, y, s, heightField, heightOrLength);
				heightField[IDX(x - s, y, heightOrLength)] += createRandomFloat(rndEngine, 0.f, powf(sigma, i));

				heightField[IDX(x, y - s, heightOrLength)] = square(x, y - s, s, heightField, heightOrLength);
				heightField[IDX(x, y - s, heightOrLength)] += createRandomFloat(rndEngine, 0.f, powf(sigma, i));

				heightField[IDX(x + s, y, heightOrLength)] = square(x + s, y, s, heightField, heightOrLength);
				heightField[IDX(x + s, y, heightOrLength)] += createRandomFloat(rndEngine, 0.f, powf(sigma, i));

				heightField[IDX(x, y + s, heightOrLength)] = square(x, y + s, s, heightField, heightOrLength);
				heightField[IDX(x, y + s, heightOrLength)] += createRandomFloat(rndEngine, 0.f, powf(sigma, i));
			}
		}
		i++;
	}
	//Map values to (0, 1)
	float biggest = heightField[0];
	float smallest = heightField[0];
	for (int i = 1; i < fieldSize; ++i) {
		if (heightField[i] < smallest)
			smallest = heightField[i];
		if (heightField[i] > biggest)
			biggest = heightField[i];
	}




	/*//Discard last row/column
	//float* rightSize = new float[fieldSize];
	float rangeFactor = 1.15f;
	for (int i = 0; i < fieldSize; ++i) {

		heightField[i] -= smallest;
		heightField[i] /= ((biggest - smallest)*rangeFactor);
		//std::cout << "["<< heightField[i] << "]\n";
		//if (counter % heightOrLength != heightOrLength-1 && counter < heightField.size() - heightOrLength) {
	}*/
	std::vector<float> realHeightmap((heightOrLength - 1)*(heightOrLength - 1));
	for (int i = 0; i < (heightOrLength - 1); i++) {
		for (int j = 0; j < (heightOrLength - 1); j++) {
			realHeightmap[i * (heightOrLength - 1) + j] = heightField[i * heightOrLength + j];
		}
	}
	//std::cout << realHeightmap.size();
	return realHeightmap;
}



