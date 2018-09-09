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
	chunk(float Seed, float Size, float XPosition, float ZPOSITION, FastNoise *Noise, FastNoise *noise2);
	~chunk();
	void draw(Shader shader, glm::vec2 cameraXZ);
	bool getVisibility();
	glm::vec2 getPosition();
	float calculateVectorLength(glm::vec2);
	std::string getPositionString();
	void cleanup();
private:
	struct vertex {
		float x, y, z;
		float r, g, b;
	};
	struct color {
		float r;
		float g;
		float b;
		float a;
	};
	float seed, size, xPosition, zPosition, distanceToCamera;
	bool isVisible;
	unsigned int VAO, EBO, VBO, EBO2;
	std::string positionString;
	std::vector<vertex> vertices;
	std::vector<unsigned int> indices, indices2;
	glm::vec2 positionVector;
	FastNoise* noise;
	FastNoise* noise2;
	FastNoise noise3;

	color water = { 64.0f / 255.0f, 204.0f/255.0f, 255.0f/255.0f, 1 };
	color grass = { 47.0f/255.0f, 204.0f/255.0f,   0, 1 };
	color stone = { 150.0f/255.0f, 152.0f/255.0f, 152.0f/255.0f, 1 };
	color snow = { 235.0f/255.0f, 255.0f/255.0f, 255.0f/255.0f, 1 };

	enum bioms 
	{
		grassland = 0
	};
	void setUpChunk();
	void generateVertices();
	
};

