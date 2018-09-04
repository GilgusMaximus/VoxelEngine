#include "chunk.h"
#include <iomanip>

chunk::chunk(float Seed, float Size, std::vector<float> Heightmap, float XPosition, float ZPosition, float HighestHeightValue, float LowestHeightValue, glm::vec3 cameraPosition, FastNoise Noise){
	seed				= Seed; //currently unused
	size				= Size;	//unused
	heightmap			= Heightmap;
	xPosition			= XPosition;	//position in world on x axis 
	zPosition			= ZPosition;	//position in world on z axis
	highestHeightValue	= HighestHeightValue;	//unused
	lowestHeightValue	= LowestHeightValue;	//the lowest heightvalue, used to minimize the y stretch value of the voxels
	positionVector		= glm::vec2(xPosition, zPosition);
	distanceToCamera =  calculateVectorLength( positionVector - glm::vec2(cameraPosition.x, cameraPosition.z));
	isVisible = true;
	noise = Noise;
	positionString = "" + std::to_string(static_cast<int>(xPosition)) + std::to_string(static_cast<int>(zPosition));
	generateVertices();
	setUpChunk();	
}

chunk::~chunk()
{
}

//setup Buffers
void chunk::setUpChunk() {	
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glGenBuffers(1, &EBO2);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertex), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *) 0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *) (3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	/*glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO2);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices2.size() * sizeof(unsigned int), &indices2[0], GL_STATIC_DRAW);*/

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
}

