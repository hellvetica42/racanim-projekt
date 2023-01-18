#pragma once

#include <GLFW/glfw3.h>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include "glm/ext.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>


#include <assimp/scene.h>           

#include "Curve.h"
#include <vector>
#include <iostream>

Curve::Curve() {
}

void Curve::updateIndices() {
	this->indices.clear();
	this->cpIndices.clear();
	for (int i = 0; i < this->points.size()-1; i++) {
		this->indices.push_back(i);
		this->indices.push_back(i+1);
	}

	for (int i = 0; i < this->controlPoints.size()-1; i++) {
		this->cpIndices.push_back(i);
		this->cpIndices.push_back(i+1);
	}
}

void Curve::addControlPoint(glm::vec4 point) {
	if (controlPoints.size() == 0) {
		createVAOandEBO();
	}
	this->controlPoints.push_back(point);
	if (controlPoints.size() >= 2) {
		updateIndices();
		updateVAOandEBO();
	}
}

void Curve::setPoints(std::vector<glm::vec4> controlPoints, std::vector<glm::vec4> curvePoints) {
	if (this->controlPoints.size() == 0) {
		createVAOandEBO();
		std::cout << "Created VAO" << std::endl;
	}
	this->controlPoints.clear();
	for (int i = 0; i < controlPoints.size(); i++) {
		//std::cout << controlPoints[i].x<<" " << controlPoints[i].y<<" " << controlPoints[i].z << std::endl;
		this->controlPoints.push_back(controlPoints[i]);
	}
	this->points.clear();
	for (int i = 0; i < curvePoints.size(); i++) {
		//std::cout <<"Curvepts: " <<curvePoints[i].x<<" " << curvePoints[i].y<<" " << curvePoints[i].z << std::endl;
		this->points.push_back(curvePoints[i]);
	}
	updateIndices();
	updateVAOandEBO();
}


void Curve::createVAOandEBO() {
	glGenVertexArrays(2, VAO);
	glGenBuffers(2, VBO);
	glGenBuffers(2, EBO);

}

void Curve::updateVAOandEBO() {


	glBindVertexArray(VAO[0]);
		glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
		glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(glm::vec4), &points[0].x, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (GLvoid*)0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[0]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*this->indices.size(), &this->indices[0], GL_STATIC_DRAW);
	glBindVertexArray(0);

	glBindVertexArray(VAO[1]);

		glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
		glBufferData(GL_ARRAY_BUFFER, controlPoints.size() * sizeof(glm::vec4), &controlPoints[0].x, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (GLvoid*)0);
		glEnableVertexAttribArray(0);
		

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[1]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*this->cpIndices.size(), &this->cpIndices[0], GL_STATIC_DRAW);
	glBindVertexArray(0);
}

void Curve::drawCurve() {
	if (points.size() > 0) {

		glBindVertexArray(VAO[0]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[0]);
		glDrawElements(GL_LINES, this->points.size(), GL_UNSIGNED_INT, 0);

	}
}

void Curve::drawControlLines() {
	if (points.size() > 0) {

		glBindVertexArray(VAO[1]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[1]);
		glDrawElements(GL_LINES, this->controlPoints.size(), GL_UNSIGNED_INT, 0);
	}
}

