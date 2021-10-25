#include "Input.h"

void Input::setCameraSettings(glm::vec3 cameraPos, glm::vec3 cameraFront, glm::vec3 cameraUp) {

	this->cameraPos = cameraPos;
	this->cameraFront = cameraFront;
	this->cameraUp = cameraUp;

}