//fill vertex and indexbuffer
void chunk::generateVertices() {
	for (int i = -20; i < 20; i++) {	//currently fixed size of 32x32 voxels per chunk
		for (int j = -20; j < 20; j++) {
			float heightBottom = lowestHeightValue * 0.4f;	//the lowest heightvalue mapped onto the size of voxels, so that the lowest voxel is just a plane
			if (i != -20 && i != 19 && j != -20 && j != 19) {	//are we on an edge or corner?
				//no -> we search on all 4 adjacent sides for the highest height value, that is at maxmimum one block unit lower than the current block  
					
				//+x
				float adjacent = heightmap[(i + 20 + 1) * 40 + j + 20] * 0.4f;	//calculating adjacent, because otherwise need to look it up 2 or 3 times per check
				if (adjacent > heightBottom && adjacent < heightmap[(i + 20) * 40 + j + 20] * 0.4f)	//is adjacent heightvalue greater than the lowestValue but still lower than the current height value
					heightBottom = adjacent;

				//-x
				adjacent = heightmap[(i + 20 - 1) * 40 + j + 20] * 0.4f;
				if (adjacent > heightBottom && adjacent < heightmap[(i + 20) * 40 + j + 20] * 0.4f)
					heightBottom = adjacent;

				//+z
				adjacent = heightmap[(i + 20) * 40 + j + 20 + 1] * 0.4f;
				if (adjacent > heightBottom && adjacent < heightmap[(i + 20) * 40 + j + 20] * 0.4f)
					heightBottom = adjacent;

				//-z
				adjacent = heightmap[(i + 20) * 40 + j + 20 - 1] * 0.4f;
				if (adjacent > heightBottom && adjacent < heightmap[(i + 20) * 40 + j + 20] * 0.4f)
					heightBottom = adjacent;
			}
			heightBottom = 0.0f;
			//yes -> skip making the stretch value of the voxel on y smaller, because we are on the outside of teh chunk -> no value to check against
			//TODO: implementing to check against edge values of other chunk

			//positions of cube vertices are calculated through the positions of a standard cube spanend by one corner on (0,0,0) and another on (0.4,0.4,-0.4) (and the otehr 2 on teh corresponding points)
			//on top of these coordinates translation to the right x and z position is added (bc of that the loops from -16 to 15, to make clear, which local coordinates are used)
			//bottom front
			vertices.push_back({ 0.0f + j * 0.4f,  heightBottom,  0.0f + i * 0.4f, 0.3, 0.7, 0.0 }); 	
			vertices.push_back({ 0.4f + j * 0.4f,  heightBottom,  0.0f + i * 0.4f, 0.3, 0.7, 0.0 });

			//std::cout << std::setw(6) << std::fixed << std::setprecision(3) << std::round(noise.GetSimplex(j + (xPosition / 0.4f), i + (zPosition / 0.4f)) * 10)  << " ";
			
			int  v = std::round((noise.GetSimplex(j + (xPosition / 0.4f), i + (zPosition / 0.4f))*0.5f+0.5f) * 10);
			float value = (float)(v) * 0.4f;
			
			//std::cout << std::setw(6) << std::fixed << std::setprecision(3) << value << " ";

			vertices.push_back({ 0.0f + j * 0.4f,  value, 0.0f + i * 0.4f, 0.3, 0.7, 0.0 });
			vertices.push_back({ 0.4f + j * 0.4f,  value, 0.0f + i * 0.4f, 0.3, 0.7, 0.0 });

			/*
			//top front
			vertices.push_back({ 0.0f + j * 0.4f,  heightmap[(i + 16) * 32 + j + 16] * 0.4f, 0.0f + i * 0.4f, 0.3, 0.7, 0.0 });
			vertices.push_back({ 0.4f + j * 0.4f,  heightmap[(i + 16) * 32 + j + 16] * 0.4f, 0.0f + i * 0.4f, 0.3, 0.7, 0.0 });
			*/
			//bottom back
			vertices.push_back({ 0.0f + j * 0.4f , heightBottom,  -0.4f + i * 0.4f, 0.5, 0.7, 0.0 });
			vertices.push_back({ 0.4f + j * 0.4f,  heightBottom,  -0.4f + i * 0.4f, 0.5, 0.7, 0.0 });

			vertices.push_back({ 0.0f + j * 0.4f, value, -0.4f + i * 0.4f, 0.5, 0.7, 0.0 });
			vertices.push_back({ 0.4f + j * 0.4f, value, -0.4f + i * 0.4f, 0.5, 0.7, 0.0 });

			/*
			//top back
			vertices.push_back({ 0.0f + j * 0.4f,  heightmap[(i + 16) * 32 + j + 16] * 0.4f, -0.4f + i * 0.4f, 0.5, 0.7, 0.0 });
			vertices.push_back({ 0.4f + j * 0.4f,  heightmap[(i + 16) * 32 + j + 16] * 0.4f, -0.4f + i * 0.4f, 0.5, 0.7, 0.0 });
			*/
			//check wich faces have to be drawn besides the top face, through checking whether the the next cube into teh direction the face is facing has a higehr or lower y value 
			//if it has a lower y value, then we have to draw the face
			//if (i == 15 || (i < 16 && heightmap[((i)+16) * 32 + j + 16] > heightmap[((i + 1) + 16) * 32 + j + 16])) {
				indices.push_back(1 + ((i + 20) * 40 + (j + 20)) * 8);	//front
				indices.push_back(3 + ((i + 20) * 40 + (j + 20)) * 8);
				indices.push_back(0 + ((i + 20) * 40 + (j + 20)) * 8);
				indices.push_back(0 + ((i + 20) * 40 + (j + 20)) * 8);
				indices.push_back(3 + ((i + 20) * 40 + (j + 20)) * 8);
				indices.push_back(2 + ((i + 20) * 40 + (j + 20)) * 8);
			//}

			//if (i == -16 || (i < 16 && heightmap[((i)+16) * 32 + j + 16] > heightmap[((i - 1) + 16) * 32 + j + 16])) {
				indices.push_back(5 + ((i + 20) * 40 + (j + 20)) * 8);	//back
				indices.push_back(4 + ((i + 20) * 40 + (j + 20)) * 8);
				indices.push_back(7 + ((i + 20) * 40 + (j + 20)) * 8);
				indices.push_back(7 + ((i + 20) * 40 + (j + 20)) * 8);
				indices.push_back(4 + ((i + 20) * 40 + (j + 20)) * 8);
				indices.push_back(6 + ((i + 20) * 40 + (j + 20)) * 8);
			//}

			//if (j == 15 || j < 20 && heightmap[((i)+20) * 40 + j + 20] > heightmap[((i)+20) * 40 + j + 1 + 20]) {
				indices.push_back(3 + ((i + 20) * 40 + (j + 20)) * 8);	//right
				indices.push_back(1 + ((i + 20) * 40 + (j + 20)) * 8);
				indices.push_back(7 + ((i + 20) * 40 + (j + 20)) * 8);
				indices.push_back(7 + ((i + 20) * 40 + (j + 20)) * 8);
				indices.push_back(1 + ((i + 20) * 40 + (j + 20)) * 8);
				indices.push_back(5 + ((i + 20) * 40 + (j + 20)) * 8);
			//}

			//if (j == -20 || j < 20 && heightmap[((i)+20) * 40 + j + 20] > heightmap[((i)+20) * 40 + j - 1 + 20]) {
				indices.push_back(2 + ((i + 20) * 40 + (j + 20)) * 8);	//left
				indices.push_back(6 + ((i + 20) * 40 + (j + 20)) * 8);
				indices.push_back(0 + ((i + 20) * 40 + (j + 20)) * 8);
				indices.push_back(0 + ((i + 20) * 40 + (j + 20)) * 8);
				indices.push_back(6 + ((i + 20) * 40 + (j + 20)) * 8);
				indices.push_back(4 + ((i + 20) * 40 + (j + 20)) * 8);
			//}

			indices.push_back(6 + ((i + 20) * 40 + (j + 20)) * 8);	//top has to be drawn in al lcases, because there are no layers of cubes, but just the cube streched to the bottom
			indices.push_back(2 + ((i + 20) * 40 + (j + 20)) * 8);
			indices.push_back(7 + ((i + 20) * 40 + (j + 20)) * 8);
			indices.push_back(7 + ((i + 20) * 40 + (j + 20)) * 8);
			indices.push_back(2 + ((i + 20) * 40 + (j + 20)) * 8);
			indices.push_back(3 + ((i + 20) * 40 + (j + 20)) * 8);

			/*indices.push_back(4 + ((i + 16) * 32 + (j + 16)) * 8);	//bottom
			indices.push_back(5 + ((i + 16) * 32 + (j + 16)) * 8);
			indices.push_back(0 + ((i + 16) * 32 + (j + 16)) * 8);
			indices.push_back(0 + ((i + 16) * 32 + (j + 16)) * 8);
			indices.push_back(5 + ((i + 16) * 32 + (j + 16)) * 8);
			indices.push_back(1 + ((i + 16) * 32 + (j + 16)) * 8);*/
		}
		//std::cout << std::endl;
	}
	//std::cout << std::endl;
}

//calculate the length of the vector
float chunk::calculateVectorLength(glm::vec2 vector) {
	vector = (positionVector - vector);
	return sqrt(vector.x * vector.x + vector.y * vector.y);
}

bool chunk::getVisibility() {
	return isVisible;
}

glm::vec2 chunk::getPosition() {
	return positionVector;
}
std::string chunk::getPositionString() {
	return positionString;
}
//draw vertex buffer defined by indexbuffer
void chunk::draw(Shader shader, glm::vec2 cameraXZ) {
	distanceToCamera = calculateVectorLength(cameraXZ);	//calculate distance to camera
	if (distanceToCamera < 40) {											//is the camera close enough?
		//yes -> draw it
		isVisible = true;
		glBindVertexArray(VAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glm::mat4 model;
		model = glm::translate(model, glm::vec3(xPosition, 0/*(-lowestHeightValue * 0.4f) - 2*/, zPosition));
		shader.setMat4("model", model);
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	}
	else {
		//no -> dont draw it
		isVisible = false;
	}

}
