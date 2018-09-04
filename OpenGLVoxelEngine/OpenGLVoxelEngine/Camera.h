#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>

#include <iostream>

class Camera {

public:

	//standard constructor
	Camera() {
		cameraFront		= glm::vec3(0.0f, 0.0f, -1.0f);
		cameraPosition	= glm::vec3(0.0f, 0.0f, 3.0f);
		cameraUp		= glm::vec3(0.0f, 1.0f, 0.0f);
		yaw				= -90.0f;
		pitch			= 0.0f;
		cameraSpeed		= 10.0f;//3.65f;
		mouseSensitivity = 0.08f;
		viewMatrix = glm::lookAt(cameraPosition, cameraPosition + cameraFront, cameraUp);
	}
	Camera(glm::vec3 cameraStartPosition, glm::vec3 cameraStartFront, glm::vec3 cameraStartUp, float cameraStartSpeed = 3.65f, float mouseStartSensitivity = 0.08f) {
		//customizeable constructor - the 3 vectors must be specified, the 3 config values can be left out
		cameraPosition		= cameraStartPosition;
		cameraFront			= cameraStartFront;
		cameraUp			= cameraStartUp;
		cameraSpeed			= cameraStartSpeed;
		mouseSensitivity	= mouseStartSensitivity;
		viewMatrix			= glm::lookAt(cameraPosition, cameraPosition + cameraFront, cameraUp);
	}

	void updateCameraRotation(float mouseOffsetX, float mouseOffsetY) {
		mouseOffsetX *= mouseSensitivity;	//to reduce the strength of the rotation
		mouseOffsetY *= mouseSensitivity;
		//euler magic
		yaw += mouseOffsetX;
		pitch -= mouseOffsetY;
		if (pitch > 89.0f)
			pitch = 89.0f;
		if (pitch < -89.0f)
			pitch = -89.0f;

		glm::vec3 front;
		front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		front.y = sin(glm::radians(pitch));
		front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		cameraFront = glm::normalize(front);	
	}

	void updateCameraPosition(unsigned int wert, float deltaTime) {
		switch (wert) {
		case GLFW_KEY_W: cameraPosition += cameraSpeed * cameraFront * deltaTime;
			break;
		case GLFW_KEY_S: cameraPosition -= cameraSpeed * cameraFront * deltaTime;
			break;
		case GLFW_KEY_A: cameraPosition += glm::normalize(glm::cross(cameraUp, cameraFront)) * cameraSpeed * deltaTime;
			break;
		case GLFW_KEY_D: cameraPosition += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed * deltaTime;
			break;
		default:
			std::cout << "ERROR: CAMERA: INVALID KEYVALUE FOR MOVEMENT" << std::endl;
			break;
		}
	}

	glm::vec3 getCameraFront() {
		return cameraFront;
	}
	glm::vec3 getCameraPosition() {
		return cameraPosition;
	}
	glm::vec3 getCameraUp() {
		return cameraUp;
	}
	glm::mat4 getViewMatrix() {
		viewMatrix = glm::lookAt(cameraPosition, cameraPosition + cameraFront, cameraUp);
		return viewMatrix;
	}

	void setCameraSpeed(float newValue) {
		cameraSpeed = newValue;
	}
	void setMouseSensitivity(float newValue) {
		mouseSensitivity = newValue;
	}

private:
	glm::mat4 viewMatrix;	
	glm::vec3 cameraFront;
	glm::vec3 cameraPosition;
	glm::vec3 cameraUp;
	float yaw;
	float pitch;
	float cameraSpeed;
	float mouseSensitivity;
};
