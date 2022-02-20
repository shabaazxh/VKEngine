#pragma once


#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>

class Input {
public:	
	Input() = default;
	Input(GLFWwindow* window, glm::vec3 cameraPos, glm::vec3 cameraFront, glm::vec3 cameraUp)
		: window{ window }, cameraPos{ cameraPos }, cameraFront{ cameraFront }, cameraUp{ cameraUp } {}

	Input(const Input& camera) {
		this->cameraPos = camera.cameraPos;
		this->cameraFront = camera.cameraFront;
		this->cameraUp = camera.cameraUp;
	}

	glm::vec3 getCameraPos() { return cameraPos; }
	glm::vec3 getCameraFront() { return cameraFront; }
	glm::vec3 getcameraUp() { return cameraUp; }

	void setCameraPos(glm::vec3 pos) {
		cameraPos = pos;
	}

	void setCameraSettings(glm::vec3 cameraPos, glm::vec3 cameraFront, glm::vec3 cameraUp);
	void mouse_callback(GLFWwindow* window, double xOffset, double yOffset);

private:
	glm::vec3 cameraPos;
	glm::vec3 cameraFront;
	glm::vec3 cameraUp;
	GLFWwindow* window;

	bool firstMouse = true;
	float yaw = -90.0f;
	float pitch = 0.0f;
	float lastX = 1920 / 2.0f;
	float lastY = 1080 / 2.0f;
	float fov = 45.0f;
};

