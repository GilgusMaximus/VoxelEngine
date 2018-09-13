#include <iostream>
#include <time.h>
#include <iomanip>


#include "DiamondSquare.h"
#include "worldBuilder.h"

worldBuilder::worldBuilder( glm::vec3 cameraPosition, std::vector<glm::vec2> *chunksToGenerate)
{
	//Noise generator setups
	noiseBottomLayer.SetNoiseType(noiseBottomLayer.SimplexFractal);
	noiseBottomLayer.SetSeed(time(nullptr));
	noiseBottomLayer.SetFrequency(0.004f);

	noiseTopLayer.SetNoiseType(noiseTopLayer.SimplexFractal);
	noiseTopLayer.SetSeed(time(nullptr));
	noiseTopLayer.SetFrequency(0.04f);

	genSize = 40;

	//initial 11x11 chunk generation
	for (int i = 0; i < 11; i++) {
		for (int j = 0; j < 11; j++) {
			chunk c = createChunk((i - 5) * 16, (j - 5) * 16, genSize);
			activeChunks.push_back(c);			
			//saving the chunk in the general chunk database (map atm, circular buffer probably better choice)
			std::pair<std::string, chunk> a(c.getPositionString(), c);
			chunkMap.insert(a);
		}
	}
}

worldBuilder::~worldBuilder()
{
	for (std::map<std::string,chunk>::iterator it = chunkMap.begin(); it != chunkMap.end(); ) {
		chunk c = it->second;
		it++;
		c.cleanup();
	}
}

chunk worldBuilder::createChunk(int posX, int posZ, int genSize) {
	//generating the actual chunk - params: seed, sidelength of chunk, heightmap, xPosition, zPosition, higehst value, lowest value
	chunk c(0, 32, posX, posZ, &noiseBottomLayer, &noiseTopLayer);	
	//insert chunk into map of already generate chunks
	chunkMap.insert(std::pair<std::string, chunk>(c.getPositionString(), c));
	return c;
}

std::vector<chunk> worldBuilder::calculateVisibleChunks(glm::vec2 cameraPositionXZ, std::vector<glm::vec2> *chunksToGenerate) {
	int currentChunkIndex = 48;
	float distance = activeChunks[0].calculateVectorLength(cameraPositionXZ);
	
	//calculating the chunk index we are in - only check between 48 and 73, because these are the 8 surrounding chunks
	for (int i = 49; i < 73; i++) {
		float testDistance = activeChunks[i].calculateVectorLength(cameraPositionXZ);
		if (testDistance < distance) {
			distance = testDistance;
			currentChunkIndex = i;
		}
	}

	glm::vec2 currentChunkPos = activeChunks[currentChunkIndex].getPosition();
	activeChunks.clear();
	for (int i = 0; i < 11; i++) {
		for (int j = 0; j < 11; j++) {
			//calculate world position of all chunks that will be active
			glm::vec2 asa((currentChunkPos.x + (i - 5) * 16), (currentChunkPos.y + (j - 5) * 16));
			//get the world position of the chunk as string
			std::string newChunkPos = "" + std::to_string(static_cast<int>(asa.x)) + std::to_string(static_cast<int>(asa.y));
			//does the map alrady contain this key? 
			std::map<std::string, chunk>::iterator it = chunkMap.find(newChunkPos);
			if ( it  != chunkMap.end()) {
				//yes -> push back the value
				activeChunks.push_back(it->second);
			}
			else {
				//no -> push the world coordinates into the generation queue
				chunksToGenerate->push_back(asa);
			}
		}
	}
	return activeChunks;
}