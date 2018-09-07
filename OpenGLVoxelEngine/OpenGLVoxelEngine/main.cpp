#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <ctime>
#include <iomanip>
#include <random>

#include "Shader.h"
#include "Camera.h"
#include "worldBuilder.h"
#include "chunk.h"
#include "FastNoise.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window, float deltaTime);
void mouse_callback(GLFWwindow* window, double mosuePositionY, double mousePositionX);
// settings
const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;

//gloabl variables

//random value generator
std::default_random_engine randomEngine;

//camera
Camera* camera = new Camera();

//mouse
float lastMouseX = 400.0f;
float lastMouseY = 300.0f;
bool firstMouse = true;

//time
float currentFrame = 0.0f;
float lastFrame = 0.0f;

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
	std::vector<glm::vec2> chunksToGenerate;

	worldBuilder* builder = new worldBuilder(randomEngine, camera->getCameraPosition(), &chunksToGenerate);
	
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
	//RenderLoop
	//----------------------------------------------------------------------------------------
	glm::mat4 projection;																					//projection matrix
	projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 200.0f); //generating projection matrix - params: fov, ratio, near, far
	ourShader.setMat4("projection", projection);															//does not change, so no need to set it every render call 
	
	std::vector<chunk> activeChunks;
	while (!glfwWindowShouldClose(window)){
		//std::cout << camera->getCameraPosition().x << " " << camera->getCameraPosition().y << " " << camera->getCameraPosition().z << std::endl;
		//calculating amount of passed time since last calc
		float deltaTime;
		currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		// input
		// -----
		processInput(window, deltaTime);
		
		// render
		// ------
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // also clear the depth buffer now!

		 // bind textures on corresponding texture units
		/*glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture2);*/

		// activate shader
		ourShader.use();

		// create viewMatrix
		glm::mat4 view;
		view = camera->getViewMatrix();
		// pass view matrix to the shader
		ourShader.setMat4("view", view);
		for (int i = 0; i < chunksToGenerate.size(); i++) {
			//std::cout << chunksToGenerate[i].x << " " << chunksToGenerate[i].y << std::endl;
		}
		if (chunksToGenerate.size() == 0) {
			glm::vec3 c = camera->getCameraPosition();
			activeChunks = builder->calculateVisibleChunks(glm::vec2(c.x, c.z), &chunksToGenerate);
		}
		else {
			//chunksToGenerate[chunksToGenerate.size()-1].x
			activeChunks.push_back(builder->createChunk(chunksToGenerate[chunksToGenerate.size() - 1].x, chunksToGenerate[chunksToGenerate.size() - 1].y, 40));
			chunksToGenerate.erase(chunksToGenerate.end()-1);
			//std::cout << chunksToGenerate.size();
		}
		//call to all chunks to draw their data
		for (int i = 0; i < activeChunks.size(); i++) {
			activeChunks[i].draw(ourShader, glm::vec2(camera->getCameraPosition().x, camera->getCameraPosition().z));
		}

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}


	//----------------------------------------------------------------------------------------
	//Cleanup
	//----------------------------------------------------------------------------------------
	/*glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);*/
	delete builder;

	// glfw: terminate, clearing all previously allocated GLFW resources.
	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
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
// ---------------------------------------------------------------------------------------------
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

