#pragma once

#include <GLFW/glfw3.h>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include "glm/ext.hpp"
#include "glm/gtx/string_cast.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#include <assimp/scene.h>           
#include<iostream>

#include "Mesh.h"
#include "Transform.h"
#include "Object.h"
#include "Material.h"

Object::Object(aiMesh* mesh, Material material, GLuint modelMatrixID) {

	this->mesh = Mesh(mesh, material);
	this->modelMatrixID = modelMatrixID;

	glm::vec3 mins, maxes;
	this->mesh.getBoundingBox(&mins, &maxes);
	glm::vec3 position = glm::vec3(
		(mins[0] + maxes[0]) / 2,
		(mins[1] + maxes[1]) / 2,
		(mins[2] + maxes[2]) / 2);


	this->transform = Transform(glm::vec4(position, 1));

	float M = glm::max(maxes[0] - mins[0], glm::max(maxes[1] - mins[1], maxes[2] - mins[2]));

	
	applyTransform(glm::translate(glm::mat4(1.0f), -position));
	

	glm::mat4 scalingMat = glm::mat4(2/M);

	applyTransform(scalingMat);

}


void Object::applyTransform(glm::mat4 transformMat) {
	mesh.applyTransform(transformMat);
	transform.setPosition(transformMat * transform.getPosition());
}

