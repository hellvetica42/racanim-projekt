#include "Transform.h"
#include "glm/ext.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

Transform::Transform(glm::vec4 position) {
	this->position = position;

	this->directionForward = glm::vec4(0.0, 0.0, 1.0, 0.0);
	this->directionRight = glm::vec4(1.0, 0.0, 0.0, 0.0);
	this->directionUp = glm::vec4(0.0, 1.0, 0.0, 0.0);

	this->modelMatrix = glm::mat4(1.0);
	this->viewMatrix = glm::mat4(1.0);
}

void Transform::setPosition(glm::vec4 position) {
	this->position = position;
}

glm::vec4 Transform::getPosition() {
	return this->position;
}

void Transform::translate(glm::vec3 translation) {
	this->modelMatrix = glm::translate(modelMatrix, translation);
	this->position = glm::translate(glm::mat4(1.0), translation) * position;
}

void Transform::translateForward(float amount) {
	this->modelMatrix = glm::translate(modelMatrix, glm::vec3(directionForward) * amount);
	this->position = position + amount * directionForward;
}

void Transform::translateRight(float amount) {
	this->modelMatrix = glm::translate(modelMatrix, glm::vec3(directionRight) * amount);
	this->position = position + amount * directionRight;
}

void Transform::translateUp(float amount) {
	this->modelMatrix = glm::translate(modelMatrix, glm::vec3(directionUp) * amount);
	this->position = position + amount * directionUp;
}

void Transform::rotateDirection(float xoffset, float yoffset) {
	yaw += xoffset;
	pitch += yoffset;

	glm::vec3 direction;
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	directionForward = glm::vec4(glm::normalize(direction), 0);
	directionRight = glm::vec4(glm::normalize(glm::cross(glm::vec3(directionForward), glm::vec3(directionUp))), 0);
}


void Transform::rotate(glm::vec3 axis, float angle) {
	glm::mat4 rotMat = glm::rotate(angle, axis);

}

void Transform::rotateLocalX(float angle) {
	glm::mat4 rotMat = glm::rotate(angle, glm::vec3(directionRight));

	this->modelMatrix =  rotMat * modelMatrix;

	this->directionForward = rotMat * directionForward;
	this->directionUp = rotMat * directionUp;
}

void Transform::rotateLocalY(float angle) {
	glm::mat4 rotMat = glm::rotate(angle, glm::vec3(directionUp));

	this->modelMatrix =  rotMat * modelMatrix;

	this->directionForward = rotMat * directionForward;
	this->directionRight = rotMat * directionRight;
}

void Transform::rotateGlobalY(float angle) {
	glm::mat4 rotMat = glm::rotate(angle, glm::vec3(0,1,0));
	this->position = rotMat * position;
	this->directionForward = rotMat * directionForward;
	this->directionRight = rotMat * directionRight;
}

glm::mat4 Transform::getViewMatrix() {
	return glm::lookAt(
		glm::vec3(this->position),
		glm::vec3(this->position + this->directionForward),
		glm::vec3(0.0, 1.0, 0.0)
	);
}

glm::mat4 Transform::getModelMatrix() {
	return modelMatrix;
}