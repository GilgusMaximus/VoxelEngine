#pragma once
#include <vector>
#include <random>
#include <map>
#include <set>
#include <string>
#include "chunk.h"
#include "FastNoise.h"
class worldBuilder
{
public:
	enum generationType
	{
		WORLD_COMP_RANDOM		= 0,	//generate a heightmap with random values
		WORLD_SMOOTH_RANDOM		= 1,	//generate a heightmap with random values and smooth it
		WORLD_DIAMOND_RANDOM	= 2		//generate a heightmap with diamond square algorithm
	};
	struct MyStruct
	{
		float x, y;
	};

	worldBuilder(std::default_random_engine, glm::vec3, std::vector<glm::vec2> *);
	~worldBuilder();

	void generateHeightmap(unsigned int type, int resolution, std::default_random_engine& rndEngine, int smoothAmount = 0, float roughenss = 0.0f);//resolution must be provided - smoothAmount only for smoothRandom generation - roughness only for diamond square
	std::vector<float> getHeightmap();
	void printHeightmap(unsigned int resolution);
	std::vector<chunk> calculateVisibleChunks(glm::vec2 cameraPositionXZ, std::vector<glm::vec2> *chunksToGenerate);
	chunk createChunk(int i, int j, int genSize);
private:
	std::vector<float> heightmap;
	std::vector<chunk> activeChunks;
	std::map<std::string, chunk> chunkMap;
	std::default_random_engine randomEngine;
	FastNoise noise;
	
	void generateHeightmapRandom(unsigned int resolution);
	void smoothHeightmap(int smoothIterations, unsigned int resolution);
	void generateHeightmapDiamond(unsigned int resolution, float roughness, std::default_random_engine& rndEngine);
	void readjustHeightmapsize(unsigned int resolution);
	float smoothMid(unsigned int x, unsigned int y, unsigned int resolution);
	float smoothEdge(unsigned int x, unsigned int y, unsigned int resolution);
	float smoothCorner(unsigned int x, unsigned int y, unsigned int resolution);
};

