#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <ctime>
#include <iostream>
#include <iomanip>
#include <random>
#include <Windows.h>

#include "Camera.h"
#include "chunk.h"
#include "FastNoise.h"
#include "Shader.h"
#include "worldBuilder.h"

//method declarations
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window, float deltaTime);
void mouse_callback(GLFWwindow* window, double mosuePositionY, double mousePositionX);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

//gloabl variables
Camera* camera = new Camera();

//mouse
float lastMouseX = 400.0f;
float lastMouseY = 300.0f;
bool firstMouse = true;

//time
float currentFrame = 0.0f;
float lastFrame = 0.0f;


double clockToMilliseconds(clock_t ticks) {
	// units/(units/time) => time (seconds) * 1000 = milliseconds
	return (ticks / (double)CLOCKS_PER_SEC)*1000.0;
}
int main()
{

	//----------------------------------------------------------------------------------------
	//GLFW Initilization
	//----------------------------------------------------------------------------------------
	#pragma region glfw initilization

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	#pragma endregion

	//----------------------------------------------------------------------------------------
	//Window object creation
	//----------------------------------------------------------------------------------------
	#pragma region window object initilization

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL){
		std::cout << "ERROR: GLFW: Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); //Set the viewport resize function if the windows gets resized
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);	//Set mosue input to capture cursor, but cursor not visible and always within the windw
	#pragma endregion


	//----------------------------------------------------------------------------------------
	//GLAD Initilization
	//----------------------------------------------------------------------------------------
	#pragma region glad initilization

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
		std::cout << "ERROR: GLAD: Failed to initialize GLAD" << std::endl;
		return -1;
	}

	#pragma endregion


	//----------------------------------------------------------------------------------------
	//OpenGL options
	//----------------------------------------------------------------------------------------
	#pragma region opengl options

	//Enable Depth Test
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	//glCullFace(GL_FRONT);

	#pragma endregion


	//Shader Initilization
	Shader ourShader("Shader\\basicVertexShader.txt", "Shader\\basicFragmentShader.txt");
	
	
	//----------------------------------------------------------------------------------------
	//World Setup
	//----------------------------------------------------------------------------------------
	#pragma region world setup

	std::vector<glm::vec2>	chunksToGenerate;
	std::vector<chunk>		activeChunks;
	worldBuilder* builder = new worldBuilder(camera->getCameraPosition(), &chunksToGenerate);

	#pragma endregion

	
	//----------------------------------------------------------------------------------------
	//Texture Initilization
	//----------------------------------------------------------------------------------------
	#pragma region texture setup
	// load and create a texture 
	// -------------------------
	unsigned int texture1, texture2;
	// texture 1
	// ---------
	glGenTextures(1, &texture1);
	glBindTexture(GL_TEXTURE_2D, texture1);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	//set wrap mode for 2D textures to repeat
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	//set filter mode for 2D textures to bilinear
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
	unsigned char *data = stbi_load("Resources\\Textures\\grass.jpg", &width, &height, &nrChannels, 0);
	if (data){
		//arguments: to the texture object bound to slot texture2d; mipmaplevel, color format, ..,.., always 0, which format has the data, what type is the data, data
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);	
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else{
		std::cout << "ERROR: TEXTURE: Failed to load texture" << std::endl;
	}
	stbi_image_free(data);
	// texture 2
	// ---------
	glGenTextures(1, &texture2);
	glBindTexture(GL_TEXTURE_2D, texture2);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	data = stbi_load("Resources\\Textures\\awesomeface.png", &width, &height, &nrChannels, 0);
	if (data){
		// note that the awesomeface.png has transparency and thus an alpha channel, so make sure to tell OpenGL the data type is of GL_RGBA
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else{
		std::cout << "ERROR: TEXTURE: Failed to load texture" << std::endl;
	}
	stbi_image_free(data);

	// tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
	// -------------------------------------------------------------------------------------------
	ourShader.use();
	ourShader.setInt("texture1", 0);
	ourShader.setInt("texture2", 1);
	#pragma endregion
	

	//----------------------------------------------------------------------------------------
	//Matrices Setup
	//----------------------------------------------------------------------------------------
	#pragma region matrices setup

	//projection and view mtrices can be created here, model matrix inside chunk
	glm::mat4 projection, view;																				//projection matrix
	projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 200.0f);	//generating projection matrix - params: fov, ratio, near, far
	ourShader.setMat4("projection", projection);															//does not change, so no need to set it every render call 

	#pragma endregion

	
	//----------------------------------------------------------------------------------------
	//RenderLoop
	//----------------------------------------------------------------------------------------

	while (!glfwWindowShouldClose(window)){

		float deltaTime;
		currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		// input
		// -----
		processInput(window, deltaTime);
		
		// render setup
		// ------
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // also clear the depth buffer now!

		// activate shader
		ourShader.use();

		// pass view matrix to the shader
		view = camera->getViewMatrix();
		ourShader.setMat4("view", view);

		//did we generate all chunks in the queue?
		if (chunksToGenerate.size() == 0) {
			//yes -> check, which chunks have to be drawn since we last checked
			glm::vec3 c = camera->getCameraPosition();
			activeChunks = builder->calculateVisibleChunks(glm::vec2(c.x, c.z), &chunksToGenerate);
		}
		else {
			//no -> no new check but generation of new chunk
			activeChunks.push_back(builder->createChunk(chunksToGenerate[chunksToGenerate.size() - 1].x, chunksToGenerate[chunksToGenerate.size() - 1].y, 40));
			chunksToGenerate.erase(chunksToGenerate.end()-1);
		}

		//get the needed vectors in order to calculate chunk frustrum 
		glm::vec3 camPositon = camera->getCameraPosition();
		glm::vec3 camFront = camera->getCameraFront();
		glm::vec2 camFrontXZ(camFront.x, camFront.z);

		//check for every chunk whether it is within the camera view, and in case follow up draw call - everything in xz plane
		for (int i = 0; i < activeChunks.size(); i++) {
			//vector from camera to chunk center
			glm::vec2 vecToChunk = activeChunks[i].getPosition() - glm::vec2(camPositon.x, camPositon.z);
			//dot product between forward vector and vecToChunk = cos of angle between them
			double anglecos = (camFrontXZ.x * (activeChunks[i].getPosition().x - camPositon.x) + camFrontXZ.y * (activeChunks[i].getPosition().y - camPositon.z)) / (camFrontXZ.length() * vecToChunk.length());
			//calculate angle from cos
			float angle = acos(anglecos);
			//only draw the chunk if the angle is not smaller than 160, or the chunk mid is closer than 5 units 
			if (!(angle < 160) || vecToChunk.length() < 5) {
				activeChunks[i].draw(ourShader, glm::vec2(camPositon.x, camPositon.z));
			}
		}

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}


	//----------------------------------------------------------------------------------------
	//Cleanup
	//----------------------------------------------------------------------------------------

	delete builder;
	// glfw: terminate, clearing all previously allocated GLFW resources.
	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void processInput(GLFWwindow *window, float deltaTime)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera->updateCameraPosition(GLFW_KEY_W, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera->updateCameraPosition(GLFW_KEY_S, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera->updateCameraPosition(GLFW_KEY_D, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera->updateCameraPosition(GLFW_KEY_A, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double mousePositionX, double mousePositionY) {
	//fixes mouse jump at startup of rogram
	if (firstMouse) {
		lastMouseX = mousePositionX;
		lastMouseY = mousePositionY;
		firstMouse = false;
	}

	float mouseOffsetX = mousePositionX - lastMouseX;
	float mouseOffsetY = mousePositionY - lastMouseY;
	lastMouseX = mousePositionX;
	lastMouseY = mousePositionY;
	camera->updateCameraRotation(mouseOffsetX, mouseOffsetY);
}

