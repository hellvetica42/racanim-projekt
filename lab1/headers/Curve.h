
#pragma once

#include <GLFW/glfw3.h>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <assimp/scene.h>           
#include <vector>

#include "Transform.h"

class Curve {
public:
	Curve();
	GLuint modelMatrixID;
	void drawCurve();
	void drawControlLines();
	void addControlPoint(glm::vec4 point);
	GLuint VAO[2];
	GLuint VBO[2];
	GLuint EBO[2];

	void setPoints(std::vector<glm::vec4> controlPoints, std::vector<glm::vec4> curvePoints);

private:
	std::vector<glm::vec4> points;
	std::vector<glm::vec4> controlPoints;
	std::vector<GLuint> indices;
	std::vector<GLuint> cpIndices;

	void createVAOandEBO();
	void updateVAOandEBO();
	void updateIndices();

};
