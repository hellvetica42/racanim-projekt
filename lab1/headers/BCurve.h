#pragma once
#include <GLFW/glfw3.h>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <assimp/scene.h>           
#include <vector>
#include "Transform.h"

class BCurve {
public:
	BCurve();
	Transform transform = Transform(glm::vec4(0, 0, 0, 1));
	static BCurve loadCurve(std::string fname);
	static BCurve createCurve(BCurve curve);
	static BCurve createTangents(BCurve curve);
	void addPoint(glm::vec4 point);

	glm::mat4x4 B;
	glm::mat3x4 tgB;


	std::vector<glm::vec4> controlPoints;

	std::vector<glm::vec4> curveLines;
	std::vector<glm::mat4x3> curveSegments;
	std::vector<glm::vec4> tangentLines;

	int renderResolution = 20;
	int tgPerSegment = 5;

	GLuint modelMatrixID;

	GLuint VAO[2];
	GLuint VBO[2];
	GLuint EBO[2];

	std::vector<GLuint> indices;
	std::vector<GLuint> cpIndices;
	std::vector<GLuint> tgIndices;

	void createVAOandEBO();
	void updateVAOandEBO();
	void updateIndices();

	void drawCurve();
	void drawTangents();
	void drawControlLines();

	glm::vec4 interpolate(int segment, float t);
	glm::vec4 interpolateRotation(int segment, float t);
private:
	void applyTransform(glm::mat4 transformMat);

};
