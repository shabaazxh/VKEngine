#include "Input.h"

void Input::setCameraSettings(glm::vec3 cameraPos, glm::vec3 cameraFront, glm::vec3 cameraUp) {

	this->cameraPos = cameraPos;
	this->cameraFront = cameraFront;
	this->cameraUp = cameraUp;

}


void Input::mouse_callback(GLFWwindow* window, double xOffset, double yOffset) {

	if (firstMouse)
	{
		lastX = xOffset;
		lastY = yOffset;
		firstMouse = false;
	}

	float xoffset =  xOffset - lastX;
	float yoffset = lastY - yOffset; // reversed since y-coordinates go from bottom to top
	lastX = xOffset;
	lastY = yOffset;

	float sensitivity = 0.1f; // change this value to your liking
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	// make sure that when pitch is out of bounds, screen doesn't get flipped
	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	glm::vec3 direction;
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(direction);

	//std::cout << cameraFront.x << " " << cameraFront.y << " " << cameraFront.z << std::endl;
}
