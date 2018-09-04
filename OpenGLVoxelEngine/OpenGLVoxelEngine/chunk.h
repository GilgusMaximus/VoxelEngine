#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>

#include <vector>
#include "FastNoise.h"
#include "Shader.h"
#include "Camera.h"
class chunk
{
public:
	chunk(float Seed, float Size, std::vector<float> Heightmap, float XPosition, float ZPOSITION,  float HighestHeightValue, float LowestHeightValue, glm::vec3 cameraPosition, FastNoise Noise);
	~chunk();
	void draw(Shader shader, glm::vec2 cameraXZ);
	bool getVisibility();
	glm::vec2 getPosition();
	float calculateVectorLength(glm::vec2);
	std::string getPositionString();
private:
	struct vertex {
		float x, y, z;
		float r, g, b;
	};

	float seed, size, xPosition, zPosition, highestHeightValue, lowestHeightValue, distanceToCamera;
	bool isVisible;
	std::string positionString;
	std::vector<float> heightmap;			//the surface blocks
	std::vector<vertex> vertices;
	std::vector<unsigned int> indices, indices2;
	enum bioms 
	{
		grassland = 0
	};
	void setUpChunk();
	void generateVertices();
	unsigned int VAO, EBO, VBO, EBO2;
	glm::vec2 positionVector;
	FastNoise noise;
};

