
#include <GLFW/glfw3.h>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include "glm/ext.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "glm/ext.hpp"
#include "glm/gtx/string_cast.hpp"
#include <glm/gtx/transform.hpp>


#include <assimp/scene.h>           

#include "BCurve.h"
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

using namespace std;

BCurve::BCurve() {
	this->createVAOandEBO();
}

void BCurve::addPoint(glm::vec4 point) {
	this->controlPoints.push_back(point);
}

BCurve BCurve::loadCurve(std::string fname) {
	BCurve curve = BCurve();
	vector<float> row;
	std::string line, word;

	fstream file(fname, ios::in);
	if (file.is_open()) {
		while (getline(file, line)) {
			row.clear();
			stringstream str(line);
			while (getline(str, word, ','))
				row.push_back(stof(word));
			glm::vec4 point = glm::vec4(row[0], row[1], row[2], 1.0);
			curve.addPoint(point);
		}
	}
	else
		cout << "Could not open the file\n";

	curve = BCurve::createCurve(curve);
	return curve;
}

BCurve BCurve::createCurve(BCurve curve) {

	curve.B = glm::mat4x4(
		glm::vec4(-1, 3, -3, 1),
		glm::vec4(3, -6, 3, 0),
		glm::vec4(-3, 0, 3, 0),
		glm::vec4(1, 4, 1, 0)
	);
	curve.tgB = glm::mat3x4(
		glm::vec4(-1, 3, -3, 1),
		glm::vec4(2, -4, 2, 0),
		glm::vec4(-1, 0, 1, 0)
	);
	//	std::cout<<"B: " << glm::to_string(curve.B) << std::endl;

	//curve.B = glm::transpose(curve.B);

	for (int i = 0; i < curve.controlPoints.size() - 3; i++) {
		glm::mat4x3 R = glm::mat4x3(
			glm::vec3(curve.controlPoints[i]),
			glm::vec3(curve.controlPoints[i + 1]),
			glm::vec3(curve.controlPoints[i + 2]),
			glm::vec3(curve.controlPoints[i + 3])
		);
		curve.curveSegments.push_back(R);
		//std::cout<<"R: " << glm::to_string(R) << std::endl;
	}

	for (int segment = 0; segment < curve.curveSegments.size(); segment++) {

		for (int ti = 0; ti < curve.renderResolution; ti++) {
			float t = (float)ti / ((float)curve.renderResolution);
			glm::vec4 T = glm::vec4(glm::pow(t, 3), glm::pow(t, 2), t, 1);

			glm::vec3 tmpPoint = curve.curveSegments[segment] * (curve.B * T);
			tmpPoint /= 6;
			glm::vec4 point = glm::vec4(tmpPoint, 1);

			curve.curveLines.push_back(point);

			if (ti % curve.tgPerSegment == 0) {
				glm::vec3 tgT = glm::vec3(glm::pow(t, 2), t, 1);
				glm::vec3 tgDir = curve.curveSegments[segment] * (curve.tgB * tgT);
				tgDir /= 2;
				curve.tangentLines.push_back(point);
				curve.tangentLines.push_back(point + glm::vec4(tgDir, 1.0));
			}

		}
	}

	curve.updateIndices();
	curve.updateVAOandEBO();
	return curve;
}

glm::vec4 BCurve::interpolate(int segment, float t) {
	glm::vec4 T = glm::vec4(glm::pow(t, 3), glm::pow(t, 2), t, 1);

	glm::vec3 tmpPoint = curveSegments[segment] * (B * T);
	tmpPoint /= 6;
	glm::vec4 point = glm::vec4(tmpPoint, 1);
	return point;
}

glm::vec4 BCurve::interpolateRotation(int segment, float t) {
		glm::vec3 tgT = glm::vec3(glm::pow(t, 2), t, 1);
		glm::vec3 tgDir = curveSegments[segment] * (tgB * tgT);
		tgDir /= 2;

		//curve.tangentLines.push_back(point + glm::vec4(tgDir, 1.0));
		return glm::vec4(tgDir, 0);
}

void BCurve::updateIndices() {
	this->indices.clear();
	for (int i = 0; i < this->curveLines.size() - 1; i++) {
		this->indices.push_back(i);
		this->indices.push_back(i + 1);
	}

	this->tgIndices.clear();
	for (int i = 0; i < this->tangentLines.size(); i++) {
		this->tgIndices.push_back(i);
	}
}

void BCurve::applyTransform(glm::mat4 transformMat) {
	for (int i = 0; i < curveLines.size(); i++) {
		curveLines[i] = transformMat * curveLines[i];
	}
	transform.setPosition(transformMat * transform.getPosition());
}

void BCurve::createVAOandEBO() {
	glGenVertexArrays(2, VAO);
	glGenBuffers(2, VBO);
	glGenBuffers(2, EBO);
}

void BCurve::updateVAOandEBO() {
	glBindVertexArray(VAO[0]);
		glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
		glBufferData(GL_ARRAY_BUFFER, curveLines.size() * sizeof(glm::vec4), &curveLines[0].x, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (GLvoid*)0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[0]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*this->indices.size(), &this->indices[0], GL_STATIC_DRAW);
	glBindVertexArray(0);

	glBindVertexArray(VAO[1]);
		glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
		glBufferData(GL_ARRAY_BUFFER, tangentLines.size() * sizeof(glm::vec4), &tangentLines[0].x, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (GLvoid*)0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[1]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*this->tgIndices.size(), &this->tgIndices[0], GL_STATIC_DRAW);
	glBindVertexArray(0);
}

void BCurve::drawCurve() {
	if (curveLines.size() <= 0)
		return;

	glBindVertexArray(VAO[0]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[0]);
	glLineWidth(3);
	glDrawElements(GL_LINES, curveLines.size(), GL_UNSIGNED_INT, 0);
	glLineWidth(1);

}

void BCurve::drawTangents() {
	glBindVertexArray(VAO[1]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[1]);
	glDrawElements(GL_LINES, tangentLines.size(), GL_UNSIGNED_INT, 0);
}

