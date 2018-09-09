#include <iostream>
#include <time.h>
#include <iomanip>


#include "DiamondSquare.h"
#include "worldBuilder.h"

worldBuilder::worldBuilder(std::default_random_engine RandomEngine, glm::vec3 cameraPosition, std::vector<glm::vec2> *chunksToGenerate)
{
	//----------------------------------------------------------------------------------------
//World Generation Setup
//----------------------------------------------------------------------------------------
#pragma region worldgeneration
	
	noise.SetNoiseType(noise.Simplex);
	noise.SetSeed(time(nullptr));
	noise.SetFrequency(0.004f);

	noise2.SetNoiseType(noise2.Simplex);
	noise2.SetFrequency(0.04f);
	int genSize = 40;
	randomEngine = RandomEngine;
	randomEngine.seed(time(nullptr));

	//draw 11x11 chunks
	//TODO: moving it to the gameloop
	for (int i = 0; i < 11; i++) {
		for (int j = 0; j < 11; j++) {
			chunk c = createChunk((i - 5) * 16, (j - 5) * 16, genSize);
			activeChunks.push_back(c);			
			//saving the chunk in the general chunk database (vector so far, probably map as best choice)
			std::pair<std::string, chunk> a(c.getPositionString(), c);
			//std::cout << c.getPositionString() << " " << std::endl;
			chunkMap.insert(a);
		}
	}
	int currentChunk = 4;
#pragma endregion
}

bool ja = true;

worldBuilder::~worldBuilder()
{
	for (std::map<std::string,chunk>::iterator it = chunkMap.begin(); it != chunkMap.end(); ) {
		chunk c = it->second;
		it++;
		c.cleanup();
	}
}

chunk worldBuilder::createChunk(int posX, int posZ, int genSize) {
	chunk c(0, 32, posX, posZ, &noise, &noise2);	//generating the actual chunk - params: seed, sidelength of chunk, heightmap, xPosition, zPosition, higehst value, lowest value
	//std::cout << posX << " " << posZ << " " << c.getPositionString() << std::endl;
	chunkMap.insert(std::pair<std::string, chunk>(c.getPositionString(), c));
	//std::cout << chunkMap.size() << std::endl;
	return c;
}

void worldBuilder::printHeightmap(unsigned int resolution) {
	for (int i = 0; i < resolution; i++) {
		for (int j = 0; j < resolution; j++) {
			std::cout << std::fixed << std::setprecision(3) << std::setw(5) << heightmap[i * (resolution) + j] << " ";
		}
		std::cout << std::endl;
	}
}

void worldBuilder::generateHeightmap(unsigned int type, int resolution, std::default_random_engine& rndEngine, int smoothAmount, float roughenss) {
	heightmap = std::vector<float>((resolution+1)*(resolution+1));	//to give give an extra value on side of the outer edges -> better smoothing
	//depending on passed enum, a different map generation type is executed
	switch (type) {
		case worldBuilder::WORLD_COMP_RANDOM:
			generateHeightmapRandom(resolution);
			readjustHeightmapsize(resolution);
			break;
		case worldBuilder::WORLD_SMOOTH_RANDOM:
			generateHeightmapRandom(resolution);
			smoothHeightmap(smoothAmount, resolution);
			readjustHeightmapsize(resolution);
			break;
		case worldBuilder::WORLD_DIAMOND_RANDOM:
			generateHeightmapDiamond(resolution, roughenss, rndEngine);
			break;
		default:
			std::cout << "ERROR: WORLDBUILDER: INVALID GENERATION TYPE" << std::endl;
			break;
	}
}

//generating the heightmap completely random, with no correlation between the values
void worldBuilder::generateHeightmapRandom(unsigned int resolution) {
	srand(time(nullptr));	//seed the generator with current time
	for (int i = 0; i <= resolution; i++) {
		for (int j = 0; j <= resolution; j++) {
			heightmap[i * (resolution + 1) + j] = ((float)(rand()%10))/10.0f;
		}
	}
}

//smoothing a heightmap via 9x smooth - can be used with random generation, to create a sort of correlation between teh values
void worldBuilder::smoothHeightmap(int smoothIterations, unsigned int resolution) {	
	//averaging a value at position j, k through calculating the average of all 8 adjacent values + value itsself
	for (int i = 0; i < smoothIterations; i++) {
		std::vector<float> newHeightMap((resolution+1)*(resolution+1));
		//std::cout << newHeightMap.size();
		for (int j = 0; j <= resolution; j++) {
			for (int k = 0; k <= resolution; k++) {
				if((j == 0 && k == 0) || (j == 0 && k == resolution) || (j == resolution && k == 0) || (j == resolution && k == resolution))
					newHeightMap[j * (resolution + 1) + k] = smoothCorner(k, j, resolution);
				else if (j == 0 || j == resolution || k == 0 || k == resolution) 
					newHeightMap[j * (resolution + 1) + k] = smoothEdge(k, j, resolution);
				
				else
					newHeightMap[j * (resolution + 1) + k] = smoothMid(k, j, resolution);				
			}
		}
		heightmap = newHeightMap;
	}
}

