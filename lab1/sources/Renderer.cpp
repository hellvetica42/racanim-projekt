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
#include "Renderer.h"

Renderer::Renderer() {

}

void Renderer::addObject(Object * o) {
	this->objects.push_back(o);
}

void Renderer::renderAll() {

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	for (int i = 0; i < objects.size(); i++) {

		glBindVertexArray(objects[i]->mesh.VAO);
			glUniformMatrix4fv(objects[i]->modelMatrixID, 1, GL_FALSE, &objects[i]->transform.getModelMatrix()[0][0]);
			glDrawElements(GL_TRIANGLES, objects[i]->mesh.numTriangles * 3, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

	}
}

void Renderer::renderSingle(int idx) {

glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glBindVertexArray(objects[idx]->mesh.VAO);
		glUniformMatrix4fv(objects[idx]->modelMatrixID, 1, GL_FALSE, &objects[idx]->transform.getModelMatrix()[0][0]);
		glDrawElements(GL_TRIANGLES, objects[idx]->mesh.numTriangles * 3, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

