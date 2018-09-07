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
	chunk(float Seed, float Size, float XPosition, float ZPOSITION, FastNoise *Noise);
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

	float seed, size, xPosition, zPosition, distanceToCamera;
	bool isVisible;
	unsigned int VAO, EBO, VBO, EBO2;
	std::string positionString;
	std::vector<vertex> vertices;
	std::vector<unsigned int> indices, indices2;
	glm::vec2 positionVector;
	FastNoise* noise;
	enum bioms 
	{
		grassland = 0
	};
	void setUpChunk();
	void generateVertices();
	
};