//smooth cases
float worldBuilder::smoothEdge(unsigned int x, unsigned int y, unsigned int resolution) {
	float value = 0.0f;

	if (x == 0) {
		value += 2 * heightmap[(y-1) * (resolution + 1)];
		value += 2 * heightmap[y * (resolution + 1)];
		value += 2 * heightmap[(y+1) * (resolution + 1)];

		value += heightmap[(y+1) * (resolution + 1) + 1];
		value += heightmap[y * (resolution + 1) + 1];
		value += heightmap[(y-1) * (resolution + 1) + 1]; 
	}
	if (x == resolution) {
 		value += 2 * heightmap[(y - 1) * (resolution + 1) + x];
		value += 2 * heightmap[y * (resolution + 1) + x];
		value += 2 * heightmap[(y + 1) * (resolution + 1) + x];
		value += heightmap[(y + 1) * (resolution + 1) + x - 1];
		value += heightmap[y * (resolution + 1) + x - 1];
		value += heightmap[(y - 1) * (resolution + 1) + x - 1];
	}
	if (y == 0) {
		value += 2 * heightmap[(x - 1)];
		value += 2 * heightmap[x];
		value += 2 * heightmap[(x + 1)];

		value += heightmap[resolution + 1 + (x - 1)];
		value += heightmap[resolution + 1 + (x)];
		value += heightmap[resolution + 1 + (x + 1)];
	}
	if (y == resolution) {
		value += 2 * heightmap[y * (resolution + 1) + (x - 1)];
		value += 2 * heightmap[y * (resolution + 1) + x];
		value += 2 * heightmap[y * (resolution + 1) + (x + 1)];

		value += heightmap[y * (resolution + 1) + (x - 1)];
		value += heightmap[y * (resolution + 1) + (x)];
		value += heightmap[y * (resolution + 1) + (x + 1)];
	}
	return value / 9.0f;
}
float worldBuilder::smoothCorner(unsigned int x, unsigned int y, unsigned int resolution) {
	float value = 0.0f;
	if (x == 0 && y == 0) {
		value += 4 * heightmap[y * (resolution + 1) + x];
		value += 2 * heightmap[y * (resolution + 1) + x + 1];
		value += 2 * heightmap[(y + 1) * (resolution + 1) + x];
		value += heightmap[(y + 1) * (resolution + 1) + x + 1];
	}
	else if (x == 0) {
		value += 4 * heightmap[y * (resolution + 1) + x];
		value += 2 * heightmap[y * (resolution + 1) + x + 1];
		value += 2 * heightmap[(y - 1) * (resolution + 1) + x];
		value += heightmap[(y - 1) * (resolution + 1) + x + 1];
	}
	else if (y == 0) {
		value += 4 * heightmap[y * (resolution + 1) + x];
		value += 2 * heightmap[y * (resolution + 1) + x - 1];
		value += 2 * heightmap[(y + 1) * (resolution + 1) + x];
		value += heightmap[(y + 1) * (resolution + 1) + x - 1];
	}
	else {
		value += 4 * heightmap[y * (resolution + 1) + x];
		value += 2 * heightmap[y * (resolution + 1) + x - 1];
		value += 2 * heightmap[(y - 1) * (resolution + 1) + x];
		value += heightmap[(y - 1) * (resolution + 1) + x - 1];
	}
	return value / 9.0f;
}
float worldBuilder::smoothMid(unsigned int x, unsigned int y, unsigned int resolution) {
	float value = 0.0f;
	value += heightmap[(y - 1) * (resolution + 1) + x - 1];
	value += heightmap[(y - 1) * (resolution + 1) + x];
	value += heightmap[(y - 1) * (resolution + 1) + x + 1];

	value += heightmap[y * (resolution + 1) + x - 1];
	value += heightmap[y * (resolution + 1) + x];
	value += heightmap[y * (resolution + 1) + x + 1];

	value += heightmap[(y + 1) * (resolution + 1) + x - 1];
	value += heightmap[(y + 1) * (resolution + 1) + x];
	value += heightmap[(y + 1) * (resolution + 1) + x + 1];
	return value / 9.0f;
}

//discarding the extra column and row
void worldBuilder::readjustHeightmapsize(unsigned int resolution) {
	std::vector<float> finalHeight(resolution*resolution);
	for(int i = 0; i < resolution; i++){
		for (int j = 0; j < resolution; j++) {
			finalHeight[i * (resolution)+j] = heightmap[i * (resolution + 1) + j];
		}
	}
	heightmap = finalHeight;
}

//generating the heightmap through the DiamondSquare Class
//TODO: Decide whetehr always a new DiamondSquare should be created or only using 1
void worldBuilder::generateHeightmapDiamond(unsigned int resolution, float roughness, std::default_random_engine& rndEngine){
	DiamondSquare gen;
	heightmap = gen.createHeightfield(resolution, roughness, rndEngine);
}

std::vector<float> worldBuilder::getHeightmap() {
	return heightmap;
}

std::vector<chunk> worldBuilder::calculateVisibleChunks(glm::vec2 cameraPositionXZ, std::vector<glm::vec2> *chunksToGenerate) {
	int index = 48;
	float distance = activeChunks[0].calculateVectorLength(cameraPositionXZ);
	
	for (int i = 49; i < 73; i++) {
		float testDistance = activeChunks[i].calculateVectorLength(cameraPositionXZ);
		if (testDistance < distance) {
			distance = testDistance;
			index = i;
		}
	}
	glm::vec2 currentChunkPos = activeChunks[index].getPosition();
	activeChunks.clear();
	for (int i = 0; i < 11; i++) {
		for (int j = 0; j < 11; j++) {
			glm::vec2 asa((currentChunkPos.x + (i - 5) * 16), (currentChunkPos.y + (j - 5) * 16));
			std::string newChunkPos = "" + std::to_string(static_cast<int>(asa.x)) + std::to_string(static_cast<int>(asa.y));
			std::map<std::string, chunk>::iterator it = chunkMap.find(newChunkPos);
			if ( it  != chunkMap.end()) {
				activeChunks.push_back(it->second);
			}
			else {
				chunksToGenerate->push_back(asa);
			}
		}
	}
	return activeChunks;
}