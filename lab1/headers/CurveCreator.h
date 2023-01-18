#pragma once
#include <GLFW/glfw3.h>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <assimp/scene.h>           
#include <vector>

class CurveCreator {
public:
	CurveCreator();
	std::vector<glm::vec4> getApproxCurve(std::vector<glm::vec4> controlPoints, int sampleRate);
	//std::vector<glm::vec3> getInterpCurve(int sampleRate);

private:
	std::vector<glm::vec4> controlPoints;
	void computeFactors(int n, int* factors);
};
