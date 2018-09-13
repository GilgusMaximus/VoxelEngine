#pragma once
#include <vector>

#include <map>
#include <string>

#include "chunk.h"
#include "FastNoise.h"
class worldBuilder
{
public:

	worldBuilder(glm::vec3, std::vector<glm::vec2> *);
	~worldBuilder();

	std::vector<chunk> calculateVisibleChunks(glm::vec2 cameraPositionXZ, std::vector<glm::vec2> *chunksToGenerate);
	chunk createChunk(int i, int j, int genSize);
private:
	int genSize;
	std::vector<float> heightmap;
	std::vector<chunk> activeChunks;
	std::map<std::string, chunk> chunkMap;
	FastNoise noiseBottomLayer, noiseTopLayer;
	
};

