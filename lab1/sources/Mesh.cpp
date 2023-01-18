#pragma once

#include <GLFW/glfw3.h>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include "glm/ext.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>


#include <assimp/scene.h>           

#include "Mesh.h"
#include <vector>
#include <iostream>

Mesh::Mesh(aiMesh * mesh) {

	this->mesh = mesh;
	this->numVertices = mesh->mNumVertices;

	this->numIndices = mesh->mNumFaces * 3;
	this->indices = new unsigned int[this->numIndices];

	this->numTriangles = mesh->mNumFaces;

	for (int i = 0; i < this->numVertices; i++) {
		this->vertices.push_back(glm::vec4(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z, 1));
	}


	for (int i = 0; i < mesh->mNumFaces; i++) {
		this->indices[i * 3] =	   mesh->mFaces[i].mIndices[0];
		this->indices[i * 3 + 1] = mesh->mFaces[i].mIndices[1];
		this->indices[i * 3 + 2] = mesh->mFaces[i].mIndices[2];
		this->vIndices.push_back(mesh->mFaces[i].mIndices[0]);
		this->vIndices.push_back(mesh->mFaces[i].mIndices[1]);
		this->vIndices.push_back(mesh->mFaces[i].mIndices[2]);
		//std::cout << this->indices[i * 3] << std::endl;
		//std::cout << this->indices[i * 3+1] << std::endl;
		//std::cout << this->indices[i * 3+2] << std::endl;
	}

	createVAOandEBO();
}

void Mesh::createVAOandEBO() {
	glGenVertexArrays(1, &VAO);
	glGenBuffers(2, VBO);
	glGenBuffers(1, &EBO);

	updateVAOandEBO();

}

void Mesh::updateVAOandEBO() {

	glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec4), &vertices[0].x, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), 0);
		
		glEnableVertexAttribArray(0);
		//glEnableVertexAttribArray(1);

		//buffer za indekse, moze biti samo jedan GL_ELEMENT_ARRAY_BUFFER po VAO
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*this->vIndices.size(), &this->vIndices[0], GL_STATIC_DRAW);
	glBindVertexArray(0);
}

void Mesh::getBoundingBox(glm::vec3 * mins, glm::vec3 * maxes) {
	float minx, miny, minz;
	float maxx, maxy, maxz;

	minx = this->vertices[0].x;
	miny = this->vertices[0].y;
	minz = this->vertices[0].z;

	maxx = this->vertices[0].x;
	maxy = this->vertices[0].y;
	maxz = this->vertices[0].z;
	for (int i = 0; i < vertices.size(); i++) {

		if (vertices[i].x < minx)
			minx = vertices[i].x;

		if (vertices[i].x > maxx)
			maxx = vertices[i].x;

		if (vertices[i].y < miny)
			miny = vertices[i].y;

		if (vertices[i].y > maxy)
			maxy = vertices[i].y;

		if (vertices[i].z < minz)
			minz = vertices[i].z;

		if (vertices[i].z > maxz)
			maxz = vertices[i].z;
	}

	mins->x = minx;
	mins->y = miny;
	mins->z = minz;

	maxes->x = maxx;
	maxes->y = maxy;
	maxes->z = maxz;
}

void Mesh::applyTransform(glm::mat4 transformMat) {
	for (int i = 0; i < vertices.size(); i++) {
		vertices[i] = transformMat * vertices[i];
	}
	updateVAOandEBO();
}
