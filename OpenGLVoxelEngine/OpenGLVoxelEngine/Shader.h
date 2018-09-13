#pragma once
#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h> // include glad to get all the required OpenGL headers
#include <iostream>
#include <glm/common.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <fstream>
#include <sstream>

class Shader
{
public:
	// the program ID
	unsigned int ID;

	// constructor reads and builds the shader
	Shader(const char* vertexPath, const char* fragmentPath) {
		std::string vertexShaderCode, fragmentShaderCode;
		std::ifstream vertexStream;
		std::ifstream fragmentStream;
		//----------------------------------------------------------------------------------------
		//read the shader codes from the files
		//----------------------------------------------------------------------------------------
		#pragma region shaderRetrieve
		try {
			vertexStream.open(vertexPath);
			fragmentStream.open(fragmentPath);
			vertexShaderCode.assign((std::istreambuf_iterator<char>(vertexStream)),(std::istreambuf_iterator<char>()));	//inside assign a string consructor with 2 interators; first set to iterate over the stream, last one is a standard iterator which returns true until the first one rusn out of data
			fragmentShaderCode.assign((std::istreambuf_iterator<char>(fragmentStream)), (std::istreambuf_iterator<char>()));
			vertexStream.close();
			fragmentStream.close();
		}
		catch (std::ifstream::failure e) {
			std::cout << "ERROR: SHADER: INVALID SHADERFILES " << vertexPath << " " << fragmentPath << " COULD NOT BE READ CORRECTLY" << std::endl;
		}
		#pragma endregion
		
		//----------------------------------------------------------------------------------------
		//compile the fragment and vertex shader
		//----------------------------------------------------------------------------------------
		#pragma region shaderCompilation

		const char* vertexShaderC(vertexShaderCode.c_str());
		const char* fragmentShaderC(fragmentShaderCode.c_str());
		
		unsigned int vertexShader, pixelShader;
		int success;
		char info[512];
		vertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexShader, 1, &vertexShaderC, NULL);
		glCompileShader(vertexShader);
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(vertexShader, 512, NULL, info);
			std::cout << "ERROR: SHADER: COMPILATION ERROR VERTEXSHADER " << info << std::endl;
		}

		pixelShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(pixelShader, 1, &fragmentShaderC, NULL);
		glCompileShader(pixelShader);
		glGetShaderiv(pixelShader, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(pixelShader, 512, NULL, info);
			std::cout << "ERROR: SHADER: COMPILATION ERROR PIXELSHADER " << info << std::endl;
		}
		#pragma endregion

		//----------------------------------------------------------------------------------------
		//create shader ID and link the shaders from above
		//----------------------------------------------------------------------------------------
		#pragma region shaderIDInitilization

		ID = glCreateProgram();
		glAttachShader(ID, vertexShader);
		glAttachShader(ID, pixelShader);
		glLinkProgram(ID);
		glGetProgramiv(ID, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(ID, 512, NULL, info);
			std::cout << "ERROR: SHADER: SHADERID LINKING ERROR " << info << std::endl;
		}
		glDeleteShader(pixelShader);
		glDeleteShader(vertexShader);

		#pragma endregion

	}
	// use/activate the shader

	Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath) {
		std::string vertexShaderCode, fragmentShaderCode, geometryCode;
		std::ifstream vertexStream;
		std::ifstream fragmentStream;
		std::ifstream geometryStream;
		//----------------------------------------------------------------------------------------
		//read the shader codes from the files
		//----------------------------------------------------------------------------------------
#pragma region shaderRetrieve
		try {
			vertexStream.open(vertexPath);
			fragmentStream.open(fragmentPath);
			geometryStream.open(geometryPath);
			vertexShaderCode.assign((std::istreambuf_iterator<char>(vertexStream)), (std::istreambuf_iterator<char>()));	//inside assign a string consructor with 2 interators; first set to iterate over the stream, last one is a standard iterator which returns true until the first one rusn out of data
			fragmentShaderCode.assign((std::istreambuf_iterator<char>(fragmentStream)), (std::istreambuf_iterator<char>()));
			geometryCode.assign((std::istreambuf_iterator<char>(geometryStream)), (std::istreambuf_iterator<char>()));
			vertexStream.close();
			fragmentStream.close();
			geometryStream.close();
		}
		catch (std::ifstream::failure e) {
			std::cout << "ERROR: SHADER: INVALID SHADERFILES " << vertexPath << " " << fragmentPath << " COULD NOT BE READ CORRECTLY" << std::endl;
		}
#pragma endregion

		//----------------------------------------------------------------------------------------
		//compile the fragment and vertex shader
		//----------------------------------------------------------------------------------------
#pragma region shaderCompilation

		const char* vertexShaderC(vertexShaderCode.c_str());
		const char* fragmentShaderC(fragmentShaderCode.c_str());
		const char* geometryShaderC(geometryCode.c_str());
		unsigned int vertexShader, pixelShader, geometryShader;
		int success;
		char info[512];
		vertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexShader, 1, &vertexShaderC, NULL);
		glCompileShader(vertexShader);
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(vertexShader, 512, NULL, info);
			std::cout << "ERROR: SHADER: COMPILATION ERROR VERTEXSHADER " << info << std::endl;
		}

		pixelShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(pixelShader, 1, &fragmentShaderC, NULL);
		glCompileShader(pixelShader);
		glGetShaderiv(pixelShader, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(pixelShader, 512, NULL, info);
			std::cout << "ERROR: SHADER: COMPILATION ERROR PIXELSHADER " << info << std::endl;
		}

		geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(geometryShader, 1, &geometryShaderC, NULL);
		glCompileShader(geometryShader);
		glGetShaderiv(geometryShader, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(geometryShader, 512, NULL, info);
			std::cout << "ERROR: SHADER: COMPILATION ERROR PIXELSHADER " << info << std::endl;
		}

#pragma endregion

		//----------------------------------------------------------------------------------------
		//create shader ID and link the shaders from above
		//----------------------------------------------------------------------------------------
#pragma region shaderIDInitilization

		ID = glCreateProgram();
		glAttachShader(ID, vertexShader);
		glAttachShader(ID, geometryShader);
		glAttachShader(ID, pixelShader);
		glLinkProgram(ID);
		glGetProgramiv(ID, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(ID, 512, NULL, info);
			std::cout << "ERROR: SHADER: SHADERID LINKING ERROR " << info << std::endl;
		}
		glDeleteShader(pixelShader);
		glDeleteShader(vertexShader);
		glDeleteShader(geometryShader);
#pragma endregion

	}

	void use() {
		glUseProgram(ID);
	}


	// utility uniform functions
	void setBool(const std::string &name, bool value) const {
		glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
	}
	void setInt(const std::string &name, int value) const {
		glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
	}
	void setFloat(const std::string &name, float value) const {
		glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
	}

	void setMat4(const std::string &name, glm::mat4 value) {
		int modelLoc = glGetUniformLocation(ID, name.c_str());
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(value));
	}
};

#endif