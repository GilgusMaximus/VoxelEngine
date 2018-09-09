#include "chunk.h"
#include <iomanip>

chunk::chunk(float Seed, float Size, float XPosition, float ZPosition, FastNoise *Noise, FastNoise *Noise2){
	seed				= Seed; //currently unused
	size				= Size;	//unused
	xPosition			= XPosition;	//position in world on x axis 
	zPosition			= ZPosition;	//position in world on z axis
	positionVector		= glm::vec2(xPosition, zPosition);
	isVisible = true;
	noise = Noise;
	positionString = "" + std::to_string(static_cast<int>(xPosition)) + std::to_string(static_cast<int>(zPosition));

	noise2 = Noise2;
	

	noise3.SetSeed(time(nullptr));
	noise3.SetFrequency(0.1f);
	noise3.SetNoiseType(noise3.Simplex);

	generateVertices();
	setUpChunk();	
}

chunk::~chunk()
{
}

void chunk::cleanup() {
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteVertexArrays(1, &VAO);
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
				//the lowest heightvalue mapped onto the size of voxels, so that the lowest voxel is just a plane

			//positions of cube vertices are calculated through the positions of a standard cube spanend by one corner on (0,0,0) and another on (0.4,0.4,-0.4) (and the otehr 2 on teh corresponding points)
			//on top of these coordinates translation to the right x and z position is added (bc of that the loops from -16 to 15, to make clear, which local coordinates are used)
			

			int  actualValue		= std::round((noise->GetSimplex(j + (xPosition / 0.4f), i + (zPosition / 0.4f))*0.5f + 0.5f) * 100 + (noise2->GetSimplex(j + (xPosition / 0.4f), i + (zPosition / 0.4f))) * 6 /*+ (noise3.GetSimplex(j + (xPosition / 0.4f), i + (zPosition / 0.4f))) * 0.5f*/);
			int  actualValueLinks	= std::round((noise->GetSimplex((j-1) + (xPosition / 0.4f), i + (zPosition / 0.4f))*0.5f + 0.5f) * 100 + (noise2->GetSimplex((j-1) + (xPosition / 0.4f), i + (zPosition / 0.4f))) * 6 /*+ (noise3.GetSimplex((j-1) + (xPosition / 0.4f), i + (zPosition / 0.4f))) * 0.5f*/);
			int  actualValueRechts	= std::round((noise->GetSimplex((j+1) + (xPosition / 0.4f), i + (zPosition / 0.4f))*0.5f + 0.5f) * 100 + (noise2->GetSimplex((j+1) + (xPosition / 0.4f), i + (zPosition / 0.4f))) * 6 /*+ (noise3.GetSimplex((j+1) + (xPosition / 0.4f), i + (zPosition / 0.4f))) * 0.5f*/);
			int  actualValueVorne	= std::round((noise->GetSimplex(j + (xPosition / 0.4f), (i+1) + (zPosition / 0.4f))*0.5f + 0.5f) * 100 + (noise2->GetSimplex(j + (xPosition / 0.4f), (i+1) + (zPosition / 0.4f))) * 6 /*+ (noise3.GetSimplex(j + (xPosition / 0.4f), (i+1) + (zPosition / 0.4f))) * 0.5f*/);			
			int  actualValueHinten	= std::round((noise->GetSimplex(j + (xPosition / 0.4f), (i-1) + (zPosition / 0.4f))*0.5f + 0.5f) * 100 + (noise2->GetSimplex(j + (xPosition / 0.4f), (i-1) + (zPosition / 0.4f))) * 6 /*+ (noise3.GetSimplex(j + (xPosition / 0.4f), (i-1) + (zPosition / 0.4f))) * 0.5f*/);

			float value = (float)(actualValue) * 0.4f;
			
			float heightBottom = (float)actualValue;
			if (actualValueHinten < heightBottom) {
				heightBottom = (float)actualValueHinten;
			}
			if (actualValueLinks < heightBottom) {
				heightBottom = (float)actualValueLinks;
			}
			if (actualValueRechts < heightBottom) {
				heightBottom = (float)actualValueRechts;
			}
			if (actualValueVorne < heightBottom){
				heightBottom = (float)actualValueVorne;
			}
			heightBottom *= 0.4f;
			//actualValue += 0.4f;
			//check wich faces have to be drawn besides the top face, through checking whether the the next cube into teh direction the face is facing has a higehr or lower y value 
			//if it has a lower y value, then we have to draw the face

			//std::cout << "actual: " << actualValue << " actualVorne: " << actualValueVorne;
			if (actualValue > actualValueVorne) {
				//std::cout << " yes" << std::endl;
				indices.push_back(1 + ((i + 20) * 40 + (j + 20)) * 8);	//front
				indices.push_back(3 + ((i + 20) * 40 + (j + 20)) * 8);
				indices.push_back(0 + ((i + 20) * 40 + (j + 20)) * 8);
				indices.push_back(0 + ((i + 20) * 40 + (j + 20)) * 8);
				indices.push_back(3 + ((i + 20) * 40 + (j + 20)) * 8);
				indices.push_back(2 + ((i + 20) * 40 + (j + 20)) * 8);
			}
			//std::cout << "actual: " << actualValue << " actualHinten: " << actualValueHinten;
			if (actualValue > actualValueHinten) {
				//std::cout << " yes" << std::endl;
				indices.push_back(5 + ((i + 20) * 40 + (j + 20)) * 8);	//back
				indices.push_back(4 + ((i + 20) * 40 + (j + 20)) * 8);
				indices.push_back(7 + ((i + 20) * 40 + (j + 20)) * 8);
				indices.push_back(7 + ((i + 20) * 40 + (j + 20)) * 8);
				indices.push_back(4 + ((i + 20) * 40 + (j + 20)) * 8);
				indices.push_back(6 + ((i + 20) * 40 + (j + 20)) * 8);
			}
			//std::cout << "actual: " << actualValue << " actualRechts: " << actualValueRechts;
			if (actualValue > actualValueRechts) {
				//std::cout << " yes" << std::endl;
				indices.push_back(3 + ((i + 20) * 40 + (j + 20)) * 8);	//right
				indices.push_back(1 + ((i + 20) * 40 + (j + 20)) * 8);
				indices.push_back(7 + ((i + 20) * 40 + (j + 20)) * 8);
				indices.push_back(7 + ((i + 20) * 40 + (j + 20)) * 8);
				indices.push_back(1 + ((i + 20) * 40 + (j + 20)) * 8);
				indices.push_back(5 + ((i + 20) * 40 + (j + 20)) * 8);
			}
			//std::cout << "actual: " << actualValue << " actualLinks: " << actualValueLinks;
			if (actualValue > actualValueLinks) {
				//std::cout << " yes" << std::endl;
				indices.push_back(2 + ((i + 20) * 40 + (j + 20)) * 8);	//left
				indices.push_back(6 + ((i + 20) * 40 + (j + 20)) * 8);
				indices.push_back(0 + ((i + 20) * 40 + (j + 20)) * 8);
				indices.push_back(0 + ((i + 20) * 40 + (j + 20)) * 8);
				indices.push_back(6 + ((i + 20) * 40 + (j + 20)) * 8);
				indices.push_back(4 + ((i + 20) * 40 + (j + 20)) * 8);
			}

			//bottom front
			vertices.push_back({ 0.0f + j * 0.4f,  heightBottom,  0.0f + i * 0.4f, 0.3, 0.7, 0.0 });
			vertices.push_back({ 0.4f + j * 0.4f,  heightBottom,  0.0f + i * 0.4f, 0.3, 0.7, 0.0 });

			//frontTiop
			vertices.push_back({ 0.0f + j * 0.4f,  value, 0.0f + i * 0.4f, 0.3, 0.7, 0.0 });
			vertices.push_back({ 0.4f + j * 0.4f,  value, 0.0f + i * 0.4f, 0.3, 0.7, 0.0 });

			//bottom back
			vertices.push_back({ 0.0f + j * 0.4f , heightBottom,  -0.4f + i * 0.4f, 0.5, 0.7, 0.0 });
			vertices.push_back({ 0.4f + j * 0.4f,  heightBottom,  -0.4f + i * 0.4f, 0.5, 0.7, 0.0 });

			//backTop
			vertices.push_back({ 0.0f + j * 0.4f, value, -0.4f + i * 0.4f, 0.5, 0.7, 0.0 });
			vertices.push_back({ 0.4f + j * 0.4f, value, -0.4f + i * 0.4f, 0.5, 0.7, 0.0 });

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
	}
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
	if (distanceToCamera < 80) {											//is the camera close enough?
		//yes -> draw it
		isVisible = true;
		glBindVertexArray(VAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glm::mat4 model;
		model = glm::translate(model, glm::vec3(xPosition, 0, zPosition));
		shader.setMat4("model", model);
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	}
	else {
		//no -> dont draw it
		isVisible = false;
	}

}
