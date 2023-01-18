#include "Camera.h"

#include "glm/ext.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

Camera::Camera(glm::vec3 pos, int width, int height) : Transform(glm::vec4(pos, 1.0)) {
	this->width = width;
	this->height = height;
}

glm::mat4 Camera::getProjectionMatrix() {
	return glm::perspective(
		(float)glm::radians(90.0),
		(float)width/height,
		1.0f,
		100.0f
	);
}