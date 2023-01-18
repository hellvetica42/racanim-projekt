#pragma once

#include <GLFW/glfw3.h>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include "glm/ext.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>


#include <assimp/scene.h>           

#include "CurveCreator.h"
#include <vector>
#include <iostream>

CurveCreator::CurveCreator() {
}

std::vector<glm::vec4> CurveCreator::getApproxCurve(std::vector<glm::vec4> controlPoints, int sampleRate) {

	std::vector<glm::vec4> segments;

	glm::vec4 p;
	int n = controlPoints.size() - 1;
	int *factors = (int*)malloc(sizeof(int) * (n + 1));
	double t, b;

	//std::cout << "N: " << n << std::endl;

	computeFactors(n, factors);

	for (int i = 1; i <= sampleRate; i++) {
		t = 1.0 / sampleRate * i;
		p.x = 0; p.y = 0; p.z = 0;
		for (int j = 0; j <= n; j++) {
			if (j == 0) {
				b = factors[j] * pow(1 - t, n);
			}
			else if (j == n) {
				b = factors[j] * pow(t, n);
			}
			else {
				b = factors[j] * pow(t, j) * pow(1 - t, n - j);
			}
			p.x += b * controlPoints[j].x;
			p.y += b * controlPoints[j].y;
			p.z += b * controlPoints[j].z;
			p.w = controlPoints[j].w;
			//std::cout << "CTRLPTS: " << controlPoints[j].x << " " << controlPoints[j].y << " "<<controlPoints[j].z << std::endl;
		}
		segments.push_back(p);
	}
	free(factors);
	return segments;
}
void CurveCreator::computeFactors(int n, int*factors){
		int i, a = 1;
		for (i = 1; i <= n + 1; i++) {
			factors[i - 1] = a;
			a = a * (n - i + 1) / i;
		}
}

