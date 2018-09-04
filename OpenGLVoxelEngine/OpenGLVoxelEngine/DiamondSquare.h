#pragma once
#include <random>
#include <vector>
class DiamondSquare {
public:
	DiamondSquare();
	~DiamondSquare();
	std::vector<float> createHeightfield(int resolution, float roughness, std::default_random_engine& rndEngine);
private:
	float diamond(int x, int y, int s, float* field, int fieldWidth);
	float square(int x, int y, int s, float* field, int fieldWidth);

	float createRandomFloat(std::default_random_engine& rndEngine, float midPoint, float roughness);
	int resolution;
};

