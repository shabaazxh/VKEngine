#pragma once


#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>
class Input {
public:	
	glm::vec3 getCameraPos() { return cameraPos; }
	glm::vec3 getCameraFront() { return cameraFront; }
	glm::vec3 getcameraUp() { return cameraUp; }

	void setCameraSettings(GLFWwindow* window, glm::vec3 cameraPos, glm::vec3 cameraFront, glm::vec3 cameraUp);

private:
	glm::vec3 cameraPos;
	glm::vec3 cameraFront;
	glm::vec3 cameraUp;
	GLFWwindow* window;
};