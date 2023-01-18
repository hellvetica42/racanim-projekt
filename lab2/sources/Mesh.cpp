#pragma once

#include <GLFW/glfw3.h>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include "glm/ext.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>


#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <assimp/scene.h>           

#include "Mesh.h"
#include "Material.h"
#include <vector>
#include <iostream>

#include <windows.h>
#include <string>

using namespace std;

string getCurrentDir() {
	char buff[MAX_PATH];
	GetModuleFileName(NULL, buff, MAX_PATH);
	string::size_type position = string(buff).find_last_of("\\/");
	return string(buff).substr(0, position);
}

Mesh::Mesh(aiMesh * mesh, Material material){

	aiString texturePath;
	if (AI_SUCCESS == material.material->Get(AI_MATKEY_TEXTURE(aiTextureType_DIFFUSE, 0), texturePath)) {
		std::string dirPath(getCurrentDir());
		string resPath(dirPath);
		resPath.append("\\resources");
		resPath.append("\\glava\\");
		resPath.append(texturePath.C_Str());

		textureData = stbi_load(resPath.c_str(), &width, &height, &nrChannels, 0);
		cout << "Loaded: " << resPath << endl;

		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		if (textureData) {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, textureData);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else {
			std::cout << "Failed to load texture" << std::endl;
		}

		stbi_image_free(textureData);
	}

	this->mesh = mesh;
	this->numVertices = mesh->mNumVertices;

	this->numIndices = mesh->mNumFaces * 3;
	this->indices = new unsigned int[this->numIndices];

	this->numTriangles = mesh->mNumFaces;

	for (int i = 0; i < this->numVertices; i++) {
		this->vertices.push_back(glm::vec4(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z, 1));
		this->normals.push_back(glm::vec4(mesh->mNormals[i].x,
								mesh->mNormals[i].y,
								mesh->mNormals[i].z,
								0));

		this->uvs.push_back(glm::vec2(
			mesh->mTextureCoords[0][i].x,
			1-mesh->mTextureCoords[0][i].y
		));
	}


	for (int i = 0; i < mesh->mNumFaces; i++) {
		for (int j = 0; j < mesh->mFaces[i].mNumIndices; j++) {
			int vertex = mesh->mFaces[i].mIndices[j];
			this->vIndices.push_back(vertex);
		}
	}

	createVAOandEBO();
}

void Mesh::createVAOandEBO() {
	glGenVertexArrays(1, &VAO);
	glGenBuffers(3, VBO);
	glGenBuffers(1, &EBO);



	updateVAOandEBO();

}

void Mesh::updateVAOandEBO() {

	//std::cout << sizeof(glm::vec4) << std::endl;
	glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec4), &vertices[0].x, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (void*)0);

		glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
		glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec4), &normals[0].x, GL_STATIC_DRAW);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (void*)0);

		glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
		glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0].x, GL_STATIC_DRAW);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);


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
